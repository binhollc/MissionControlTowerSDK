#include "mct_api.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#ifdef _WIN32
    #include <io.h>
#else
    #include <unistd.h>
#endif

CommandManager::CommandManager() : isRunningWriteThread(false), isRunningReadThread(false), isRunningCallbackThread(false) {}

void CommandManager::start() {
    // Start the bridge process
    #ifdef _WIN32
        bridgeProcess = _popen("bridge.exe BinhoNova", "r+");
    #else
        bridgeProcess = popen("bridge BinhoNova", "r+");
    #endif

    // Initialize the bridge reader class
    bridgeReader = std::make_unique<BridgeReader>(bridgeProcess);

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

    if (bridgeProcess != nullptr) {
        #ifdef _WIN32
            _pclose(bridgeProcess);
        #else
            pclose(bridgeProcess);
        #endif
    }
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

        // std::cout << "Command read from queue: " << jsonString << "\n";

        // write jsonString to bridge's stdin
        if (bridgeProcess != nullptr) {
            fputs(jsonString.c_str(), bridgeProcess);
            fflush(bridgeProcess); // make sure jsonString is written immediately
        }

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
            // std::cout << "Command response read from bridge: " << jsonString << "\n";

            // Parse the JSON string and enqueue a CommandResponse
            nlohmann::json j = nlohmann::json::parse(jsonString);
            CommandResponse response;
            from_json(j, response);

            std::lock_guard<std::mutex> lock(responseMutex);
            responseQueue.push(response);
            responseCV.notify_one();
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
