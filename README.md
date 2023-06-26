# MissionControlTowerSDK
A C++ dynamic library for launching and interacting with MissionControlTower system

## Building the Python Backend

To build the Python backend, which is contained in the `python-backend` folder, we use cx_Freeze, a Python packaging tool. The following instructions guide you through the process.

### Creating a virtual environment

1. Create a virtual environment and save it in the `venv` folder:

   ```shell
   python3 -m venv venv
   ```

2. Activate the virtual environment:

   ```shell
   ./venv/bin/activate
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

7. Run the following command to build the executable:

   ```shell
   $(venv)> python3 setup.py build
   ```

   This command invokes cx_Freeze using the `setup.py` script, which contains the necessary configuration to freeze the Python backend into an executable.

8. After the build process completes, the executable and its dependencies will be located in the `build_bridge` directory.

### Running the Executable

To run the executable, navigate to the `build` directory and execute the `bridge.exe` on Windows, or `bridge` on Mac/Linux.

**Note:** If you encounter any issues during the build process or when running the executable, ensure that you have the necessary dependencies and that your environment is correctly set up.

Feel free to adjust the instructions as needed based on your project's specific requirements or directory structure.

### Killing the Executable

On Mac/Linux you can kill the bridge executable by pressing Command+C on Mac, or Control+C on Linux.

On Windows, if you are using PowerShell you can open a different PowerShell window and execute the following command:

```shell
Stop-Process -Name "bridge"
```

### Building the Library

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
