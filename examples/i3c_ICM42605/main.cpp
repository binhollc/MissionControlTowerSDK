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
void invokeI3CReadUsingSubaddress(CommandDispatcher& dispatcher, const std::string& transactionId,
                                   const std::string& subaddress, const std::string& bytesToRead,
                                   const std::string& mode = "SDR", const std::string& pushPullClockFrequencyInMHz = "5",
                                   const std::string& openDrainClockFrequencyInKHz = "1250", const std::string& address = "08") {
    dispatcher.invokeCommandSync(transactionId, "i3c_read_using_subaddress", {{"address", address}, {"subaddress", subaddress}, {"bytesToRead", bytesToRead}, {"mode", mode}, {"pushPullClockFrequencyInMHz", pushPullClockFrequencyInMHz}, {"openDrainClockFrequencyInKHz", openDrainClockFrequencyInKHz}},
    [](CommandResponse cr) {
            printCommandResponse(cr, std::string("READ"));
        });
}
// Utility function to simplify command invocation
void invokeI3CWriteUsingSubaddress(CommandDispatcher& dispatcher, const std::string& transactionId,
                                   const std::string& subaddress, const std::string& writeBuffer,
                                   const std::string& mode = "SDR", const std::string& pushPullClockFrequencyInMHz = "5",
                                   const std::string& openDrainClockFrequencyInKHz = "1250", const std::string& address = "08") {
    dispatcher.invokeCommandSync(transactionId, "i3c_write_using_subaddress", {{"address", address}, {"subaddress", subaddress}, {"writeBuffer", writeBuffer}, {"mode", mode}, {"pushPullClockFrequencyInMHz", pushPullClockFrequencyInMHz}, {"openDrainClockFrequencyInKHz", openDrainClockFrequencyInKHz}},
    [](CommandResponse cr) {
            printCommandResponse(cr, std::string("WRITE"));
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
  invokeI3CWriteUsingSubaddress(dispatcher, idGenerator.nextID(), "4E", "00");

  dispatcher.invokeCommandSync(idGenerator.nextID(), "i3c_read_using_subaddress", {{"address", "08"}, {"subaddress", "75"}, {"bytesToRead", "1"}, {"mode", "SDR"}, {"pushPullClockFrequencyInMHz", "5"}, {"openDrainClockFrequencyInKHz", "1250"}}, handleCommandResponse("Read 'Who Am I'"));
  invokeI3CReadUsingSubaddress(dispatcher, idGenerator.nextID(), "75", "01");

  invokeI3CReadUsingSubaddress(dispatcher, idGenerator.nextID(), "4E", "01");
  
  dispatcher.invokeCommandSync(idGenerator.nextID(), "exit", {});

  dispatcher.waitForAllCommands();

  dispatcher.stop();

  return 0;
}
