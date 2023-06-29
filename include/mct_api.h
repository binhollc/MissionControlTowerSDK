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

using json = nlohmann::json;

// CommandRequest struct definition
struct CommandRequest {
    std::string transaction_id;
    std::string command;
    json params;

    CommandRequest()
        : transaction_id("0"), command(""), params(json::object()) {}

    CommandRequest(const std::string& id, const std::string& cmd)
        : transaction_id(id), command(cmd), params(json::object()) {}

    CommandRequest(const std::string& id, const std::string& cmd, const json& prms)
        : transaction_id(id), command(cmd), params(prms) {}
};

// CommandResponse struct definition
struct CommandResponse {
    std::string transaction_id;
    std::string status;
    bool is_promise;
    json data;

    CommandResponse()
        : transaction_id(""), status(""), is_promise(false), data(json::object()) {}

    CommandResponse(const std::string& id, const std::string& st, bool pr, const json& dt)
        : transaction_id(id), status(st), is_promise(pr), data(dt) {}
};

// Nlohmann json serialization and deserialization functions
inline void to_json(json& j, const CommandRequest& p) {
    j = json{{"transaction_id", p.transaction_id}, {"command", p.command}, {"params", p.params}};
}

inline void from_json(const json& j, CommandRequest& p) {
    j.at("transaction_id").get_to(p.transaction_id);
    j.at("command").get_to(p.command);
    j.at("params").get_to(p.params);
}

inline void to_json(json& j, const CommandResponse& p) {
    j = json{{"transaction_id", p.transaction_id}, {"status", p.status}, {"is_promise", p.is_promise}, {"data", p.data}};
}

inline void from_json(const json& j, CommandResponse& p) {
    j.at("transaction_id").get_to(p.transaction_id);
    j.at("status").get_to(p.status);
    j.at("is_promise").get_to(p.is_promise);
    j.at("data").get_to(p.data);
}

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
    bool isRunningWriteThread;
    bool isRunningReadThread;
    bool isRunningCallbackThread;
    std::unique_ptr<BridgeReader> bridgeReader;
};
