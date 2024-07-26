#include "CommandDispatcher.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <algorithm>
#include <cmath>
#include <fstream>
#include <numeric>
#include <map>
#include <string>
#include <random>

void process_response(const CommandResponse& cr) {
    if (cr.status != "success") {
        std::cerr << "Error occurred.\n";
    }
}

double calculate_mean(const std::vector<double>& data) {
    return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
}

double calculate_variance(const std::vector<double>& data, double mean) {
    double variance = 0.0;
    for (const auto& value : data) {
        variance += (value - mean) * (value - mean);
    }
    return variance / data.size();
}

void save_statistics(const std::string& filename, const std::vector<double>& data) {
    std::ofstream file(filename);
    if (file.is_open()) {
        for (const auto& value : data) {
            file << value << "\n";
        }
        file.close();
    }
}

// Helper function to convert data vector to hex string
std::string vector_to_hex_string(const std::vector<uint8_t>& data) {
    std::ostringstream oss;
    for (const auto& byte : data) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
    }
    return oss.str();
}

int main() {
    CommandDispatcher dispatcher("BinhoSupernova");

    dispatcher.start();

    // Open device
    dispatcher.invokeCommandSync("0", "open", {}, process_response);

    std::vector<int> frequencies = {100000, 400000, 1000000};
    std::vector<int> data_lengths = {10, 1024};
    int nro_of_experiments = 1000;
    bool randomize_order = true; // Set this to true to randomize the trial order
    int recovery_delay_ms = 100; // Set this to 0 for no delay between command invocations

    // Randomize the order of frequencies and data lengths if required
    if (randomize_order) {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(frequencies.begin(), frequencies.end(), g);
        std::shuffle(data_lengths.begin(), data_lengths.end(), g);
    }

    for (const auto& data_length : data_lengths) {
        std::vector<uint8_t> data(data_length, 20);

        for (const auto& frequency : frequencies) {
            std::cout << "Time analysis for frequency " << frequency << "Hz and data length of " << data_length << ".\n";

            dispatcher.invokeCommandSync("0", "i2c_set_parameters", {{"clockFrequencyInKHz", std::to_string(frequency / 1000)}}, process_response);
            dispatcher.invokeCommandSync("0", "i2c_spi_uart_set_bus_voltage", {{"busVoltageInV", "3.3"}}, process_response);

            std::vector<double> round_trip_times;
            round_trip_times.reserve(nro_of_experiments);

            for (int i = 0; i < nro_of_experiments; ++i) {
                auto start = std::chrono::high_resolution_clock::now();

                dispatcher.invokeCommandSync("0", "i2c_write_using_subaddress", {
                    {"address", "50"},
                    {"subaddress", "0000"},
                    {"writeBuffer", vector_to_hex_string(data)}
                }, process_response);

                auto end = std::chrono::high_resolution_clock::now();

                // Give the I2C device some milliseconds to recover if needed
                if (recovery_delay_ms > 0) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(recovery_delay_ms));
                }

                std::chrono::duration<double, std::milli> round_trip_time = end - start;
                round_trip_times.push_back(round_trip_time.count());
            }

            double mean_value = calculate_mean(round_trip_times);
            double variance_value = calculate_variance(round_trip_times, mean_value);
            double min_value = *std::min_element(round_trip_times.begin(), round_trip_times.end());
            double max_value = *std::max_element(round_trip_times.begin(), round_trip_times.end());
            std::nth_element(round_trip_times.begin(), round_trip_times.begin() + round_trip_times.size() / 2, round_trip_times.end());
            double median_value = round_trip_times[round_trip_times.size() / 2];

            std::cout << "Min: " << min_value << " ms.\n";
            std::cout << "Max: " << max_value << " ms.\n";
            std::cout << "Mean: " << mean_value << " ms.\n";
            std::cout << "Variance: " << variance_value << "\n";
            std::cout << "Median: " << median_value << " ms.\n";

            std::string filename = "results_Freq_" + std::to_string(frequency) + "_Length_" + std::to_string(data_length) + ".txt";
            save_statistics(filename, round_trip_times);
        }
    }

    // Exit
    dispatcher.invokeCommandSync("0", "exit", {}, process_response);
    dispatcher.stop();

    return 0;
}
