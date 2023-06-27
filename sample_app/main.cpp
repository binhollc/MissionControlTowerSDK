#include <iostream>
#include "mct_api.h"
#include <thread>
#include <chrono>
#include <future>
#include <map>

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

    // For each command request, create a promise and store it in the map
    promises["1"] = std::promise<void>();
    cm.invoke_command(CommandRequest("1", "open", {{"address", "SIM"}}));

    promises["2"] = std::promise<void>();
    cm.invoke_command(CommandRequest("2", "i2c_scan", {
        {"config", {
            {"internalPullUpResistors", "false"},
            {"clockFrequency", "400000"},
            {"addressFormat", "7"}
        }}
    }));

    // Wait for all futures before sending the exit command
    for (auto& promise : promises) {
        promise.second.get_future().wait();
    }

    cm.invoke_command(CommandRequest("0", "exit"));

    cm.stop();

    return 0;
}
