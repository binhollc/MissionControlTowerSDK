#include "CommandDispatcher.h"
#include <iostream>

void printCommandResponse(const CommandResponse& cr, const std::string& action) {
    std::cout << "Action: " << action << "\n";
    std::cout << "Transaction ID: " << cr.transaction_id << "\n";
    std::cout << "Status: " << cr.status << "\n";
    std::cout << "Is Promise: " << (cr.is_promise ? "True" : "False") << "\n";
    std::cout << "Data: " << cr.data.dump() << "\n";
    std::cout << "----------------------------------\n";
}

int main() {
    CommandDispatcher dispatcher("BinhoSupernova");

    dispatcher.start();
    std::cout << "Starting Command Dispatcher for BinhoSupernova...\n";

    // Open the SupernovaSimulatedPort
    dispatcher.invokeCommandSync("0", "open", {}, [](CommandResponse cr) {
        printCommandResponse(cr, "Opening SupernovaSimulatedPort");
    });

    // Fetch various USB strings
    const char* subCommands[] = {"MANUFACTURER", "PRODUCT_NAME", "SERIAL_NUMBER", "HW_VERSION", "FW_VERSION"};
    for (const char* subCommand : subCommands) {
        dispatcher.invokeCommandSync("0", "get_usb_string", {{"subCommand", subCommand}}, [subCommand](CommandResponse cr) {
            printCommandResponse(cr, std::string("Fetching USB String (") + subCommand + ")");
        });
    }

    dispatcher.waitForAllCommands();
    std::cout << "All commands executed. Waiting for all responses...\n";

    // Exit the command dispatcher
    dispatcher.invokeCommandSync("0", "exit", {});
    std::cout << "Exiting Command Dispatcher...\n";

    dispatcher.stop();
    std::cout << "Command Dispatcher stopped successfully.\n";

    return 0;
}
