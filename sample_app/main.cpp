#include <iostream>
#include "mct_api.h"
#include <thread>
#include <chrono>

int main() {
    CommandManager cm;

    cm.on_command_response([](CommandResponse cr){
        // Print the response to stdout
        std::cout << "Received response: " << cr.transaction_id << " " << cr.status << " " << cr.is_promise << " " << cr.data << "\n";
    });

    cm.start();

    cm.invoke_command(CommandRequest("0", "open", {{"address", "SIM"}}));
    cm.invoke_command(CommandRequest("0", "i2c_scan", {
        {"config", {
            {"internalPullUpResistors", "false"},
            {"clockFrequency", "400000"},
            {"addressFormat", "7"}
        }}
    }));

    std::this_thread::sleep_for(std::chrono::seconds(5));

    cm.invoke_command(CommandRequest("0", "exit"));

    cm.stop();

    return 0;
}
