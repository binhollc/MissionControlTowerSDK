#include "CommandDispatcher.h"
#include <iostream>

void CommandDispatcher::start() {
    commandManager.start();
    commandManager.on_command_response([this](CommandResponse cr) {
        std::lock_guard<std::mutex> lock(mtx);

        // If there is a notification callback, call it
        if (notificationCallback) {
            notificationCallback(cr);
        }

        if (!cr.is_promise) {
            if (callbacks.count(cr.transaction_id) > 0) {
                callbacks[cr.transaction_id](cr);
                callbacks.erase(cr.transaction_id);
            }
            cv.notify_all();
        } else {
            if (callbacks.count(cr.transaction_id) > 0) {
                callbacks[cr.transaction_id](cr);
            }
        }
    });
}

void CommandDispatcher::stop() {
    commandManager.stop();
}

void CommandDispatcher::invokeCommand(const std::string& id, const std::string& cmd, const json& params, std::function<void(CommandResponse)> fn) {
    std::lock_guard<std::mutex> lock(mtx);
    if (fn != nullptr) {
        callbacks[id] = fn;
    }
    commandManager.invoke_command(CommandRequest(id, cmd, params));
}

void CommandDispatcher::waitFor(const std::vector<std::string>& ids) {
    std::unique_lock<std::mutex> lock(mtx);

    if (std::all_of(ids.begin(), ids.end(), [this](const std::string& id){ return callbacks.count(id) == 0; })) {
        return; // All specified callbacks are already inactive, return immediately.
    }

    cv.wait(lock, [this, &ids]() {
        for (const auto& id : ids) {
            if (callbacks.count(id) > 0) {
                return false;
            }
        }
        return true;
    });
}

void CommandDispatcher::waitForAllCommands() {
    std::unique_lock<std::mutex> lock(mtx);

    if (callbacks.empty()) {
        return; // No active callbacks, return immediately.
    }

    cv.wait(lock, [this]() { return callbacks.empty(); });
}

void CommandDispatcher::invokeCommandSync(const std::string& id, const std::string& cmd, const json& params, std::function<void(CommandResponse)> fn) {
    invokeCommand(id, cmd, params, fn);
    waitFor({id});
}

void CommandDispatcher::onNotification(std::function<void(CommandResponse)> fn) {
    std::lock_guard<std::mutex> lock(mtx); // Protect the callback setup with mutex
    notificationCallback = fn;
}