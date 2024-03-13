#pragma once

#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <functional>
#include "definitions.h"
#include "CommandManager.h"

using json = nlohmann::json;

class CommandManager;

class BMC_SDK CommandDispatcher {
private:
    CommandManager commandManager;
    std::mutex mtx;
    std::condition_variable cv;
    std::unordered_map<std::string, int> activeCommands;
    std::unordered_map<std::string, std::function<void(CommandResponse)>> callbacks;
    std::function<void(CommandResponse)> notificationCallback;

public:
    CommandDispatcher(const std::string &targetCommandAdaptorName) : commandManager(targetCommandAdaptorName) {}
    void start();
    void stop();
    void invokeCommand(const std::string& id, const std::string& cmd, const json& params = json::object(), std::function<void(CommandResponse)> fn = nullptr);
    void waitFor(const std::vector<std::string>& ids);
    void waitForAllCommands();
    void invokeCommandSync(const std::string& id, const std::string& cmd, const json& params = json::object(), std::function<void(CommandResponse)> fn = nullptr);
    void onNotification(std::function<void(CommandResponse)> fn);
};
