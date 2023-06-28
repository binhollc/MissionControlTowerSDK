#include <iostream>
#include "mct_api.h"
#include <thread>
#include <mutex>
#include <map>
#include <condition_variable>
#include <chrono>

class CommandDispatcher {
private:
    CommandManager commandManager;
    std::mutex mtx;
    std::condition_variable cv;
    std::unordered_map<std::string, int> activeCommands;
    std::unordered_map<std::string, std::function<void(CommandResponse)>> callbacks;

public:
    void start() {
        commandManager.start();
        commandManager.on_command_response([this](CommandResponse cr) {
            std::lock_guard<std::mutex> lock(mtx);
            if (!cr.is_promise) {
                if (callbacks.count(cr.transaction_id) > 0) {
                    callbacks[cr.transaction_id](cr);
                    callbacks.erase(cr.transaction_id);
                }
                --activeCommands[cr.transaction_id];
                if (activeCommands[cr.transaction_id] == 0) {
                    activeCommands.erase(cr.transaction_id);
                }
                cv.notify_all();
            } else {
                if (callbacks.count(cr.transaction_id) > 0) {
                    callbacks[cr.transaction_id](cr);
                }
            }
        });
    }

    void stop() {
        commandManager.stop();
    }

    void invokeCommand(const std::string& id, const std::string& cmd, const json& params = json::object(), std::function<void(CommandResponse)> fn = nullptr) {
        std::lock_guard<std::mutex> lock(mtx);
        ++activeCommands[id];
        if (fn != nullptr) {
            callbacks[id] = fn;
        }
        commandManager.invoke_command(CommandRequest(id, cmd, params));
    }

    void waitFor(const std::vector<std::string>& ids) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this, &ids]() {
            for (const auto& id : ids) {
                if (activeCommands.count(id) > 0) {
                    return false;
                }
            }
            return true;
        });
    }

    void waitForAllCommands() {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [this]() { return activeCommands.empty(); });
    }

    void invokeCommandSync(const std::string& id, const std::string& cmd, const json& params = json::object(), std::function<void(CommandResponse)> fn = nullptr) {
        invokeCommand(id, cmd, params, fn);
        waitFor({id});
    }
};

int main() {
    CommandDispatcher dispatcher;

    dispatcher.start();

    dispatcher.invokeCommandSync("1", "open", {{"address", "/dev/cu.usbmodem14201"}}, [](CommandResponse cr) {
        std::cout << cr.transaction_id << cr.status << cr.is_promise << cr.data.dump() << "\n";
    });

    for (int i = 1; i <= 10; ++i) {
        int dc = i * 10;  // Duty cycle goes from 10 to 100 in increments of 10
        dispatcher.invokeCommandSync(std::to_string(i), "gpio", {
            {"channel", 0},
            {"type", "pwm"},
            {"params", {
                {"dc", std::to_string(dc)}
            }}
        }, [](CommandResponse cr) {
            std::cout << cr.transaction_id << cr.status << cr.is_promise << cr.data.dump() << "\n";
        });
    //    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    dispatcher.waitForAllCommands();

    dispatcher.invokeCommand("0", "exit", {});

    dispatcher.stop();

    return 0;
}
