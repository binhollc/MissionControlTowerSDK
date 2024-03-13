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

int main()
{
  CommandDispatcher dispatcher("BinhoSupernova");

  dispatcher.start();

  dispatcher.onNotification([&](const CommandResponse &cr) {
    if (cr.transaction_id == "0") {
        printCommandResponse(cr, "IBI RECEIVED");
    }
  });

  // Open device
  dispatcher.invokeCommandSync("1", "open", {}, [](CommandResponse cr) {
      printCommandResponse(cr, "Supernova Opened");
  });

  // Set bus voltage
  dispatcher.invokeCommandSync("2", "i3c_init_bus", {{"busVoltageInV", "3.3"}}, [](CommandResponse cr) {
      printCommandResponse(cr, "Bus Voltage Set");
  });

  // Command to enable IBIs on the target device
  dispatcher.invokeCommandSync("100", "i3c_ibi_enable", {{"address","08"}});

  // Commands to write using subaddress
  dispatcher.invokeCommandSync("3", "i3c_write_using_subaddress", {{"address","08"}, {"subaddress","76"}, {"writeBuffer","00"}, {"mode","SDR"}, {"pushPullClockFrequencyInMHz","5"}, {"openDrainClockFrequencyInKHz","1250"}});
  dispatcher.invokeCommandSync("4", "i3c_write_using_subaddress", {{"address","08"}, {"subaddress","4E"}, {"writeBuffer","20"}, {"mode","SDR"}, {"pushPullClockFrequencyInMHz","5"}, {"openDrainClockFrequencyInKHz","1250"}});
  dispatcher.invokeCommandSync("5", "i3c_write_using_subaddress", {{"address","08"}, {"subaddress","13"}, {"writeBuffer","05"}, {"mode","SDR"}, {"pushPullClockFrequencyInMHz","5"}, {"openDrainClockFrequencyInKHz","1250"}});
  dispatcher.invokeCommandSync("6", "i3c_write_using_subaddress", {{"address","08"}, {"subaddress","16"}, {"writeBuffer","40"}, {"mode","SDR"}, {"pushPullClockFrequencyInMHz","5"}, {"openDrainClockFrequencyInKHz","1250"}});
  dispatcher.invokeCommandSync("7", "i3c_write_using_subaddress", {{"address","08"}, {"subaddress","5F"}, {"writeBuffer","61"}, {"mode","SDR"}, {"pushPullClockFrequencyInMHz","5"}, {"openDrainClockFrequencyInKHz","1250"}});
  dispatcher.invokeCommandSync("8", "i3c_write_using_subaddress", {{"address","08"}, {"subaddress","60"}, {"writeBuffer","0F00"}, {"mode","SDR"}, {"pushPullClockFrequencyInMHz","5"}, {"openDrainClockFrequencyInKHz","1250"}});
  dispatcher.invokeCommandSync("9", "i3c_write_using_subaddress", {{"address","08"}, {"subaddress","50"}, {"writeBuffer","0E"}, {"mode","SDR"}, {"pushPullClockFrequencyInMHz","5"}, {"openDrainClockFrequencyInKHz","1250"}});
  dispatcher.invokeCommandSync("10", "i3c_write_using_subaddress", {{"address","08"}, {"subaddress","76"}, {"writeBuffer","01"}, {"mode","SDR"}, {"pushPullClockFrequencyInMHz","5"}, {"openDrainClockFrequencyInKHz","1250"}});
  dispatcher.invokeCommandSync("11", "i3c_write_using_subaddress", {{"address","08"}, {"subaddress","03"}, {"writeBuffer","38"}, {"mode","SDR"}, {"pushPullClockFrequencyInMHz","5"}, {"openDrainClockFrequencyInKHz","1250"}});
  dispatcher.invokeCommandSync("12", "i3c_write_using_subaddress", {{"address","08"}, {"subaddress","7A"}, {"writeBuffer","02"}, {"mode","SDR"}, {"pushPullClockFrequencyInMHz","5"}, {"openDrainClockFrequencyInKHz","1250"}});
  dispatcher.invokeCommandSync("13", "i3c_write_using_subaddress", {{"address","08"}, {"subaddress","7C"}, {"writeBuffer","1F"}, {"mode","SDR"}, {"pushPullClockFrequencyInMHz","5"}, {"openDrainClockFrequencyInKHz","1250"}});
  dispatcher.invokeCommandSync("14", "i3c_write_using_subaddress", {{"address","08"}, {"subaddress","76"}, {"writeBuffer","04"}, {"mode","SDR"}, {"pushPullClockFrequencyInMHz","5"}, {"openDrainClockFrequencyInKHz","1250"}});
  dispatcher.invokeCommandSync("15", "i3c_write_using_subaddress", {{"address","08"}, {"subaddress","4F"}, {"writeBuffer","04"}, {"mode","SDR"}, {"pushPullClockFrequencyInMHz","5"}, {"openDrainClockFrequencyInKHz","1250"}});
  dispatcher.invokeCommandSync("16", "i3c_write_using_subaddress", {{"address","08"}, {"subaddress","76"}, {"writeBuffer","00"}, {"mode","SDR"}, {"pushPullClockFrequencyInMHz","5"}, {"openDrainClockFrequencyInKHz","1250"}});
  dispatcher.invokeCommandSync("17", "i3c_write_using_subaddress", {{"address","08"}, {"subaddress","4E"}, {"writeBuffer","02"}, {"mode","SDR"}, {"pushPullClockFrequencyInMHz","5"}, {"openDrainClockFrequencyInKHz","1250"}});

  std::this_thread::sleep_for(std::chrono::milliseconds(10000));

  dispatcher.invokeCommandSync("1000", "exit", {});

  dispatcher.waitForAllCommands();

  dispatcher.stop();

  return 0;
}
