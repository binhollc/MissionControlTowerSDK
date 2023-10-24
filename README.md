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


## Building the Python Backend

To build the Python backend, which is contained in the `python-backend` folder, we use cx_Freeze, a Python packaging tool. The following instructions guide you through the process.

### Special note for Windows users

> :warning: ** On Windows 64 **: Make sure that Python 64, and not 32, is installed in your system.

#### How do I know if my Windows installation is 32 or 64 bits?

1. Press the Windows key + R to open the Run dialog box.

2. Type "msinfo32" and press Enter.

3. The System Information window will open. Look for the "System Type" field, which will indicate whether your Windows is 32-bit or 64-bit.

### How can I know if I'm using Python 32 or 64 bits on windows?

To determine whether you're using a 32-bit or 64-bit version of Python on Windows, you can follow these steps:

1. Open the Command Prompt: Press the Windows key, type "Command Prompt," and select the Command Prompt application.

2. In the Command Prompt, type the following command and press Enter:

   ```shell
   python --version
   ```

   This command will display the version of Python you have installed, along with additional information.

   If you see something like `Python 3.x.x`, where `x.x` represents the version number, without any mention of "64-bit" or "32-bit," it means you have the 32-bit version of Python installed.

   If you see something like `Python 3.x.x [MSC v.1916 64 bit (AMD64)]`, where `x.x` represents the version number, and it includes "64 bit" or "AMD64," it indicates that you have the 64-bit version of Python installed.

### Creating a virtual environment

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

5. Execute the script that creates the `requirements.txt` file:

   ```shell
   $(venv)> python create_requirements.py
   ```

6. Verify that `requirements.txt` has been created in the base folder of the project.

7. Install the requirements.

   ```shell
   $(venv)> pip install -r requirements.txt

8. Run the following command to build the executable:

   ```shell
   $(venv)> python setup.py build
   ```

   This command invokes cx_Freeze using the `setup.py` script, which contains the necessary configuration to freeze the Python backend into an executable.

9. After the build process completes, the executable and its dependencies will be located in the `build_bridge` directory.

### Testing the bridge

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

### Killing the bridge

On Mac/Linux you can kill the bridge executable by pressing Command+C on Mac, or Control+C on Linux.

On Windows, if you are using PowerShell you can open a different PowerShell window and execute the following command:

```shell
Stop-Process -Name "bridge"
```

## Dynamic library

### Special note for Windows developers

```
Ensure that your application and all its dependent libraries are compiled for the same architecture, either all for 32-bit or all for 64-bit. Mixing 32-bit and 64-bit applications and libraries often leads to errors like the one you're seeing.
```

If you get an error code `0xc000007b` when executing your app, it is most likely because the DLL was compiled for a different architecture.

#### How to solve this issue

1. For Windows users, we recommend downloading CMake's command line tool from the [official download page](https://cmake.org/download/). Make sure that you download the right installer for your system architecture - e.g. CMake 64 bits for your Windows 64 bits.

2. When building the DLL you can specify the target architecture when you run cmake, like so:

   - For 32-bit: cmake -DCMAKE_GENERATOR_PLATFORM=x86 ..
   - For 64-bit: cmake -DCMAKE_GENERATOR_PLATFORM=x64 ..

### Prerequisites

- CMake
- Make
- C++ 17
  - For Windows users, we recommend downloading the [build tools for Visual Studio](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022).

### Building the library

1. Navigate to the root of your project (where the `CMakeLists.txt` file is).

2. Create a new directory for the build files. You can call it `build` or anything you like.

    ```bash
    mkdir build
    ```
3. Then navigate into this directory.

    ```bash
    cd build
    ```
4. Run `cmake` from within this new directory, pointing it to the parent directory, which is where the `CMakeLists.txt` file is.

    ```bash
    cmake ..
    ```

   ```
   For Windows developers we strongly recommend you to explicitly specify the platform (x32 or x64) when building the library and the sample_app. See special note above.
   ```

5. Navigate to the base folder and execute the following command:

   ```shell
   cmake --build build/
   ```

After the build process completes, the library file libmct_api.so (libmct_api.dylib on macOS, mct_api.dll on Windows) will be located in the lib directory.

### Building the Sample App

To build the sample app that demonstrates how to use the libbmc_sdk library, follow these steps:

1. Open a terminal or command prompt.

2. Navigate to the `MissionControlTowerSDK` project’s base directory.

3. Copy the necessary files and directories to the `staging` directory:

   ```shell
   cp -R build_bridge staging
   cp build/libbmc_sdk.*.dylib staging
   cp -R include staging
   ```

4. Navigate to the `sample_app` directory and create a `build` directory:

   ```shell
   cd sample_app
   mkdir build
   ```

5. Navigate to the `build` directory:

   ```shell
   cd build
   ```

6. Run cmake to configure the project, specifying the `BMC_SDK_PATH`:

   ```shell
   cmake .. -DBMC_SDK_PATH=../../staging
   ```

7. Build the sample app:

   ```shell
   cmake --build .
   ```

After the build process completes, the executable file `sample_app` will be located in the `build` directory.

Note: Make sure you have already built the libbmc_sdk library as described in the 'Building the Library' section before building the sample app. The sample app depends on the library.

For Windows developers, the steps are the same but ensure that you open the Command Prompt or PowerShell and adapt the commands to the Windows environment. Also, specify the platform (x32 or x64) when running cmake commands.

### Executing the Sample App

The sample app uses the dynamic library, which executes the bridge. This means that the bridge executable must be reachable in the path. Before executing the app, you should add the directory that contains the bridge executable to the PATH environment variable.

Alternatively, you can prepend the PATH variable to the command execution:

1. Navigate to the folder containing the sample app’s built executable.

2. Update the PATH and execute the app:

   On Mac or Linux:

   ```shell
   PATH=$PATH:/path/to/MissionControlTowerSDK/staging/build_bridge/ ./sample_app
   ```

   On Windows (using Command Prompt):

   ```shell
   set PATH=%PATH%;\path\to\MissionControlTowerSDK\staging\build_bridge\
   sample_app.exe
   ```

   Or on Windows (using PowerShell):

   ```shell
   $env:PATH += ";\path\to\MissionControlTowerSDK\staging\build_bridge\"
   .\sample_app.exe
   ```

   Replace `/path/to/MissionControlTowerSDK` and `\path\to\MissionControlTowerSDK` with the actual path to your `MissionControlTowerSDK` directory. Ensure you use the correct slashes for your operating system (`/` for Mac/Linux, `\` for Windows).