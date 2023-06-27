#include <iostream>
#include "mct_api.h"
#include <thread>
#include <chrono>
#include <future>
#include <map>

class CommandDispatcher {
private:
    CommandManager& cm;
    std::map<std::string, std::promise<void>> promises;

public:
    CommandDispatcher(CommandManager& commandManager)
        : cm(commandManager) {
        cm.on_command_response([this](CommandResponse cr){
            std::cout << "Received response: " << cr.transaction_id << " " << cr.status << " " << cr.is_promise << " " << cr.data << "\n";
            // If the response is not a promise, set the value of the corresponding promise
            if (!cr.is_promise) {
                auto it = promises.find(cr.transaction_id);
                if (it != promises.end()) {
                    it->second.set_value();
                }
            }
        });
    }

    void invokeCommand(const std::string& transaction_id, const std::string& cmd, const json& prms) {
        promises[transaction_id] = std::promise<void>();
        cm.invoke_command(CommandRequest(transaction_id, cmd, prms));
    }

    void waitForAllCommands() {
        for (auto& promise : promises) {
            promise.second.get_future().wait();
        }
    }
};

int main() {
    CommandManager cm;
    CommandDispatcher dispatcher(cm);

    cm.start();

    dispatcher.invokeCommand("1", "open", {{"address", "SIM"}});
    dispatcher.invokeCommand("2", "i2c_scan", {
        {"config", {
            {"internalPullUpResistors", "false"},
            {"clockFrequency", "400000"},
            {"addressFormat", "7"}
        }}
    });

    dispatcher.waitForAllCommands();

    cm.invoke_command(CommandRequest("0", "exit"));

    cm.stop();

    return 0;
}
