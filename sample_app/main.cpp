#include <iostream>
#include "mct_api.h"
#include <thread>
#include <chrono>
#include <future>
#include <map>

void invokeCommandWithPromise(CommandManager& cm, std::map<std::string, std::promise<void>>& promises, const std::string& transaction_id, const std::string& cmd, const std::map<std::string, std::map<std::string, std::string>>& args) {
    promises[transaction_id] = std::promise<void>();
    cm.invoke_command(CommandRequest(transaction_id, cmd, args));
}

int main() {
    CommandManager cm;

    // Create a map to hold the promises for each command request
    std::map<std::string, std::promise<void>> promises;

    auto responseFunction = [&promises](CommandResponse cr) {
        // Print the response to stdout
        std::cout << "Received response: " << cr.transaction_id << " " << cr.status << " " << cr.is_promise << " " << cr.data << "\n";

        // If the response is not a promise, set the value of the corresponding promise
        if (!cr.is_promise) {
            auto it = promises.find(cr.transaction_id);
            if (it != promises.end()) {
                it->second.set_value();
            }
        }
    };

    cm.on_command_response(responseFunction);

    cm.start();

    // Invoke commands with promises
    invokeCommandWithPromise(cm, promises, "1", "open", {{"address", "SIM"}});
    invokeCommandWithPromise(cm, promises, "2", "i2c_scan", {
        {"config", {
            {"internalPullUpResistors", "false"},
            {"clockFrequency", "400000"},
            {"addressFormat", "7"}
        }}
    });

    // Wait for all futures before sending the exit command
    for (auto& promise : promises) {
        promise.second.get_future().wait();
    }

    cm.invoke_command(CommandRequest("0", "exit"));

    cm.stop();

    return 0;
}
