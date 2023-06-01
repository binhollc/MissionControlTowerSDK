#include "mct_api.h"

// Note: the details of how these functions work would depend heavily on the specifics
// of your program and how the child process is set up and communicates with the parent process.

void CommandManager::invoke_command(const CommandRequest& request) {
    // Convert the CommandRequest into a JSON message
    // Send the message to the child process
    // This will be an async operation, the response will be handled elsewhere
}

void CommandManager::on_command_response(std::function<void(CommandResponse)> fn) {
    // Wait for a message from the child process
    // This might be in a separate thread, or using non-blocking I/O
    // Convert the message into a CommandResponse
    // Call the provided callback function with the CommandResponse
}
