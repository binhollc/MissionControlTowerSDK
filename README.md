# Binho Mission Control C++ SDK (BMC C++ SDK)
A C++ dynamic library for interacting with Binho host adapters

## Prerequisites

BMC C++ SDK relies on the bridge service to be installed on your machine. Follow these steps to get everything set up:

1. **Download and Install the Bridge Service:**
   - Visit [support.binho.io](https://support.binho.io) to download the bridge executable.
   - Follow the installation instructions specific to your operating system.

2. **Add the Bridge to Your PATH Environment Variable:**
   - **Windows:**
     - The bridge is typically installed in `C:\Program Files (x86)\BinhoMissionControlBridge`.
     - Add this path to your system's PATH environment variable.
       - Press `Win + R`, type `sysdm.cpl`, and press Enter.
       - Go to the Advanced tab, and click on Environment Variables.
       - In the System variables section, find the PATH variable, select it, and click Edit.
       - Click New and add `C:\Program Files (x86)\BinhoMissionControlBridge`.
     - To verify the installation, open Command Prompt and execute:
       ```cmd
       bridge --version
       ```
       - You should see an output similar to `0.13.0`.

   - **Mac/Linux:**
     - Add the bridge executable path to your PATH environment variable by editing your shell profile file (e.g., `~/.bashrc`, `~/.zshrc`).
       ```sh
       export PATH=$PATH:/path/to/BinhoMissionControlBridge
       ```
     - Apply the changes:
       ```sh
       source ~/.bashrc  # or source ~/.zshrc
       ```
     - Verify the installation:
       ```sh
       bridge --version
       ```
       - You should see an output similar to `0.13.0`.

## Installing the development environment

1. Clone this repo.

   ```shell
   git clone git@github.com:binhollc/MissionControlTowerSDK.git
   ```

2. Navigate to the project folder.

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

### Dynamic Library

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

   On Windows it is recommended to select the generator (-G argument) and the platform (-A argument). The platform can be "win32" or "x64":

   ```bash
   cmake -DCMAKE_BUILD_TYPE=Debug .. -G "Visual Studio 17 2022" -A win32
   ```

5. Within the build folder, execute the following command:

   ```shell
   cmake --build . --config Release --target install
   ```

After the build process completes, all the generated files, including the shared library, examples and docs will be installed in the staging/ directory.

## Staging Directory

After executing the build scripts or manually building and installing the library and examples, the resulting folder structure will be organized for ease of distribution and usage. Below is an approximate overview of the directory structure on Mac/Linux systems:

```
├── staging
│   ├── README.md
│   ├── examples
│   │   ├── list_devices
│   │   ├── nova_breathing_leds
│   │   ├── sample_app_using_sample_library
│   │   ├── sample_library
│   │   ├── supernova_101
│   │   ├── supernova_i2c
│   │   ├── supernova_i2c_benchmark
│   ├── include
│   │   ├── bridge_reader.h
│   │   ├── BridgeReader_windows.h
│   │   ├── CommandDispatcher.h
│   │   ├── CommandManager.h
│   │   ├── CommandRequest.h
│   │   ├── CommandResponse.h
│   │   ├── definitions.h
│   ├── bin (Windows)
│   │   ├── bmc_sdk.dll (Windows)
│   ├── lib
│   │   ├── bmc_sdk.lib (Windows)
│   │   ├── libbmc_sdk.dylib (Mac)
│   │   ├── libbmc_sdk.so (Linux)
```

When distributing this tool, package the entire `staging` directory. Users can then access the tool's functionalities, its examples, and all the required dependencies in a structured manner.

## Executing the Sample Apps

The sample app uses the dynamic library, which executes the bridge. This means that BOTH the bridge executable AND the library must be reachable in the path. Before executing the app, you should add the directory that contains the bridge executable AND the directory that contains the library to the PATH environment variable.

Alternatively, you can prepend the PATH variable to the command execution:

1. Navigate to the folder containing the sample app’s built executable.

2. Update the PATH and execute the app:

   On Mac:

   ```shell
   DYLD_LIBRARY_PATH=/path/to/staging/lib PATH=$PATH:/path/to/bridge ./sample_app
   ```

   On Linux:

   ```shell
   LD_LIBRARY_PATH=/path/to/staging/lib PATH=$PATH:/path/to/bridge ./sample_app
   ```

   On Windows (using Command Prompt):

   ```shell
   set PATH=%PATH%;\path\to\bridge;\path\to\staging\bin
   sample_app.exe
   ```

   Or on Windows (using PowerShell):

   ```shell
   $env:PATH += ";\path\to\bridge\;\path\to\staging\bin"
   .\sample_app.exe
   ```

   Replace `/path/to/staging` with the actual path to your `staging` directory. Ensure you use the correct slashes for your operating system (`/` for Mac/Linux, `\` for Windows).

   ### Additional Examples

   The repository includes several examples under the `examples` folder. The instructions to build and execute the examples are analogous to the ones for the sample app.

## Automated Tests

### Build

Preconditions:
   - The project is built and staged.
   - PATH (and DYLD_LIBRARY_PATH in Mac) environment variables are correctly set.

On Windows 64 bits:
```shell
cd tests
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
cd build
ctest
```

On Windows 32 bits: Change `-A x64` to `-A win32` in the second line above.

On Mac/Linux:
```shell
cd tests
cmake -S . -B build
cmake --build build
cd build
ctest
```

## Building MissionControlTowerSDK Installer (Windows)

To build the installer for MissionControlTowerSDK, follow the steps below:

### Prerequisites
- NSIS (Nullsoft Scriptable Install System): You need to have NSIS installed on your system to compile the bmc_sdk_installer.nsi script.

### Compilingt the installer

#### 1. Locate the .nsi Script
Navigate to the root directory of this repository, where the MissionControlTowerSDK.nsi script is located.

#### 2. Compile the script
Right click on the nsi script and select `Compile NSIS Script`. NOTE: In Windows 11, this option is available after selecting `Show More Options` when right-clicking.

#### 3. Generated Installer
After compiling, the installer MissionControlTowerSDKInstaller.exe will be generated in the same directory as the .nsi script.

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

Add `/path/to/bridge` to the PATH environment variable.
