// Example: I3C IBIS (In-Band Interrupts) with BMI323 and BinhoSupernova
//
// This example demonstrates how to use the MissionControlTowerSDK to configure and interact with a BMI323 sensor
// over I3C using a BinhoSupernova host adapter. It shows how to:
//   - Initialize the I3C controller and bus
//   - Set bus voltage
//   - Discover the target device by PID
//   - Enable and handle I3C In-Band Interrupts (IBI)
//   - Configure the BMI323 sensor registers
//   - Cleanly disable IBI and shut down the session
//
// The example uses the CommandDispatcher and prints responses for each command.

#include "CommandDispatcher.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <cstdint>
#include <iomanip>

// BMI323 Register Definitions (translated from Python)
const uint8_t BMI323_ACCEL_CONFIG_REG = 0x20;
const uint8_t BMI323_GYRO_CONFIG_REG = 0x21;
const uint8_t BMI323_INT_MAP1 = 0x3A;
const uint8_t BMI323_INT_MAP2 = 0x3B;

const uint8_t BMI323_ACCEL_DATA_X = 0x03;

// BMI323 Configuration Values
const uint8_t BMI323_ACCEL_MODE = 0b100;    // Enables the accelerometer in a continuous operation mode with reduced current
const uint8_t BMI323_ACCEL_AVG_NUM = 0b000; // No averaging; pass sample without filtering
const uint8_t BMI323_ACCEL_BW = 0b1;        // BW = acc_odr/4
const uint8_t BMI323_ACCEL_RANGE = 0b000;   // ±2g, 16.38 LSB/mg
const uint8_t BMI323_ACCEL_ODR = 0b0101;    // ODR = 12.5 Hz

const uint8_t BMI323_GYRO_MODE = 0b100;    // Enables the gyroscope in a continuous operation mode with reduced current
const uint8_t BMI323_GYRO_AVG_NUM = 0b000; // No averaging; pass sample without filtering
const uint8_t BMI323_GYRO_BW = 0b1;        // BW = gyro_odr/4
const uint8_t BMI323_GYRO_RANGE = 0b001;   // ±250º/s, 131.072 LSB/º/s
const uint8_t BMI323_GYRO_ODR = 0b0101;    // ODR = 12.5 Hz

const uint8_t BMI323_INT_MAP1_TILT_OUT            = 0b00; // Interrupt disabled
const uint8_t BMI323_INT_MAP1_SIG_MOTION_OUT      = 0b00; // Interrupt disabled
const uint8_t BMI323_INT_MAP1_STEP_COUNTER_OUT    = 0b00; // Interrupt disabled
const uint8_t BMI323_INT_MAP1_STEP_DETECTOR_OUT   = 0b00; // Interrupt disabled
const uint8_t BMI323_INT_MAP1_STEP_ORIENTATOR_OUT = 0b00; // Interrupt disabled
const uint8_t BMI323_INT_MAP1_STEP_FLAT_OUT       = 0b00; // Interrupt disabled
const uint8_t BMI323_INT_MAP1_STEP_ANY_MOTION_OUT = 0b00; // Interrupt disabled
const uint8_t BMI323_INT_MAP1_STEP_NO_MOTION_OUT  = 0b00; // Interrupt disabled

const uint8_t BMI323_INT_MAP2_FIFO_FULL_INT       = 0b00; // Interrupt disabled
const uint8_t BMI323_INT_MAP2_FIFO_WATERMARK_INT  = 0b00; // Interrupt disabled
const uint8_t BMI323_INT_MAP2_ACC_DRDY_INT        = 0b11; // Mapped to I3C IBI, Accelerator sample ready (DRDY)
const uint8_t BMI323_INT_MAP2_GYR_DRDY_INT        = 0b00; // Interrupt disabled, Gyroscope sample ready (DRDY)
const uint8_t BMI323_INT_MAP2_TEMP_DRDY_INT       = 0b00; // Interrupt disabled
const uint8_t BMI323_INT_MAP2_ERR_STATUS          = 0b00; // Interrupt disabled
const uint8_t BMI323_INT_MAP2_I3C_OUT             = 0b00; // Interrupt disabled
const uint8_t BMI323_INT_MAP2_TAP_OUT             = 0b00; // Interrupt disabled

