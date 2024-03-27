#include "CommandDispatcher.h"
#include <iostream>
#include <chrono>
#include <thread>

void printCommandResponse(const CommandResponse &cr, const std::string &action)
{
    if (!cr.is_promise){
        std::cout << "Action: " << action << "\n";
        std::cout << "Transaction ID: " << cr.transaction_id << "\n";
        std::cout << "Status: " << cr.status << "\n";
        std::cout << "Is Promise: " << (cr.is_promise ? "True" : "False") << "\n";
        std::cout << "Data: " << cr.data.dump() << "\n";
        std::cout << "----------------------------------\n";
    }
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

  // Set bus voltage
  dispatcher.invokeCommandSync(idGenerator.nextID(), "i2c_spi_uart_set_bus_voltage", {{"busVoltageInV", "3.3"}}, handleCommandResponse("Bus Voltage Set"));

  // Initialize SPI
  dispatcher.invokeCommandSync(idGenerator.nextID(), "spi_init", {
    {"bitOrder", "MSB"}, {"mode", "0"}, {"bitsPerTransfer", "8"}, {"chipSelect", "0"}, {"chipSelectPol", "0"}, {"clockFrequencyInKHz", "1000"}
    }, handleCommandResponse("Initialize SPI"));

  // Configure SPI (It is not neccesary, this is for demonstration purposes)
  dispatcher.invokeCommandSync(idGenerator.nextID(), "spi_config", {
    {"bitOrder", "MSB"}, {"mode", "0"}, {"bitsPerTransfer", "8"}, {"chipSelect", "0"}, {"chipSelectPol", "0"}, {"clockFrequencyInKHz", "2000"}
    }, handleCommandResponse("Config SPI"));

  // Transfer: Read Who Am I register
  dispatcher.invokeCommandSync(idGenerator.nextID(), "spi_transfer", {
    {"writeBuffer", "9F"}, {"bytesToRead", "5"}
    }, handleCommandResponse("Read Who Am I"));

  // Transfer: Enable writing
  dispatcher.invokeCommandSync(idGenerator.nextID(), "spi_transfer", {
    {"writeBuffer", "06"}, {"bytesToRead", "1"}
    }, handleCommandResponse("Enable Write"));
  // Transfer: Write to subaddress "0x0000" data "0xABCD"
  dispatcher.invokeCommandSync(idGenerator.nextID(), "spi_transfer", {
    {"writeBuffer", "020000ABCD"}, {"bytesToRead", "5"}
    }, handleCommandResponse("Writing to 0x0000"));
  // Transfer: Read at subaddress "0x0000"
  dispatcher.invokeCommandSync(idGenerator.nextID(), "spi_transfer", {
    {"writeBuffer", "030000"}, {"bytesToRead", "5"}
    }, handleCommandResponse("Reading at 0x0000"));
  
  dispatcher.invokeCommandSync(idGenerator.nextID(), "exit", {});

  dispatcher.stop();

  return 0;
}
