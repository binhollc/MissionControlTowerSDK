#pragma once
#include <string>
#include <map>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <nlohmann/json.hpp>
#include "bridge_reader.h"

// CommandRequest class definition
class CommandRequest {
public:
    std::string transaction_id;
    std::string command;
    std::map<std::string, std::string> params;

    CommandRequest()
        : transaction_id("0"), command(""), params() {}

    CommandRequest(const std::string& id, const std::string& cmd, const std::map<std::string, std::string>& prms)
        : transaction_id(id), command(cmd), params(prms) {}
};

// CommandResponse class definition
class CommandResponse {
public:
    std::string transaction_id;
    std::string status;
    std::string is_promise;
    std::string data;

    CommandResponse(const std::string& id, const std::string& st, const std::string& pr, const std::string& dt)
        : transaction_id(id), status(st), is_promise(pr), data(dt) {}
};

// CommandManager class definition
class CommandManager {
public:
    CommandManager();  // Constructor
    void start();
    void stop();
    void invoke_command(const CommandRequest& request);
    void on_command_response(std::function<void(CommandResponse)> fn);
private:
    FILE* bridgeProcess;
    std::queue<CommandRequest> requestQueue;
    std::queue<CommandResponse> responseQueue;
    std::mutex requestMutex;
    std::mutex responseMutex;
    std::condition_variable requestCV;
    std::condition_variable responseCV;
    std::thread writeBridgeThread;
    std::thread readBridgeThread;
    std::thread callbackOnResponseThread;
    void handleWriteBridgeThread();
    void handleReadBridgeThread();
    void handleCallbackOnResponseThread();
    std::function<void(CommandResponse)> callback_fn;
    bool isRunning;
    std::unique_ptr<BridgeReader> bridgeReader;
};
