# MissionControlTowerSDK
A C++ dynamic library for launching and interacting with MissionControlTower system

## Installing the development environment

1. Clone this repo.

   ```shell
   git clone git@github.com:binhollc/MissionControlTowerSDK.git
   ```

2. Navigate to the project folder.

3. Initialize the submodules.

   ```shell
   git submodule update --init
   ```

## Quick Start with Script

For developers who prefer a quicker setup, we provide convenient scripts that automate the build and staging process. Depending on your operating system, use the appropriate script to skip manual steps:

- **Mac/Linux**: Run `build_and_stage.sh` to automatically build and stage the application. Use the following command in your terminal:

  ```bash
  ./build_and_stage.sh
  ```

- **Windows**: Use `build_and_stage.bat` for an automated setup in Windows environments. Execute the script in your Command Prompt:

  ```cmd
  build_and_stage.bat
  ```

These scripts are designed to handle all necessary steps, ensuring a seamless setup process. If you encounter any issues or need to understand the steps being automated, refer to the detailed instructions in the preceding sections.

## Detailed Build and Stage Instructions

### Building the Python Backend

To build the Python backend, which is contained in the `python-backend` folder, we use cx_Freeze, a Python packaging tool. The following instructions guide you through the process.

#### Special note for Windows users

> :warning: ** On Windows 64 **: Make sure that Python 64, and not 32, is installed in your system.

##### How do I know if my Windows installation is 32 or 64 bits?

1. Press the Windows key + R to open the Run dialog box.

2. Type "msinfo32" and press Enter.

3. The System Information window will open. Look for the "System Type" field, which will indicate whether your Windows is 32-bit or 64-bit.

#### How can I know if I'm using Python 32 or 64 bits on windows?

To determine whether you're using a 32-bit or 64-bit version of Python on Windows, you can follow these steps:

1. Open the Command Prompt: Press the Windows key, type "Command Prompt," and select the Command Prompt application.

2. In the Command Prompt, type the following command and press Enter:

   ```shell
   python --version
   ```

   This command will display the version of Python you have installed, along with additional information.

   If you see something like `Python 3.x.x`, where `x.x` represents the version number, without any mention of "64-bit" or "32-bit," it means you have the 32-bit version of Python installed.

   If you see something like `Python 3.x.x [MSC v.1916 64 bit (AMD64)]`, where `x.x` represents the version number, and it includes "64 bit" or "AMD64," it indicates that you have the 64-bit version of Python installed.

#### Creating a virtual environment

1. Create a virtual environment and save it in the `venv` folder:

   On Linux / Mac:

   ```shell
   python3 -m venv venv
   ```

   On Windows:

   ```shell
   python -m venv venv
   ```

2. Activate the virtual environment:

   On Linux / Mac:

   ```shell
   source ./venv/bin/activate
   ```

   On Windows:

   ```shell
   venv\\Scripts\\Activate
   ```

3. Verify that your command prompt has an indicator that the virtual environment is active.

4. Install the cx_Freeze package:

   ```shell
   pip install cx_Freeze
   ```
   Note: inside the virtual environment you can safely use `python` to execute the interpreter.

5. Set the GH_TOKEN evironment variable.

   On Mac/Linux:
   ```shell
   export GH_TOKEN=[GitHub Personal Access Token]
   ```

   On Windows:
   ```shell
   set GH_TOKEN=[GitHub Personal Access Token]
   ```

