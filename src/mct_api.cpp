#include "mct_api.h"
#include <iostream>

CommandManager::CommandManager() : isRunning(false) {}

void CommandManager::start() {
    isRunning = true;

    // Start the request thread
    requestThread = std::thread(&CommandManager::handleRequestThread, this);

    // Start the response thread
    responseThread = std::thread(&CommandManager::handleResponseThread, this);
}

void CommandManager::stop() {
    isRunning = false;
    requestCV.notify_all();
    responseCV.notify_all();

    if (requestThread.joinable()) {
        requestThread.join();
    }

    if (responseThread.joinable()) {
        responseThread.join();
    }
}

void CommandManager::handleRequestThread() {
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
        // TODO: Write jsonString to bridge's stdin
    }
}

void CommandManager::handleResponseThread() {
    while (isRunning) {
        // TODO: Read from bridge's stdout

        // Parse the JSON string and enqueue a CommandResponse
        // TODO: Replace the below example JSON string with the real string read from bridge's stdout
        std::string jsonString = "{\"transaction_id\": \"524\", \"status\": \"ok\", \"is_promise\": \"false\", \"data\": \"mock data\"}";

        nlohmann::json j = nlohmann::json::parse(jsonString);
        CommandResponse response(j["transaction_id"], j["status"], j["is_promise"], j["data"]);

        {
            std::lock_guard<std::mutex> lock(responseMutex);
            responseQueue.push(response);
        }

        responseCV.notify_one();

        // Call the callback function
        callback_fn(response);
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
