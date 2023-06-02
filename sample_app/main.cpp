#include <iostream>
#include "mct_api.h"
#include <thread>
#include <chrono>

int main() {
    CommandManager cm;

    cm.start();

    CommandRequest cr("1", "open", {{"address", "123"}});
    cm.invoke_command(cr);

    cm.on_command_response([](CommandResponse cr){
        // Print the response to stdout
        std::cout << "Received response: " << cr.transaction_id << " " << cr.status << " " << cr.is_promise << " " << cr.data << "\n";
    });

    std::this_thread::sleep_for(std::chrono::seconds(30));

    cm.stop();

    return 0;
}
