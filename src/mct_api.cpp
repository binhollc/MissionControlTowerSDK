#include "mct_api.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#ifdef _WIN32
    #include <io.h>
    #include <windows.h>
#else
    #include <unistd.h>
#endif
#ifdef _WIN32
    #include "BridgeReader_windows.h"
#else
    #include "bridge_reader.h"
#endif


void CommandManager::start() {
    // Start the bridge process
    #ifdef _WIN32
        // Create pipes for input and output
        HANDLE BridgeProcess_IN_Rd = NULL;
        HANDLE BridgeProcess_IN_Wr = NULL;
        HANDLE BridgeProcess_OUT_Rd = NULL;
        HANDLE BridgeProcess_OUT_Wr = NULL;

        SECURITY_ATTRIBUTES saAttr = { sizeof(SECURITY_ATTRIBUTES) };
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;

        // Create a pipe for the child process's STDOUT.
        if (!CreatePipe(&BridgeProcess_OUT_Rd, &BridgeProcess_OUT_Wr, &saAttr, 0)) {
            std::cerr << "STDOUT CreatePipe failed" << std::endl;
            return;
        }

        // Ensure the read handle to the pipe for STDOUT is not inherited.
        if (!SetHandleInformation(BridgeProcess_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
            std::cerr << "Stdout SetHandleInformation" << std::endl;
            return;
        }

        // Create a pipe for the child process's STDIN. 
        if (!CreatePipe(&BridgeProcess_IN_Rd, &BridgeProcess_IN_Wr, &saAttr, 0)) {
            std::cerr << "STDIN CreatePipe failed" << std::endl;
            return;
        }

        // Ensure the read handle to the pipe for STDIN is not inherited.
        if (!SetHandleInformation(BridgeProcess_IN_Wr, HANDLE_FLAG_INHERIT, 0)) {
            std::cerr << "STDIN SetHandleInformation" << std::endl;
            return;
        }

        // Set up members of the PROCESS_INFORMATION structure.
        PROCESS_INFORMATION procInfo = { 0 };

        // Set up members of the STARTUPINFO structure.
        // This structure specifies the STDIN and STDOUT handles for redirection.
        STARTUPINFO startInfo = { 0 };
        startInfo.cb = sizeof(STARTUPINFO);
        startInfo.hStdError = BridgeProcess_OUT_Wr;
        startInfo.hStdOutput = BridgeProcess_OUT_Wr;
        startInfo.hStdInput = BridgeProcess_IN_Rd;
        startInfo.dwFlags |= STARTF_USESTDHANDLES;

        // Create the child process.
        std::string command = "bridge.exe " + targetCommandAdaptor;
        BOOL bSuccess = CreateProcess(NULL, 
            (LPSTR)command.c_str(), // command line 
            NULL, // process security attributes 
            NULL, // primary thread security attributes 
            TRUE, // handles are inherited 
            0, // creation flags 
            NULL, // use parent's environment 
            NULL, // use parent's current directory 
            &startInfo, // STARTUPINFO pointer 
            &procInfo); // receives PROCESS_INFORMATION 

        // If an error occurs, exit the application.
        if (!bSuccess) {
            std::cerr << "CreateProcess failed" << std::endl;
            return;
        }
        else {
            CloseHandle(procInfo.hProcess);
            CloseHandle(procInfo.hThread);
            bridgeProcessWrite = BridgeProcess_IN_Wr;
            bridgeProcessRead = BridgeProcess_OUT_Rd;
        }
    #else
        std::string command = "bridge " + targetCommandAdaptor;
        bridgeProcess = popen(command.c_str(), "r+");
    #endif

    // Initialize the bridge reader class
    #ifdef _WIN32
        bridgeReader = std::make_unique<BridgeReader>(bridgeProcessRead);
    #else
        bridgeReader = std::make_unique<BridgeReader>(bridgeProcess);
    #endif

    // Start the write bridge thread
    isRunningWriteThread = true;
    writeBridgeThread = std::thread(&CommandManager::handleWriteBridgeThread, this);

    // Start the read bridge thread
    isRunningReadThread = true;
    readBridgeThread = std::thread(&CommandManager::handleReadBridgeThread, this);

    // Start the callback on response thread
    isRunningCallbackThread = true;
    callbackOnResponseThread = std::thread(&CommandManager::handleCallbackOnResponseThread, this);
}

void CommandManager::stop() {
    // Wait until the threads have finished running
    while (isRunningWriteThread || isRunningReadThread || isRunningCallbackThread);

    requestCV.notify_all();
    responseCV.notify_all();
    bridgeReader.reset();

    if (writeBridgeThread.joinable()) {
        writeBridgeThread.join();
    }

    if (readBridgeThread.joinable()) {
        readBridgeThread.join();
    }

    if (callbackOnResponseThread.joinable()) {
        callbackOnResponseThread.join();
    }

    #ifdef _WIN32
        if (bridgeProcessRead != NULL) {
            CloseHandle(bridgeProcessRead);
        }
        if (bridgeProcessWrite != NULL) {
            CloseHandle(bridgeProcessWrite);
        }
    #else
        if (bridgeProcess != nullptr) {
            pclose(bridgeProcess);
        }
    #endif
}

void CommandManager::handleWriteBridgeThread() {
    while (isRunningWriteThread) {
        std::unique_lock<std::mutex> lock(requestMutex);
        requestCV.wait(lock, [this]{ return !requestQueue.empty(); });

        // Process the request
        CommandRequest request = requestQueue.front();
        requestQueue.pop();

        lock.unlock();

        // Convert the request to JSON and write to bridge's stdin
        nlohmann::json j;
        to_json(j, request);

        std::string jsonString = j.dump() + "\n";

        std::cout << "Command read from queue: " << jsonString << "\n";

        // write jsonString to bridge's stdin
        #ifdef _WIN32
            if (bridgeProcessWrite != NULL) {
                DWORD bytesWritten;
                if (!WriteFile(bridgeProcessWrite, jsonString.c_str(), jsonString.length(), &bytesWritten, NULL)) {
                    // handle error here
                    std::cerr << "Error writing to bridge process stdin" << std::endl;
                }
                FlushFileBuffers(bridgeProcessWrite);
            }
        #else
            if (bridgeProcess != nullptr) {
                fputs(jsonString.c_str(), bridgeProcess);
                fflush(bridgeProcess); // make sure jsonString is written immediately
            }
        #endif

        // Break the loop upon receiving 'exit'
        if (request.command == "exit") {
            isRunningWriteThread = false;
            break;
        }
    }
}

void CommandManager::handleReadBridgeThread() {
    while (isRunningReadThread) {
        // read from bridge's stdout
        std::string jsonString = bridgeReader->readNextData();

        // Break the loop upon receiving EOF
        if (jsonString == "__EOF__") {
            isRunningReadThread = false;
            break;
        }

        if (!jsonString.empty()) {
            std::cout << "Command response read from bridge: " << jsonString << "\n";

            try {
                nlohmann::json j = nlohmann::json::parse(jsonString);
                CommandResponse response;
                std::cout << "!!!" << "\n";
                from_json(j, response);
                std::cout << "Command response parsed!" << "\n";
                
                std::lock_guard<std::mutex> lock(responseMutex);
                responseQueue.push(response);
                std::cout << "responseQueue.push(response)" << "\n";
                responseCV.notify_one();
            }
            catch (const nlohmann::json::exception& e) {
                std::cout << "Exception during parsing: " << e.what() << "\n";
                std::cout << "Failed to parse: " << jsonString << "\n";
                // handle error or rethrow
            }
        }
    }
}

void CommandManager::handleCallbackOnResponseThread() {
    while (isRunningCallbackThread) {
        std::unique_lock<std::mutex> lock(responseMutex);
        responseCV.wait(lock, [this]{ return !responseQueue.empty(); });

        // Process the response and pass it to the callback function
        CommandResponse response = responseQueue.front();
        responseQueue.pop();

        std::cout << "Command response passed to callback_fn" << "\n";

        std::string responseStatus = response.status;

        callback_fn(response);

        lock.unlock();

        // Break the loop upon receiving status == "exit"
        if (responseStatus == "exit") {
            isRunningCallbackThread = false;
            break;
        }
    }
}

void CommandManager::invoke_command(const CommandRequest& request) {
    std::lock_guard<std::mutex> lock(requestMutex);
    requestQueue.push(request);
    requestCV.notify_one();
}

void CommandManager::on_command_response(std::function<void(CommandResponse)> fn) {
    callback_fn = fn;
}
