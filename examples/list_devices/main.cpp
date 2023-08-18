#include "CommandDispatcher.h"
#include <iostream>

void process_response(const CommandResponse& cr) {
    if (cr.status == "success") {
        std::cout << "Devices Found:\n";
        for (auto& device : cr.data) {
            std::cout << "- Product: " << device["product"].template get<std::string>() << "\n";
            std::cout << "  Type: " << device["type"].template get<std::string>() << "\n";
            std::cout << "  Port: " << device["port"].template get<std::string>() << "\n";
            std::cout << "  Manufacturer: " << device["manufacturer"].template get<std::string>() << "\n";
        }
    } else {
        std::cout << "Error occurred while listing devices.\n";
    }
}

int main() {
    CommandDispatcher dispatcher("ListUsbDevices");

    dispatcher.start();

    dispatcher.invokeCommandSync("0", "list", {
        {"serialFilter", {{1240, 60724}}},
        {"hidFilter", {{1240, 221}, {8137, 33532}}}
    }, [](CommandResponse cr) {
        process_response(cr);
    });

    dispatcher.waitForAllCommands();

    dispatcher.invokeCommand("0", "exit", {});

    dispatcher.stop();

    return 0;
}
