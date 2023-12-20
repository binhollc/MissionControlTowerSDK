#include "CommandDispatcher.h"
#include <iostream>
#include <chrono>

void process_response(const CommandResponse& cr) {
    if (cr.status != "success") {
        std::cerr << "Error occurred.\n";
    }
}

int main() {
    CommandDispatcher dispatcher("BinhoSupernova");

    dispatcher.start();

    // Open device
    dispatcher.invokeCommandSync("0", "open", {}, process_response);
    // dispatcher.invokeCommandSync("0", "open", {{"address", "SupernovaSimulatedPort"}}, process_response);

    // Set bus voltage
    dispatcher.invokeCommandSync("0", "i2c_spi_uart_set_bus_voltage", {{"busVoltageInV", "3.3"}}, process_response);

    // Set I2C parameters
    dispatcher.invokeCommandSync("0", "i2c_set_parameters", {{"clockFrequencyInKHz", "400"}}, process_response);

    double total_round_trip_time = 0.0;
    for (int i = 0; i < 100; ++i) {
        auto start = std::chrono::high_resolution_clock::now();

        // Write to I2C address using subaddress
        dispatcher.invokeCommandSync("0", "i2c_write_using_subaddress", {
            {"address", "50"},
            {"subaddress", "0000"},
            {"writeBuffer", "010203040506"}  // Example write buffer, replace as needed
        }, process_response);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> round_trip_time = end - start;

        total_round_trip_time += round_trip_time.count();
    }

    double average_round_trip_time = total_round_trip_time / 100;
    std::cout << "Average round-trip time: " << average_round_trip_time << " ms\n";

    // Exit
    dispatcher.invokeCommandSync("0", "exit", {});

    dispatcher.stop();

    return 0;
}
