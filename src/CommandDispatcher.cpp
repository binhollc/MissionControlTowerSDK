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
            if (activeCommands.count(cr.transaction_id) > 0) {
                --activeCommands[cr.transaction_id];
                if (activeCommands[cr.transaction_id] == 0) {
                    activeCommands.erase(cr.transaction_id);
                }
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
    ++activeCommands[id];
    if (fn != nullptr) {
        callbacks[id] = fn;
    }
    commandManager.invoke_command(CommandRequest(id, cmd, params));
}

void CommandDispatcher::waitFor(const std::vector<std::string>& ids) {
    std::unique_lock<std::mutex> lock(mtx);

    if (std::all_of(ids.begin(), ids.end(), [this](const std::string& id){ return activeCommands.count(id) == 0; })) {
        return; // All specified commands are already inactive, return immediately.
    }

    cv.wait(lock, [this, &ids]() {
        for (const auto& id : ids) {
            if (activeCommands.count(id) > 0) {
                return false;
            }
        }
        return true;
    });
}

void CommandDispatcher::waitForAllCommands() {
    std::unique_lock<std::mutex> lock(mtx);

    if (activeCommands.empty()) {
        return; // No active commands, return immediately.
    }

    cv.wait(lock, [this]() { return activeCommands.empty(); });
}

void CommandDispatcher::invokeCommandSync(const std::string& id, const std::string& cmd, const json& params, std::function<void(CommandResponse)> fn) {
    invokeCommand(id, cmd, params, fn);
    waitFor({id});
}

void CommandDispatcher::onNotification(std::function<void(CommandResponse)> fn) {
    std::lock_guard<std::mutex> lock(mtx); // Protect the callback setup with mutex
    notificationCallback = fn;
}