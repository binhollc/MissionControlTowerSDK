// Example: Supernova I2C Benchmark - Performance Testing with BinhoSupernova
//
// This example demonstrates how to use the MissionControlTowerSDK to benchmark I2C communication
// performance with a BinhoSupernova host adapter. It shows how to:
//   - Open a connection to the device
//   - Perform repeated I2C write operations and measure round-trip time
//   - Print timing and status information for each operation
//   - Cleanly close and exit the session
//
// The example uses the CommandDispatcher pattern and is useful for evaluating I2C throughput and latency.

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
    dispatcher.invokeCommandSync("1", "open", {}, process_response);
    // dispatcher.invokeCommandSync("1", "open", {{"address", "SupernovaSimulatedPort"}}, process_response);

    // Initialize I2C controller
    dispatcher.invokeCommandSync("1", "i2c_controller_init", {
        {"clockFrequencyInKHz", 400},
        {"pullUpResistanceInOhm", "DISABLE"}
    }, process_response);

    // Set bus voltage
    dispatcher.invokeCommandSync("1", "i2c_spi_uart_set_bus_voltage", {{"busVoltageInV", "3.3"}}, process_response);

    double total_round_trip_time = 0.0;
    for (int i = 0; i < 100; ++i) {
        auto start = std::chrono::high_resolution_clock::now();

        // Write to I2C address using subaddress
        dispatcher.invokeCommandSync("0", "i2c_write_using_subaddress", {
            {"address", 0x50},
            {"subaddress", "0000"},
            {"writeBuffer", "010203040506"}  // Example write buffer, replace as needed
        }, process_response);

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> round_trip_time = end - start;

        total_round_trip_time += round_trip_time.count();
    }

    double average_round_trip_time = total_round_trip_time / 100;
    std::cout << "Average round-trip time: " << average_round_trip_time << " ms\n";

    // Close device
    dispatcher.invokeCommandSync("1", "close", {});

    // Exit
    dispatcher.invokeCommandSync("0", "exit", {});

    dispatcher.stop();

    return 0;
}
