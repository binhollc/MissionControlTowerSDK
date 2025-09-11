// Example: I3C CCCs (Common Command Codes) usage with BinhoSupernova
//
// This example demonstrates how to use the MissionControlTowerSDK to send I3C Common Command Codes (CCCs)
// to a target device using a BinhoSupernova host adapter. It shows how to:
//   - Initialize the controller and bus
//   - Set bus voltage
//   - Query and set target device properties (GETPID, GETMRL, GETMWL, SETMRL, SETMWL)
//   - Use both direct and broadcast CCCs
//   - Wait for all commands to complete and cleanly exit
//
// The example uses the CommandDispatcher and prints responses for each command.

#include "CommandDispatcher.h"
#include <iostream>
#include <chrono>
#include <thread>

void printCommandResponse(const CommandResponse &cr, const std::string &action)
{
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

auto handleCommandResponse(const std::string &action)
{
    return [action](const CommandResponse &cr) {
      printCommandResponse(cr, action);
    };
}

class CommandIDGenerator {
public:
    CommandIDGenerator() : currentID(0) {}

    std::string nextID() {
        return std::to_string(++currentID);
    }

private:
    unsigned long long currentID;
};

int main()
{
  CommandDispatcher dispatcher("BinhoSupernova");
  CommandIDGenerator idGenerator;

  dispatcher.start();

  // Open device
  dispatcher.invokeCommandSync(idGenerator.nextID(), "open", {}, handleCommandResponse("Supernova Opened"));

  // Init controller
  dispatcher.invokeCommandSync(idGenerator.nextID(), "i3c_controller_init", {{"pushPullClockFrequencyInMHz", 5}, {"pushPullDutyCycle", 50}, {"openDrainClockFrequencyInKHz", 400}, {"i2cOpenDrainClockFrequencyInkHz", 400}}, handleCommandResponse("I3C Controller Initialized"));

  // Set bus voltage
  dispatcher.invokeCommandSync(idGenerator.nextID(), "i3c_set_bus_voltage", {{"busVoltageInV", 3.3}}, handleCommandResponse("Bus Voltage Set"));

  // Init bus
  dispatcher.invokeCommandSync(idGenerator.nextID(), "i3c_controller_init_bus", {}, handleCommandResponse("I3C Bus Initialized"));

  // Gets target Provisioned ID
  dispatcher.invokeCommandSync(idGenerator.nextID(), "i3c_ccc_getpid", {{"address", 0x08}}, handleCommandResponse("GETPID"));
  
  // Gets target Max read length
  dispatcher.invokeCommandSync(idGenerator.nextID(), "i3c_ccc_getmrl", {{"address", 0x08}}, handleCommandResponse("GETMRL"));
  
  // Gets target Max write length
  dispatcher.invokeCommandSync(idGenerator.nextID(), "i3c_ccc_getmwl", {{"address", 0x08}}, handleCommandResponse("GETMWL"));
  
  // Sets target Max read length to 3
  dispatcher.invokeCommandSync(idGenerator.nextID(), "i3c_ccc_direct_setmrl", {{"address", 0x08}, {"cccDataBuffer", 3}}, handleCommandResponse("DIRECT SETMRL => 3"));
  
  // We check if it effectively changed
  dispatcher.invokeCommandSync(idGenerator.nextID(), "i3c_ccc_getmrl", {{"address", 0x08}}, handleCommandResponse("GETMRL"));
  
  // Sets target Max write length to 2
dispatcher.invokeCommandSync(idGenerator.nextID(), "i3c_ccc_broadcast_setmwl", {{"cccDataBuffer", 2}}, handleCommandResponse("BROADCAST SETMWL => 2"));

  // We check if it effectively changed
  dispatcher.invokeCommandSync(idGenerator.nextID(), "i3c_ccc_getmwl", {{"address", 0x08}}, handleCommandResponse("GETMWL"));
  
  dispatcher.waitForAllCommands();

  dispatcher.invokeCommandSync(idGenerator.nextID(), "exit", {});

  dispatcher.stop();

  return 0;
}
