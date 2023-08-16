#pragma once

#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <functional>
#include "mct_api.h"

using json = nlohmann::json;

#ifdef BUILD_MCT_API  // This macro is defined when building the library
    #define MCT_API __declspec(dllexport)
#else
    #define MCT_API __declspec(dllimport)
#endif

class CommandManager;

class MCT_API CommandDispatcher {
private:
    CommandManager commandManager;
    std::mutex mtx;
    std::condition_variable cv;
    std::unordered_map<std::string, int> activeCommands;
    std::unordered_map<std::string, std::function<void(CommandResponse)>> callbacks;

public:
    CommandDispatcher(const std::string &targetCommandAdaptorName) : commandManager(targetCommandAdaptorName) {}
    void start();
    void stop();
    void invokeCommand(const std::string& id, const std::string& cmd, const json& params = json::object(), std::function<void(CommandResponse)> fn = nullptr);
    void waitFor(const std::vector<std::string>& ids);
    void waitForAllCommands();
    void invokeCommandSync(const std::string& id, const std::string& cmd, const json& params = json::object(), std::function<void(CommandResponse)> fn = nullptr);
};