6. Install the requirements.

   ```shell
   $(venv)> pip install -r python-backend/requirements.txt

7. Run the following command to build the executable:

   ```shell
   $(venv)> python setup.py build
   ```

   This command invokes cx_Freeze using the `setup.py` script, which contains the necessary configuration to freeze the Python backend into an executable.

8. After the build process completes, the executable and its dependencies will be located in the `build_bridge` directory.

#### Testing the bridge

10. Navigate to the `build_bridge` directory and test that the bridge works properly.

   On Linux / Mac:

   ```shell
   cd build_bridge
   ./bridge BinhoNova
   ```

   On Windows:

   ```shell
   cd build_bridge
   bridge.exe BinhoNova
   ```

   Now the ListUsbDevices should be ready and waiting to receive JSON commands.

11. Send the command to open the Nova simulator:

   ```json
   {"command":"open","params":{"address":"NovaSimulatedPort"},"transaction_id":"0"}
   ```

12. Verify that the bridge returns the following command response:

   ```json
   {"transaction_id": "0", "status": "success", "is_promise": false, "data": {"command": "open", "id": "BINHONOVASIM001", "port": "NovaSimulatedPort", "productName": "Binho Nova", "firmwareVersion": "0.2.8", "hardwareVersion": "1.0", "vendorId": "1240", "productId": "60724", "mode": "normal"}}
   ```

13. Exit the bridge using the following command:

   ```json
   {"command":"exit","transaction_id":"0"}
   ```

#### Killing the bridge

On Mac/Linux you can kill the bridge executable by pressing Command+C on Mac, or Control+C on Linux.

On Windows, if you are using PowerShell you can open a different PowerShell window and execute the following command:

```shell
Stop-Process -Name "bridge"
```

### Dynamic library

#### Special note for Windows developers

```
Ensure that your application and all its dependent libraries are compiled for the same architecture, either all for 32-bit or all for 64-bit. Mixing 32-bit and 64-bit applications and libraries often leads to errors like the one you're seeing.
```

If you get an error code `0xc000007b` when executing your app, it is most likely because the DLL was compiled for a different architecture.

##### How to solve this issue

For Windows users, we recommend downloading CMake's command line tool from the [official download page](https://cmake.org/download/). Make sure that you download the right installer for your system architecture - e.g. CMake 64 bits for your Windows 64 bits.

#### Prerequisites

- CMake
- Make
- C++ 17
  - For Windows users, we recommend downloading the [build tools for Visual Studio](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022).

#### Building the library

1. Navigate to the root of your project (where the `CMakeLists.txt` file is).

2. Create a new directory for the build files. You can call it `build` or anything you like.

    ```bash
    mkdir build
    ```

3. Then navigate into this directory.

    ```bash
    cd build
    ```

4. Configure `cmake`.

   You can select the Release or Debug variant by setting the CMAKE_BUILD_TYPE parameter. For example, if you want to generate debug symbols and messages:

   ```bash
   cmake -DCMAKE_BUILD_TYPE=Debug ..
   ```

   Alternatively, if you want to optimize for speed and disable debug information:

   ```bash
   cmake -DCMAKE_BUILD_TYPE=Release ..
   ```

   On Windows it is recommended to select the generator (-G argument) and the platform (-A argument). The platform can be "win32" or "win64":

   ```bash
   cmake -DCMAKE_BUILD_TYPE=Debug .. -G "Visual Studio 17 2022" -A win32
   ```

5. Within the build folder, execute the following command:

   ```shell
   cmake --build . --config Release --target install
   ```

After the build process completes, all the generated files, including the shared library, examples and docs will be installed in the staging/ directory.

#### Staging the Bridge

1. If you want to stage the Bridge, execute:

   ```shell
   cp -R build_bridge staging\bridge
   ```

## Staging Directory

After executing the build scripts or manually building and installing the library and examples, the resulting folder structure will be organized for ease of distribution and usage. Below is an approximate overview of the directory structure on Mac/Linux systems:

```
├── staging
│   ├── README.md
│   ├── bridge
│   │   ├── bridge [Bridge executable]
│   │   ├── ...
│   ├── examples
│   │   ├── list_devices
│   │   ├── nova_breathing_leds
│   │   ├── supernova_101
│   │   ├── supernova_i2c
│   ├── include
│   │   ├── bridge_reader.h
│   │   ├── BridgeReader_windows.h
│   │   ├── CommandDispatcher.h
│   │   ├── CommandManager.h
│   │   ├── CommandRequest.h
│   │   ├── CommandResponse.h
│   │   ├── definitions.h
│   ├── libbmc_sdk.0.1.0.dylib /  libbmc_sdk.so.0.1.0
│   ├── libbmc_sdk.dylib / libbmc_sdk.so
```

When distributing this tool, package the entire `staging` directory. Users can then access the tool's functionalities, its examples, and all the required dependencies in a structured manner.

## Executing the Sample Apps

The sample app uses the dynamic library, which executes the bridge. This means that BOTH the bridge executable AND the library must be reachable in the path. Before executing the app, you should add the directory that contains the bridge executable AND the directory that contains the library to the PATH environment variable.

Alternatively, you can prepend the PATH variable to the command execution:

1. Navigate to the folder containing the sample app’s built executable.

2. Update the PATH and execute the app:

   On Mac:

   ```shell
   DYLD_LIBRARY_PATH=/path/to/staging/lib PATH=$PATH:/path/to/staging/bridge ./sample_app
   ```

   On Linux:

   [TO DO]

   On Windows (using Command Prompt):

   ```shell
   set PATH=%PATH%;\path\to\staging\bridge;\path\to\staging\bin
   sample_app.exe
   ```

   Or on Windows (using PowerShell):

   ```shell
   $env:PATH += ";\path\to\staging\bridge\;\path\to\staging\bin"
   .\sample_app.exe
   ```

   Replace `/path/to/staging` with the actual path to your `staging` directory. Ensure you use the correct slashes for your operating system (`/` for Mac/Linux, `\` for Windows).

   ### Additional Examples

   The repository includes several examples under the `examples` folder. The instructions to build and execute the examples are analogous to the ones for the sample app.

## Troubleshooting

### The system can't find the library

Error:

```
---------------------------
sample_app.exe - System Error
---------------------------
The code execution cannot proceed because bmc_sdk.dll was not found. Reinstalling the program may fix this problem.
```

Solution:

Add `/path/to/staging/` to the PATH environment variable.

### CreateProcess failed

Solution:

Add `/path/to/staging/bridge` to the PATH environment variable.
