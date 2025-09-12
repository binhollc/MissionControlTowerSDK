// Example: Nova Breathing LEDs with BinhoNova
//
// This example demonstrates how to use the MissionControlTowerSDK to control the PWM output of a BinhoNova host adapter
// to create a "breathing" LED effect. It shows how to:
//   - Open a connection to a simulated Nova port
//   - Use the GPIO PWM command to vary the duty cycle in a forward and backward loop
//   - Wait for all commands to complete and cleanly exit
//
// The example uses the CommandDispatcher and prints responses for each command.

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
    CommandDispatcher dispatcher("BinhoNova");

    dispatcher.start();

    dispatcher.invokeCommandSync("1", "open", {{"address", "NovaSimulatedPort"}}, [](CommandResponse cr) {
        printCommandResponse(cr, "Opening NovaSimulatedPort");
    });

    for (int j = 1; j <= 7; ++j) {
        // Forward loop
        for (int i = 1; i <= 10; ++i) {
            int dc = i * 10;  // Duty cycle goes from 10 to 100 in increments of 10
            dispatcher.invokeCommandSync(std::to_string(i), "gpio", {
                {"channel", 0},
                {"type", "pwm"},
                {"params", {
                    {"dc", std::to_string(dc)}
                }}
            }, [i](CommandResponse cr) {
                printCommandResponse(cr, std::string("Forward GPIO PWM Cycle - Step ") + std::to_string(i));
            });
        }

        // Backward loop
        for (int i = 10; i >= 1; --i) {
            int dc = i * 10;  // Duty cycle goes from 100 to 10 in increments of -10
            dispatcher.invokeCommandSync(std::to_string(i), "gpio", {
                {"channel", 0},
                {"type", "pwm"},
                {"params", {
                    {"dc", std::to_string(dc)}
                }}
            }, [i](CommandResponse cr) {
                printCommandResponse(cr, std::string("Backward GPIO PWM Cycle - Step ") + std::to_string(i));
            });
        }
    }

    dispatcher.waitForAllCommands();
    std::cout << "All commands executed. Waiting for all responses...\n";

    dispatcher.invokeCommandSync("0", "exit", {});
    std::cout << "Exiting Command Dispatcher...\n";

    dispatcher.stop();
    std::cout << "Command Dispatcher stopped successfully.\n";

    return 0;
}
