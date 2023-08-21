#include "CommandDispatcher.h"
#include <iostream>

void process_response(const CommandResponse& cr) {
    if (cr.status == "success") {
        if (cr.data.find("data") != cr.data.end()) {
          if (cr.data["data"].is_array()) {  // Check if the data is an array
              for (const auto& byte : cr.data["data"]) {
                  std::cout << byte.template get<std::string>() << " ";
              }
          }
        }
    } else {
        std::cout << "Error occurred.\n";
    }
}

int main() {
    CommandDispatcher dispatcher("BinhoSupernova");

    dispatcher.start();

    // Open device
    dispatcher.invokeCommandSync("0", "open", {{"address", "DevSrvsID:4295040111"}}, process_response);

    // Set bus voltage
    dispatcher.invokeCommandSync("0", "i2c_spi_uart_set_bus_voltage", {{"busVoltageInV", "3.3"}}, process_response);

    // Set I2C parameters
    dispatcher.invokeCommandSync("0", "i2c_set_parameters", {{"clockFrequencyInKHz", "400"}}, process_response);

    // Write to I2C address
    dispatcher.invokeCommandSync("0", "i2c_write", {{"address", "50"}, {"writeBuffer", "AABB"}}, process_response);

    // Read from I2C address
    dispatcher.invokeCommandSync("0", "i2c_read", {{"address", "50"}, {"bytesToRead", "17"}}, process_response);

    // Write to I2C address using subaddress
    dispatcher.invokeCommandSync("0", "i2c_write_using_subaddress", {
        {"address", "50"},
        {"subaddress", "0000"},
        {"writeBuffer", "010203040506"}
    }, process_response);

    // Read from I2C address using subaddress
    dispatcher.invokeCommandSync("0", "i2c_read_using_subaddress", {
        {"address", "50"},
        {"subaddress", "0000"},
        {"bytesToRead", "10"}
    }, process_response);

    // Exit
    dispatcher.invokeCommand("0", "exit", {});

    dispatcher.stop();

    return 0;
}
