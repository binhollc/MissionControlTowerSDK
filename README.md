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

   ```shell
   python3 -m venv venv
   ```

2. Activate the virtual environment:

   ```shell
   source ./venv/bin/activate
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

   ```shell
   cd build_bridge
   ./bridge BinhoNova
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

After the build process completes, the library file libmct_api.so (or libmct_api.dylib on macOS) will be located in the lib directory.

### Building the Sample App

To build the sample app that demonstrates how to use the mct_api library, follow these steps:

1. Open a terminal or command prompt.

2. Navigate to the sample_app directory.

3. As you may notice, there is a CMakeLists.txt file under the folder `sample_app`. We are going to follow the same steps that we did for building the library, but this time we will use the `sample_app` folder as the base folder instead of the project folder as we did before.

4. Have you followed the same steps that we did for building the library? ;)

   ```
   For Windows developers we strongly recommend you to explicitly specify the platform (x32 or x64) when building the library and the sample_app. See special note above.
   ```

5. Navigate to the `sample_app` folder and execute the following command:

   ```shell
   cmake --build build/
   ```

After the build process completes, the executable file sample_app will be located in the build directory.

Note: Make sure you have already built the mct_api library as described in the 'Building the Library' section before building the sample app. The sample app depends on the library.

### Executing the Sample App

The sample app uses the dynamic library, which executes the bridge. This means that the bridge executable must
be reacheable in the path. Before executing the app, you should add the directory that contains the bridge executable
to the PATH environment variable.

Alternatively, if you are using bash or zsh, you can prepend the PATH variable to
the command execution:

1. Navigate to the folder containing the sample app.

2. Add the PATH as a prefix of the command execution:

```shell
PATH=$PATH:/path/to/MissionControlTowerSDK/build_bridge/ ./sample_app
```
