#include "CommandManager.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <thread>
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

using json = nlohmann::json;

#define ALLOWED_VERSIONS {"0.12"}

bool checkVersionCompatibility(std::string versionStr) {
    // Verify only MAJOR and MINOR
    size_t firstDot = versionStr.find('.');
    size_t secondDot = versionStr.find('.', firstDot + 1);
    if (firstDot != std::string::npos && secondDot != std::string::npos) {
        versionStr = versionStr.substr(0, secondDot);
    } else {
        versionStr = "";
    }

    // Check if bridgeVersionNumber is in the list of allowed versions
    std::vector<std::string> allowedVersions = ALLOWED_VERSIONS;

    return (std::find(allowedVersions.begin(), allowedVersions.end(), versionStr) != allowedVersions.end());
}

std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of("\r\n");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of("\r\n");
    return str.substr(first, (last - first + 1));
}

#ifdef _WIN32
    void launchProcess(std::string command, HANDLE &hPipeStdInWrite, HANDLE &hPipeStdOutRead) {
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
        startInfo.dwFlags |= STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
        startInfo.wShowWindow = SW_HIDE;   // Prevents cmd window from flashing.
                                    // Requires STARTF_USESHOWWINDOW in dwFlags.

        // Create the child process.
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
            hPipeStdInWrite = BridgeProcess_IN_Wr;
            hPipeStdOutRead = BridgeProcess_OUT_Rd;
        }
    }
#else
    void launchProcess(std::string command, FILE* &procStream) {
        procStream = popen(command.c_str(), "r+");
    }
#endif

void CommandManager::start() {
    // Verify version compatibility
    #ifdef _WIN32
        HANDLE hPipeRead, hPipeWrite;
        launchProcess("bridge.exe --version", hPipeWrite, hPipeRead);
        bridgeReader = std::make_unique<BridgeReader>(hPipeRead);
    #else
        FILE* checkVersionStream;
        launchProcess("bridge --version", checkVersionStream);
        bridgeReader = std::make_unique<BridgeReader>(checkVersionStream);
    #endif

    std::string versionStr = trim(bridgeReader->readNextData(false)); // Non-blocking is set to false to prevent broken pipe error

    DEBUG_MSG("Read version: " + versionStr);

    // Close the pipe after attempting to read
    #ifdef _WIN32
        CloseHandle(hPipeWrite);
        CloseHandle(hPipeRead);
    #else
        if (checkVersionStream != nullptr) {
            pclose(checkVersionStream);
        }
    #endif

    if (!checkVersionCompatibility(versionStr)) {
        std::cerr << "Unsupported bridge version: " << versionStr << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // Start the bridge process
    #ifdef _WIN32
        launchProcess("bridge.exe " + targetCommandAdaptor, hPipeInputWrite, hPipeOutputRead);
    #else
        launchProcess("bridge " + targetCommandAdaptor, bridgeProcess);
    #endif

    // Initialize the bridge reader class
    #ifdef _WIN32
        bridgeReader = std::make_unique<BridgeReader>(hPipeOutputRead);
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
    DEBUG_MSG("CommandManager::stop()");

    // Wait until the threads have finished running
    while (isRunningWriteThread || isRunningReadThread || isRunningCallbackThread);

    DEBUG_MSG("CommandManager::stop() - Threads have finished running");

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

    DEBUG_MSG("About to close read and write handles");

    #ifdef _WIN32
        if (hPipeOutputRead != NULL) {
            CloseHandle(hPipeOutputRead);
        }
        if (hPipeInputWrite != NULL) {
            CloseHandle(hPipeInputWrite);
        }
    #else
        if (bridgeProcess != nullptr) {
            pclose(bridgeProcess);
        }
    #endif

    DEBUG_MSG("Handles closed");
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

        DEBUG_MSG("Command read from queue: " << jsonString);

        // write jsonString to bridge's stdin
        #ifdef _WIN32
            if (hPipeInputWrite != NULL) {
                DWORD bytesWritten;
                if (!WriteFile(hPipeInputWrite, jsonString.c_str(), jsonString.length(), &bytesWritten, NULL)) {
                    // handle error here
                    std::cerr << "Error writing to bridge process stdin" << std::endl;
                }
                FlushFileBuffers(hPipeInputWrite);
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
        std::string line = trim(bridgeReader->readNextData());

        // Break the loop upon receiving EOF
        if (line == "__EOF__") {
            DEBUG_MSG("CommandManager::handleReadBridgeThread() EOF");
            isRunningReadThread = false;
            break;
        }

        if (!line.empty()) {
            DEBUG_MSG("Command response read from bridge: " << line);

            try {
                nlohmann::json j = nlohmann::json::parse(line);
                CommandResponse response;
                from_json(j, response);

                std::lock_guard<std::mutex> lock(responseMutex);
                responseQueue.push(response);
                responseCV.notify_one();

                if (response.status == "exit") {
                    isRunningReadThread = false;
                    break;
                }
            }
            catch (const nlohmann::json::exception& e) {
                std::cerr << "Exception during parsing: " << e.what() << "\n";
                std::cerr << "Failed to parse: " << line << "\n";
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

        DEBUG_MSG("Command response passed to callback_fn");

        nlohmann::json j;
        to_json(j, response);

        DEBUG_MSG(j.dump());

        std::string responseStatus = response.status;

        callback_fn(response);

        lock.unlock();

        // Break the loop upon receiving status == "exit"
        if (responseStatus == "exit") {
            isRunningCallbackThread = false;
            break;
        }
    }
    DEBUG_MSG("CommandManager::handleCallbackOnResponseThread() EXIT");
}

void CommandManager::invoke_command(const CommandRequest& request) {
    std::lock_guard<std::mutex> lock(requestMutex);
    requestQueue.push(request);
    requestCV.notify_one();
}

void CommandManager::on_command_response(std::function<void(CommandResponse)> fn) {
    callback_fn = fn;
}
