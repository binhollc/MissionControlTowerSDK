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
  std::cout << "----------------------------------\n";
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
void invokeI3CWriteUsingSubaddress(CommandDispatcher& dispatcher, const std::string& transactionId,
                                   const std::string& subaddress, const std::string& writeBuffer,
                                   const std::string& mode = "SDR", const std::string& pushPullClockFrequencyInMHz = "5",
                                   const std::string& openDrainClockFrequencyInKHz = "1250", const std::string& address = "08") {
    dispatcher.invokeCommandSync(transactionId, "i3c_write_using_subaddress", {{"address", address}, {"subaddress", subaddress}, {"writeBuffer", writeBuffer}, {"mode", mode}, {"pushPullClockFrequencyInMHz", pushPullClockFrequencyInMHz}, {"openDrainClockFrequencyInKHz", openDrainClockFrequencyInKHz}});
}


int main()
{
  CommandDispatcher dispatcher("BinhoSupernova");
  CommandIDGenerator idGenerator;

  dispatcher.start();

  dispatcher.onNotification([&](const CommandResponse &cr) {
      if (cr.transaction_id == "0") {
          printCommandResponse(cr, "IBI RECEIVED");
      }
  });

  // Open device
  dispatcher.invokeCommandSync(idGenerator.nextID(), "open", {}, [](CommandResponse cr) {
      printCommandResponse(cr, "Supernova Opened");
  });

  // Set bus voltage
  dispatcher.invokeCommandSync(idGenerator.nextID(), "i3c_init_bus", {{"busVoltageInV", "3.3"}}, [](CommandResponse cr) {
      printCommandResponse(cr, "Bus Voltage Set");
  });

  // Command to enable IBIs on the target device
  dispatcher.invokeCommandSync(idGenerator.nextID(), "i3c_ibi_enable", {{"address", "08"}});

  // Refactored commands to write using subaddress with auto-generated IDs
  invokeI3CWriteUsingSubaddress(dispatcher, idGenerator.nextID(), "76", "00");
  invokeI3CWriteUsingSubaddress(dispatcher, idGenerator.nextID(), "4E", "20");
  invokeI3CWriteUsingSubaddress(dispatcher, idGenerator.nextID(), "13", "05");
  invokeI3CWriteUsingSubaddress(dispatcher, idGenerator.nextID(), "16", "40");
  invokeI3CWriteUsingSubaddress(dispatcher, idGenerator.nextID(), "5F", "61");
  invokeI3CWriteUsingSubaddress(dispatcher, idGenerator.nextID(), "60", "0F00");
  invokeI3CWriteUsingSubaddress(dispatcher, idGenerator.nextID(), "50", "0E");
  invokeI3CWriteUsingSubaddress(dispatcher, idGenerator.nextID(), "76", "01");
  invokeI3CWriteUsingSubaddress(dispatcher, idGenerator.nextID(), "03", "38");
  invokeI3CWriteUsingSubaddress(dispatcher, idGenerator.nextID(), "7A", "02");
  invokeI3CWriteUsingSubaddress(dispatcher, idGenerator.nextID(), "7C", "1F");
  invokeI3CWriteUsingSubaddress(dispatcher, idGenerator.nextID(), "76", "04");
  invokeI3CWriteUsingSubaddress(dispatcher, idGenerator.nextID(), "4F", "04");
  invokeI3CWriteUsingSubaddress(dispatcher, idGenerator.nextID(), "76", "00");
  invokeI3CWriteUsingSubaddress(dispatcher, idGenerator.nextID(), "4E", "02");

  std::this_thread::sleep_for(std::chrono::milliseconds(10000));

  dispatcher.invokeCommandSync(idGenerator.nextID(), "exit", {});

  dispatcher.waitForAllCommands();

  dispatcher.stop();

  return 0;
}
