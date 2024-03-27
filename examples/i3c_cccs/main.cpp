#include "CommandDispatcher.h"
#include <iostream>
#include <chrono>
#include <thread>

void printCommandResponse(const CommandResponse &cr, const std::string &action)
{
    std::cout << "Action: " << action << "\n";
    std::cout << "Transaction ID: " << cr.transaction_id << "\n";
    std::cout << "Status: " << cr.status << "\n";
    std::cout << "Is Promise: " << (cr.is_promise ? "True" : "False") << "\n";
    std::cout << "Data: " << cr.data.dump() << "\n";
    std::cout << "Data Result: " << cr.data["result"].dump() << "\n";
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

// Utility function to simplify command invocation
void invokeI3CSendCCC(CommandDispatcher& dispatcher, 
                                   const std::string& transactionId,
                                   const std::string& ccc, 
                                   const std::string& writeBuffer = "",
                                   const std::string& pushPullClockFrequencyInMHz = "5",
                                   const std::string& openDrainClockFrequencyInKHz = "1250", 
                                   const std::string& address = "08") {
    json params;
    params["cccParams"]["address"] = address;
    params["cccParams"]["writeBuffer"] = writeBuffer;
    params["cccParams"]["ccc"] = ccc;
    params["cccParams"]["pushPullClockFrequencyInMHz"] = pushPullClockFrequencyInMHz;
    params["cccParams"]["openDrainClockFrequencyInKHz"] = openDrainClockFrequencyInKHz;
    dispatcher.invokeCommandSync(transactionId, "i3c_ccc_send", params,
    [](CommandResponse cr) {
            printCommandResponse(cr, std::string("CCC"));
        });
}


int main()
{
  CommandDispatcher dispatcher("BinhoSupernova");
  CommandIDGenerator idGenerator;

  dispatcher.start();

  // Open device
  dispatcher.invokeCommandSync(idGenerator.nextID(), "open", {}, handleCommandResponse("Supernova Opened"));

  // Set bus voltage
  dispatcher.invokeCommandSync(idGenerator.nextID(), "i3c_init_bus", {{"busVoltageInV", "3.3"}}, handleCommandResponse("Bus Voltage Set"));

  // Gets target Provisioned ID
  invokeI3CSendCCC(dispatcher, idGenerator.nextID(), "GETPID");
  // Gets target Max read length
  invokeI3CSendCCC(dispatcher, idGenerator.nextID(), "GETMRL");
  // Gets target Max write length
  invokeI3CSendCCC(dispatcher, idGenerator.nextID(), "GETMWL");

  // Sets target Max read length to 3
  invokeI3CSendCCC(dispatcher, idGenerator.nextID(), "DIRECTSETMRL", "03");
  // We check if it effectively changed
  invokeI3CSendCCC(dispatcher, idGenerator.nextID(), "GETMRL");
  // Sets target Max write length to 2
  invokeI3CSendCCC(dispatcher, idGenerator.nextID(), "BROADCASTSETMWL", "02");
  // We check if it effectively changed
  invokeI3CSendCCC(dispatcher, idGenerator.nextID(), "GETMWL");

  std::this_thread::sleep_for(std::chrono::milliseconds(3000));

  dispatcher.invokeCommandSync(idGenerator.nextID(), "exit", {});

  dispatcher.waitForAllCommands();

  dispatcher.stop();

  return 0;
}
