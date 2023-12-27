# Binho Mission Control SDK

## Introduction

Welcome to the Binho Mission Control SDK (BMC_SDK), a set of software tools specifically designed to streamline your interaction with USB host adapters. Our SDK currently supports Binho Nova and Supernova.

## Installation

### Overview

The Binho Mission Control SDK installer will install the necessary components for the SDK on your system. This includes:

- The core SDK library (`bmc_sdk.dll` and `bmc_sdk.lib`).
- Example applications to test the SDK.
- The "bridge" service used by the library.
- Header files for integration (`include` directory).

### Instructions

1. **Download the Installer**
   Get the Binho Mission Control SDK installer named `BinhoMissionControlSDK.exe`.

2. **Run the Installer**
   Double-click on `BinhoMissionControlSDK.exe` and follow the on-screen prompts.

3. **Installation Directories**
   By default, the SDK will be installed into the `c:\Program Files (x86)\BinhoMissionControlSDK` directory. Inside this directory, you will find:
   - `bmc_sdk.dll`: The SDK dynamic link library.
   - `bmc_sdk.lib`: The SDK static library.
   - `examples`: A directory containing example applications.
   - `bridge`: A directory containing components for the bridge service used by the library.
   - `include`: A directory containing various header files necessary for SDK integration.

4. **Add to PATH Environment Variable**
   For the sample application to function correctly, the `bridge` directory must be added to your system's PATH environment variable. See the steps bellow.

5. **Integration**
   To integrate the Binho Mission Control SDK into your project:
   - Reference the SDK library using the `bmc_sdk.lib` file.
   - Include necessary headers from the `include` directory.

6. **Test the Example Applications**
   After setting up the PATH, you can run `sample_app.exe` to check the installation and get an understanding of how the SDK functions.

## Usage Examples

We provide a set of example applications to demonstrate how to use the library. These examples are designed to be built with your preferred development tools. Below are the general steps to build and run each sample application, along with a brief description of what each application demonstrates.

### Prerequisites

- A development environment compatible with C++17.
- The Binho Mission Control SDK installed on your system.

### Example Use Cases

**Nova Host Adapter**

- **nova_breathing_leds**: Simulates a breathing LED effect, showcasing PWM control with a BinhoNova host adapter.

**Supernova Host Adapter**

- **list_devices**: Lists connected USB devices, displaying details such as the product name, type, port, and manufacturer. This example highlights the capability to query and process information about connected devices using the CommandDispatcher framework.
- **supernova_101**: Demonstrates fetching USB strings like `MANUFACTURER`, `PRODUCT_NAME`, etc., from a BinhoSupernova host adapter.
- **supernova_i2C**: Showcases I2C operations using the BinhoSupernova host adapter.
- **supernova_i2c_benchmark**: Performs benchmarking of I2C communication with the BinhoSupernova host adapter. It measures and displays the average round-trip time for a series of I2C write operations, offering insight into the performance and efficiency of I2C communications.
- **sample_library**: A custom library that provides a simplified interface for interacting with Binho devices. It includes functions for retrieving device information such as firmware and hardware versions, and serial numbers.
- **sample_app_using_sample_library**: An application that demonstrates how to use the `sample_library` to interact with Binho devices. It shows how to integrate the custom library into a larger application, retrieve device information, and handle different types of exceptions.

### Building the Sample Applications

1. **Open the Project**:
   Navigate to the directory of the desired sample application (e.g., `supernova_101/`).

2. **Prepare for Build**:
   Set up your build environment. This might involve generating project files or configuring your build system. Ensure the Binho Mission Control SDK is properly referenced, including the library and header files.

3. **Build the Application**:
   Compile the application using your build system or IDE. You might need to specify build configurations such as the target platform or build type.

4. **Run the Sample Application**:
   After building, execute the sample application to test and explore its functionality. Ensure any required runtime dependencies or environment variables are set up correctly.

## Using the Library - General Structure

Using our library typically involves a series of steps to communicate with the target host adapter, execute commands, and handle the responses. Here's a high-level overview based on the provided sample applications:

### 1. **Include Necessary Headers**
Make sure to include the appropriate headers to gain access to the functionality of the library:

```cpp
#include "CommandDispatcher.h"
#include <iostream>
```

### 2. **Define Response Handler**
A common pattern in our examples is defining a function to handle the response from the commands sent to the target adapter. This function is usually responsible for displaying relevant information about the command's execution.

```cpp
void printCommandResponse(const CommandResponse& cr, const std::string& action);
```

### 3. **Initialize the Dispatcher**
Create an instance of `CommandDispatcher` and specify the name of the target host adapter:

```cpp
CommandDispatcher dispatcher("BinhoNova");   // For connecting with the Binho Nova host adapter
CommandDispatcher dispatcher("BinhoSupernova");   // For connecting with the Binho Supernova host adapter
```

> **Note**: The argument passed to the Dispatcher initializer is the name of the target host adapter.

### 4. **Start the Dispatcher**

Before sending any commands, ensure the dispatcher is started:

```cpp
dispatcher.start();
```

### 5. **Invoke Commands**

With the dispatcher running, you can now invoke commands:

- Synchronous Commands (waits for the command to finish before moving on):
```cpp
dispatcher.invokeCommandSync("transaction_id", "command_name", command_params, response_handler);
```

- Asynchronous Commands (doesn't wait for the command to finish):
```cpp
dispatcher.invokeCommand("transaction_id", "command_name", command_params);
```

### 6. **Wait for All Commands to Finish (Optional)**

If you've issued multiple commands and want to ensure they're all finished before proceeding, you can:

```cpp
dispatcher.waitForAllCommands();
```

### 7. **Stop the Dispatcher**

Once all tasks are done, ensure to stop the dispatcher:

```cpp
dispatcher.stop();
```

## Changelog

### v0.1.0

- Now supports Windows.
- Introduced the CommandDispatcher: Streamlines and improves synchronization handling.
- Released a new sample application: supernova_101.
- Enhanced the user experience and functionality of the existing sample app: nova_breathing_leds.
- Library renamed to BinhoMissionControlSDK (BMC_SDK) for clarity and improved recognition.
- Included a comprehensive README.md for users to better understand the distribution.
- Fully automated the build and distribution process for Windows.
- Implemented a macro that prints debug messages only when the NDEBUG flag is enabled by the developer.
- Adjusted the CommandResponse’s from_json function: Now accepts both numeric and string-typed transaction IDs for greater flexibility.

### v0.3.1

- Reduces roundtrip time.
- Adds more examples.
- Enhances supernova_i2c example.