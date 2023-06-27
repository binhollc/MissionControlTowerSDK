#include <iostream>
#include "mct_api.h"
#include <thread>
#include <chrono>
#include <future>
#include <map>
#include <condition_variable>

class CommandDispatcher {
private:
    CommandManager& cm;
    std::map<std::string, std::promise<void>> promises;
    std::condition_variable cv;
    std::mutex cv_m;
    int unresolvedPromises = 0;

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
                    {
                        std::lock_guard<std::mutex> lk(cv_m);
                        unresolvedPromises--;
                    }
                    cv.notify_all();
                }
            }
        });
    }

    void invokeCommand(const std::string& transaction_id, const std::string& cmd, const json& prms, std::function<void()> callback = []() {}) {
        promises[transaction_id] = std::promise<void>();
        {
            std::lock_guard<std::mutex> lk(cv_m);
            unresolvedPromises++;
        }
        cm.invoke_command(CommandRequest(transaction_id, cmd, prms));

        std::thread([this, transaction_id, callback](){
            promises[transaction_id].get_future().get();
            callback();
        }).detach();
    }

    void waitForAllCommands() {
        std::unique_lock<std::mutex> lk(cv_m);
        cv.wait(lk, [this](){ return unresolvedPromises == 0; });
    }
};

int main() {
    CommandManager cm;
    CommandDispatcher dispatcher(cm);

    cm.start();

    dispatcher.invokeCommand("1", "open", {{"address", "SIM"}}, [] {
        std::cout << "Command with transaction_id 1 has finished executing.\n";
    });

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
