#include "mct_api.h"
#include <thread>
#include <chrono>

CommandRequest lastRequest;
std::function<void(CommandResponse)> callback_fn;

void CommandManager::invoke_command(const CommandRequest& request) {
    lastRequest = request;
}

void CommandManager::on_command_response(std::function<void(CommandResponse)> fn) {
    callback_fn = fn;

    // Start a thread that calls the callback every N seconds
    std::thread([this]() {
        while(true) {
            // Wait N seconds
            std::this_thread::sleep_for(std::chrono::seconds(5));

            // Generate a mock CommandResponse and call the callback
            CommandResponse cr(lastRequest.transaction_id, "ok", "false", "mock data");
            callback_fn(cr);
        }
    }).detach();
}
