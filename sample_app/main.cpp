#include <iostream>
#include "mct_api.h"
#include <thread>
#include <chrono>

int main() {
    CommandManager cm;

    cm.start();

    CommandRequest cr("1", "test_command", {{"param1", "value1"}, {"param2", "value2"}});
    cm.invoke_command(cr);

    cm.on_command_response([](CommandResponse cr){
        // Print the response to stdout
        std::cout << "Received response: " << cr.transaction_id << " " << cr.status << " " << cr.is_promise << " " << cr.data << "\n";
    });

    cm.stop();

    // // Let the program run indefinitely.
    // // In a real application, you'd have some way to quit this loop.
    // while(true) {
    //     std::this_thread::sleep_for(std::chrono::seconds(1));
    // }

    return 0;
}
