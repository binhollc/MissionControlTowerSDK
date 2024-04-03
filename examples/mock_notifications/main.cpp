#include "CommandDispatcher.h"
#include <iostream>
#include <chrono>
#include <thread>

void printCommandResponse(const CommandResponse& cr, const std::string& action) {
    std::cout << "Action: " << action << "\n";
    std::cout << "Transaction ID: " << cr.transaction_id << "\n";
    std::cout << "Status: " << cr.status << "\n";
    std::cout << "Is Promise: " << (cr.is_promise ? "True" : "False") << "\n";
    std::cout << "Data: " << cr.data.dump() << "\n";
    std::cout << "----------------------------------\n";
}

int main() {
    CommandDispatcher dispatcher("Mock");

    dispatcher.start();

    dispatcher.onNotification([&](const CommandResponse& cr) {
        if (cr.transaction_id == "0") {
            printCommandResponse(cr, "NOTIFICATION");
        }
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10000));

    dispatcher.invokeCommandSync("1000", "exit", {});
    
    dispatcher.waitForAllCommands();

    dispatcher.stop();

  return 0;
}
