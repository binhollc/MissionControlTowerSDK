#include "sample_library.h"
#include <iostream>
#include <fstream>
#include <string>

int main() {
    SampleLibrary device_api;
    std::ofstream outfile("output.txt");

    if (!outfile.is_open()) {
        std::cerr << "Error: Unable to open file for writing.\n";
        return 1;
    }

    try {
        auto address = device_api.get_address();
        if (address.empty()) {
            std::cerr << "Error: No device address found.\n";
            return 1;
        }
        outfile << address << "\n";

        device_api.load_values();

        outfile << device_api.get_fw_version() << "\n";
        outfile << device_api.get_hw_version() << "\n";
        outfile << device_api.get_serial_number() << "\n";
    }
    catch (const std::runtime_error& e) {
        std::cerr << "Runtime error: " << e.what() << "\n";
        return 1;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
        return 1;
    }
    catch (...) {
        std::cerr << "An unknown error occurred.\n";
        return 1;
    }

    outfile.close();  // Close the file
    return 0;
}
