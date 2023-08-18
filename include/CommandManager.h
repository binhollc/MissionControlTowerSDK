#pragma once
#include <string>
#include <map>
#include <functional>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <nlohmann/json.hpp>

#include "CommandRequest.h"
#include "CommandResponse.h"

#ifdef _WIN32
    #include <windows.h>
    #include "BridgeReader_windows.h"
#else
    #include "bridge_reader.h"
#endif

using json = nlohmann::json;

// CommandManager class definition
class CommandManager {
public:
    CommandManager(const std::string &targetCommandAdaptorName) : targetCommandAdaptor(targetCommandAdaptorName), isRunningWriteThread(false), isRunningReadThread(false), isRunningCallbackThread(false) {}
    void start();
    void stop();
    void invoke_command(const CommandRequest& request);
    void on_command_response(std::function<void(CommandResponse)> fn);
private:
    std::string targetCommandAdaptor;
    #ifdef _WIN32
        HANDLE bridgeProcessWrite, bridgeProcessRead;
    #else
        FILE* bridgeProcess;
    #endif
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
