// Example: Supernova 101 - Basic Device Information with BinhoSupernova
//
// This example demonstrates how to use the MissionControlTowerSDK to connect to a BinhoSupernova host adapter
// and retrieve basic device information. It shows how to:
//   - Open a connection to the device
//   - Retrieve device info via USB strings (manufacturer, product name, serial number, etc.)
//   - Print responses for each command
//   - Cleanly close and exit the session
//
// The example uses the CommandDispatcher and is a starting point for more advanced Supernova usage.

#include "CommandDispatcher.h"
#include <iostream>

void printCommandResponse(const CommandResponse& cr, const std::string& action) {
    // Filter out bridge log messages (negative transaction_id)
    if (!cr.transaction_id.empty() && cr.transaction_id[0] == '-') {
        return;
    }
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
    dispatcher.invokeCommandSync("1", "open", {}, [](CommandResponse cr) {
        printCommandResponse(cr, "Opening SupernovaSimulatedPort");
    });

    // Fetch various USB strings
    const char* subCommands[] = {"MANUFACTURER", "PRODUCT_NAME", "SERIAL_NUMBER", "HW_VERSION", "FW_VERSION"};
    for (const char* subCommand : subCommands) {
        dispatcher.invokeCommandSync("1", "get_usb_string", {{"subCommand", subCommand}}, [subCommand](CommandResponse cr) {
            printCommandResponse(cr, std::string("Fetching USB String (") + subCommand + ")");
        });
    }

    dispatcher.invokeCommandSync("1", "close", {});

    dispatcher.waitForAllCommands();
    std::cout << "All commands executed. Waiting for all responses...\n";

    // Exit the command dispatcher
    dispatcher.invokeCommandSync("1", "exit", {});
    std::cout << "Exiting Command Dispatcher...\n";

    dispatcher.stop();
    std::cout << "Command Dispatcher stopped successfully.\n";

    return 0;
}
