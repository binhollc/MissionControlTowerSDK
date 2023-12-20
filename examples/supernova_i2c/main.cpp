#include "CommandDispatcher.h"
#include <iostream>

void printCommandResponse(const CommandResponse& cr, const std::string& action) {
    std::cout << "Action: " << action << "\n";
    std::cout << "Transaction ID: " << cr.transaction_id << "\n";
    std::cout << "Status: " << cr.status << "\n";
    std::cout << "Is Promise: " << (cr.is_promise ? "True" : "False") << "\n";
    std::cout << "Data: " << cr.data.dump() << "\n";
    std::cout << "----------------------------------\n";
}

int main() {
    CommandDispatcher dispatcher("BinhoSupernova");

    dispatcher.start();

    // Open device
    dispatcher.invokeCommandSync("0", "open", {}, [](CommandResponse cr) {
        printCommandResponse(cr, "Opening Supernova");
    });

    // Set bus voltage
    dispatcher.invokeCommandSync("0", "i2c_spi_uart_set_bus_voltage", {{"busVoltageInV", "3.3"}}, [](CommandResponse cr) {
        printCommandResponse(cr, "Setting Bus Voltage");
    });

    // Set I2C parameters
    dispatcher.invokeCommandSync("0", "i2c_set_parameters", {{"clockFrequencyInKHz", "400"}}, [](CommandResponse cr) {
        printCommandResponse(cr, "Setting Clock Frequency");
    });

    // Write 10 zeros to subaddress 0000
    dispatcher.invokeCommandSync("0", "i2c_write", {{"address", "50"}, {"writeBuffer", "000000000000000000000000"}}, [](CommandResponse cr) {
        printCommandResponse(cr, "Write 10 zeros to subaddress 0000");
    });

    // Read 10 bytes from subaddress 0000
    dispatcher.invokeCommandSync("0", "i2c_write", {{"address", "50"}, {"writeBuffer", "0000"}}, [](CommandResponse cr) {});
    dispatcher.invokeCommandSync("0", "i2c_read", {{"address", "50"}, {"bytesToRead", "10"}}, [](CommandResponse cr) {
        printCommandResponse(cr, "Reading 10 bytes from subaddress 0000");
    });

    // Write to I2C address using subaddress
    dispatcher.invokeCommandSync("0", "i2c_write_using_subaddress", {
        {"address", "50"},
        {"subaddress", "0005"},
        {"writeBuffer", "0102030405"}
    }, [](CommandResponse cr) {
        printCommandResponse(cr, "Write [01,02,03,04,05] to subaddress 0005");
    });

    // Read from I2C address using subaddress
    dispatcher.invokeCommandSync("0", "i2c_read_using_subaddress", {
        {"address", "50"},
        {"subaddress", "0000"},
        {"bytesToRead", "10"}
    }, [](CommandResponse cr) {
        printCommandResponse(cr, "Read 10 bytes from subaddress 0000");
    });

    // Exit
    dispatcher.invokeCommandSync("0", "exit", {});

    dispatcher.stop();

    return 0;
}
