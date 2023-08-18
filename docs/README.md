## Introduction

Welcome to the Binho Mission Control SDK (BMC_SDK), a set of software tools specifically designed to streamline your interaction with USB host adapters. Our SDK currently supports Binho Nova and Supernova.

## Installation

### Overview

The Binho Mission Control SDK installer will install the necessary components for the SDK on your system. This includes:

- The core SDK library (`bmc_sdk.dll` and `bmc_sdk.lib`).
- A sample application (`sample_app.exe`) to test the SDK.
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
   - `sample_app.exe`: A sample application to demonstrate the SDK's usage.
   - `bridge`: A directory containing components for the bridge service used by the library.
   - `include`: A directory containing various header files necessary for SDK integration.

4. **Add to PATH Environment Variable**  
   For the sample application to function correctly, the `bridge` directory must be added to your system's PATH environment variable. See the steps bellow.

5. **Integration**  
   To integrate the Binho Mission Control SDK into your project:
   - Reference the SDK library using the `bmc_sdk.lib` file.
   - Include necessary headers from the `include` directory.

6. **Test the Sample Application**  
   After setting up the PATH, you can run `sample_app.exe` to check the installation and get an understanding of how the SDK functions.

## Usage Examples

We provide a set of sample applications to demonstrate how to use the library. Below are the steps to build and run each sample application:

### Prerequisites

- Ensure you have `Visual Studio 2022` installed on your system.
- Navigate to the directory of the desired sample app (`supernova_101/` or `nova_breathing_leds/`).

### Example Use Cases

- **nova_breathing_leds**: This application simulates an LED's breathing effect by progressively increasing and decreasing its brightness using PWM on a `BinhoNova` host adapter.
  
- **supernova_101**: Demonstrates basic operations such as fetching USB strings like `MANUFACTURER`, `PRODUCT_NAME`, and others from a `BinhoSupernova` host adapter.

### Building the Sample Applications

1. **Navigate to the folder of the sample app**

2. **Create a Build Directory**:
   ```bash
   mkdir build
   ```

3. **Navigate to the Build Directory**:
   ```bash
   cd build
   ```

4. **Generate Project Files using CMake**:
   ```bash
   cmake .. -G "Visual Studio 17 2022" -A win32
   ```

5. **Build the Project**:
   ```bash
   cmake --build . --config Release
   ```

6. **Navigate to the Release Directory**:
   ```bash
   cd Release
   ```

7. **Update the PATH**:
   Make sure the BinhoMissionControlSDK and bridge directories are included in your `PATH`:
   ```bash
   set PATH=%PATH%;c:\Program Files (x86)\BinhoMissionControlSDK;c:\Program Files (x86)\BinhoMissionControlSDK\bridge
   ```

8. **Run the Sample Application**:
   ```bash
   sample_app.exe
   ```

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
