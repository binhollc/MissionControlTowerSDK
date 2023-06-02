#include "mct_api.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#ifdef _WIN32
    #include <io.h>
#else
    #include <unistd.h>
#endif

CommandManager::CommandManager() : isRunning(false) {}

void CommandManager::start() {
    isRunning = true;

    // Start the bridge process
    #ifdef _WIN32
        bridgeProcess = _popen("bridge.exe", "r+");
    #else
        bridgeProcess = popen("bridge", "r+");
    #endif

    // Initialize the bridge reader class
    bridgeReader = std::make_unique<BridgeReader>(bridgeProcess);

    // Start the write bridge thread
    writeBridgeThread = std::thread(&CommandManager::handleWriteBridgeThread, this);

    // Start the read bridge thread
    readBridgeThread = std::thread(&CommandManager::handleReadBridgeThread, this);

    // Start the callback on response thread
    callbackOnResponseThread = std::thread(&CommandManager::handleCallbackOnResponseThread, this);
}

void CommandManager::stop() {
    isRunning = false;
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
    while (isRunning) {
        std::unique_lock<std::mutex> lock(requestMutex);
        requestCV.wait(lock, [this]{ return !requestQueue.empty(); });

        // Process the request
        CommandRequest request = requestQueue.front();
        requestQueue.pop();

        lock.unlock();

        // Convert the request to JSON and write to bridge's stdin
        nlohmann::json j;
        j["transaction_id"] = request.transaction_id;
        j["command"] = request.command;
        j["params"] = request.params;

        std::string jsonString = j.dump();

        std::cout << "Command read from queue: " << jsonString << "\n";

        // write jsonString to bridge's stdin
        if (bridgeProcess != nullptr) {
            fputs(jsonString.c_str(), bridgeProcess);
            fflush(bridgeProcess); // make sure jsonString is written immediately
        }
    }
}

void CommandManager::handleReadBridgeThread() {
    while (isRunning) {
        // read from bridge's stdout
        std::string jsonString = bridgeReader->readNextData();
        while (!jsonString.empty()) {
            std::cout << "Command response read from bridge: " << jsonString << "\n";

            // Parse the JSON string and enqueue a CommandResponse
            nlohmann::json j = nlohmann::json::parse(jsonString);
            CommandResponse response(j["transaction_id"], j["status"], j["is_promise"], j["data"]);

            std::lock_guard<std::mutex> lock(responseMutex);
            responseQueue.push(response);
            responseCV.notify_one();

            // Try to read the next data
            jsonString = bridgeReader->readNextData();
        }
    }
}

void CommandManager::handleCallbackOnResponseThread() {
    while (isRunning) {
        std::unique_lock<std::mutex> lock(responseMutex);
        responseCV.wait(lock, [this]{ return !responseQueue.empty(); });

        // Process the response and pass it to the callback function
        CommandResponse response = responseQueue.front();
        responseQueue.pop();

        callback_fn(response);

        lock.unlock();
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