void printCommandResponse(const CommandResponse &cr, const std::string &action)
{
    if (!cr.is_promise) {
        std::cout << "Action: " << action << "\n";
        std::cout << "Transaction ID: " << cr.transaction_id << "\n";
        std::cout << "Status: " << cr.status << "\n";
        // std::cout << "Is Promise: " << (cr.is_promise ? "True" : "False") << "\n";
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

int findDevice(CommandDispatcher& disp, CommandIDGenerator& idGen, std::vector<int> expectedPID) {
    CommandResponse response;
    bool commandSucceeded = false;
    int dynamicAddress = 0;

    disp.invokeCommandSync(idGen.nextID(), "i3c_controller_get_target_device_table", {},
        [&](const CommandResponse &cr) {
            response = cr;
            commandSucceeded = (cr.status == "success");
            if (commandSucceeded && cr.data.contains("result") && cr.data["result"].contains("payload")) {
                auto targets = cr.data["result"]["payload"];
                for (const auto& target : targets) {
                    if (target.contains("pid") && target.contains("dynamicAddress")) {
                        auto pid = target["pid"];
                        bool pidMatches = true;
                        if (pid.size() == expectedPID.size()) {
                            for (size_t i = 0; i < pid.size(); ++i) {
                                if ((int)pid[i] != expectedPID[i]) {
                                    pidMatches = false;
                                    break;
                                }
                            }
                            if (pidMatches) {
                                dynamicAddress = (int)target["dynamicAddress"];
                                return;
                            }
                        }
                    }
                }
            }
        });

    return dynamicAddress;
}

class BMI323Device {
private:
    CommandDispatcher& dispatcher;
    CommandIDGenerator& idGenerator;
    int dynamicAddress;

public:
    BMI323Device(CommandDispatcher& disp, CommandIDGenerator& idGen)
        : dispatcher(disp), idGenerator(idGen), dynamicAddress(0) {}

    void writeRegister(uint8_t regAddr, int16_t value) {
        // Format register address as 2-digit hex string
        auto hexString = [](uint8_t val) {
            std::ostringstream oss;
            oss << std::hex << std::setfill('0') << std::setw(2) << (int)val;
            return oss.str();
        };

        // Format value as 4-digit hex string (big-endian)
        uint8_t high = value & 0xFF;
        uint8_t low = (value >> 8) & 0xFF;

        std::ostringstream valueStream;
        valueStream << std::hex << std::setfill('0') << std::setw(2) << (int)high
                    << std::setw(2) << (int)low;

        // Prepare binary string for value
        std::string hexVal = valueStream.str();

        std::ostringstream actionMsg;
        actionMsg << "Write Register 0x" << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << (int)regAddr
                  << ": 0x" << hexVal;

        dispatcher.invokeCommandSync(
            idGenerator.nextID(),
            "i3c_write_using_subaddress",
            {
                {"address", dynamicAddress},
                {"subaddress", hexString(regAddr)},
                {"writeBuffer", hexVal},
                {"mode", "SDR"}
            },
            handleCommandResponse(actionMsg.str())
        );
    }

    std::vector<int> getPID() {
        return {0x07, 0x70, 0x10, 0x43, 0x10, 0x00};
    }

    void setDynamicAddress(int d) {
        dynamicAddress = d;
    }

    void initDevice() {
        // Configure accelerometer
        uint16_t acc_conf = 0;
        acc_conf |= (BMI323_ACCEL_MODE << 12); // bits 14-12
        acc_conf |= (BMI323_ACCEL_AVG_NUM << 8); // bits 10-8
        acc_conf |= (BMI323_ACCEL_BW << 7); // bit 7
        acc_conf |= (BMI323_ACCEL_RANGE << 4); // bits 6-4
        acc_conf |= BMI323_ACCEL_ODR; // bits 3-0

        std::cout << "acc_conf = 0x" << std::hex << std::setfill('0') << std::setw(2) << acc_conf << std::endl;

        writeRegister(BMI323_ACCEL_CONFIG_REG, acc_conf);

        // Configure gyroscope
        uint16_t gyr_conf = 0;
        gyr_conf |= (BMI323_GYRO_MODE << 12); // bits 14-12
        gyr_conf |= (BMI323_GYRO_AVG_NUM << 8); // bits 10-8
        gyr_conf |= (BMI323_GYRO_BW << 7); // bit 7
        gyr_conf |= (BMI323_GYRO_RANGE << 4); // bits 6-4
        gyr_conf |= BMI323_GYRO_ODR; // bits 3-0

        std::cout << "gyr_conf = 0x" << std::hex << std::setfill('0') << std::setw(2) << gyr_conf << std::endl;

        writeRegister(BMI323_GYRO_CONFIG_REG, gyr_conf);

        // Configure interrupts
        int16_t intMap1 = (BMI323_INT_MAP1_TILT_OUT << 14) |
                        (BMI323_INT_MAP1_SIG_MOTION_OUT << 12) |
                        (BMI323_INT_MAP1_STEP_COUNTER_OUT << 10) |
                        (BMI323_INT_MAP1_STEP_DETECTOR_OUT << 8) |
                        (BMI323_INT_MAP1_STEP_ORIENTATOR_OUT << 6) |
                        (BMI323_INT_MAP1_STEP_FLAT_OUT << 4) |
                        (BMI323_INT_MAP1_STEP_ANY_MOTION_OUT << 2) |
                        (BMI323_INT_MAP1_STEP_NO_MOTION_OUT << 0);

        writeRegister(BMI323_INT_MAP1, intMap1);

        int16_t intMap2 = (BMI323_INT_MAP2_FIFO_FULL_INT << 14) |
                        (BMI323_INT_MAP2_FIFO_WATERMARK_INT << 12) |
                        (BMI323_INT_MAP2_ACC_DRDY_INT << 10) |
                        (BMI323_INT_MAP2_GYR_DRDY_INT << 8) |
                        (BMI323_INT_MAP2_TEMP_DRDY_INT << 6) |
                        (BMI323_INT_MAP2_ERR_STATUS << 4) |
                        (BMI323_INT_MAP2_I3C_OUT << 2) |
                        (BMI323_INT_MAP2_TAP_OUT << 0);

        writeRegister(BMI323_INT_MAP2, intMap2);
    }
};

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

    std::cout << "BMI323 I3C Example\n";
    std::cout << "==================\n";

    // Open device
    dispatcher.invokeCommandSync(idGenerator.nextID(), "open", {}, 
        handleCommandResponse("Supernova Opened"));

    // Initialize I3C controller
    dispatcher.invokeCommandSync(idGenerator.nextID(), "i3c_controller_init", {
        {"pushPullClockFrequencyInMHz", 5}, 
        {"pushPullDutyCycle", 50}, 
        {"openDrainClockFrequencyInKHz", 400}, 
        {"i2cOpenDrainClockFrequencyInkHz", 400}
    }, handleCommandResponse("I3C Controller Initialized"));

    // Set bus voltage
    dispatcher.invokeCommandSync(idGenerator.nextID(), "i3c_set_bus_voltage", {
        {"busVoltageInV", 3.3}
    }, handleCommandResponse("Bus Voltage Set"));

    // Initialize bus
    dispatcher.invokeCommandSync(idGenerator.nextID(), "i3c_controller_init_bus", {}, 
        handleCommandResponse("I3C Bus Initialized"));

    // Create BMI323 device instance
    BMI323Device bmi323(dispatcher, idGenerator);

    std::vector<int> expectedPID = bmi323.getPID();

    // Find and configure the device
    std::cout << "Searching for BMI323 device...\n";
    int dynamicAddress = findDevice(dispatcher, idGenerator, expectedPID);
    if (!dynamicAddress) {
        std::cout << "Failed to find BMI323 device. Exiting...\n";
        dispatcher.invokeCommandSync(idGenerator.nextID(), "exit", {});
        dispatcher.stop();
        return 1;
    }
    std::cout << "BMI323 device found at dynamicAddress = 0x" << std::hex << std::setfill('0') << std::setw(2) << dynamicAddress << std::endl;

    bmi323.setDynamicAddress(dynamicAddress);

    dispatcher.invokeCommandSync(idGenerator.nextID(), "i3c_ccc_direct_enec", {
        {"address", dynamicAddress},
        {"events", {"ENINT"}}
    }, handleCommandResponse("Enable IBI"));

    // Initialize device
    std::cout << "Initializing BMI323 device...\n";
    bmi323.initDevice();
    std::cout << "BMI323 device initialized successfully\n";

    std::this_thread::sleep_for(std::chrono::milliseconds(2000));

    dispatcher.invokeCommandSync(idGenerator.nextID(), "i3c_ccc_direct_disec", {
        {"address", dynamicAddress},
        {"events", {"DISINT"}}
    }, handleCommandResponse("Disable IBI"));

    dispatcher.waitForAllCommands();

    dispatcher.invokeCommandSync(idGenerator.nextID(), "close", {});

    dispatcher.invokeCommandSync(idGenerator.nextID(), "exit", {});

    dispatcher.stop();

    return 0;
}
