#pragma once
#include <string>
#include <map>
#include <functional>

// CommandRequest class definition
class CommandRequest {
public:
    std::string transaction_id;
    std::string command;
    std::map<std::string, std::string> params;

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
    void invoke_command(const CommandRequest& request);
    void on_command_response(std::function<void(CommandResponse)> fn);
};
