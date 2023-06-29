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
   {"command":"open","params":{"address":"SIM"},"transaction_id":"0"}
   ```

12. Verify that the bridge returns the following command response:

   ```json
   {"transaction_id": "0", "status": "success", "is_promise": false, "data": {"command": "open", "id": "BINHONOVASIM001", "port": "SIM", "productName": "Binho Nova", "firmwareVersion": "0.2.8", "hardwareVersion": "1.0", "mode": "normal"}}
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

### Building the library

1. Open a terminal or command prompt.

2. Navigate to the root directory of the project.

3. Run the following commands to build the library:

   ```shell
   cmake .
   ```

4. Build the library:

   ```shell
   make
   ```

After the build process completes, the library file libmct_api.so (or libmct_api.dylib on macOS) will be located in the lib directory.

### Building the Sample App

To build the sample app that demonstrates how to use the mct_api library, follow these steps:

1. Open a terminal or command prompt.

2. Navigate to the sample_app directory.

3. Run the following commands to build the sample app:

   ```shell
   cmake .
   ```

4. Build the app:

   ```shell
   make
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
