# MissionControlTowerSDK
A C++ dynamic library for launching and interacting with MissionControlTower system

## Building the Python Backend

To build the Python backend, which is contained in the `python-backend` folder, you can use cx_Freeze, a Python packaging tool. The following instructions guide you through the process.

### Prerequisites

Before building the Python backend, ensure that you have the cx_Freeze package installed. If you don't have it installed, you can install it using pip:

```shell
pip install cx_Freeze
```

### Building the Executable

1. Open a terminal or command prompt.

2. Navigate to the root directory of the project.

3. Run the following command to build the executable:

   ```shell
   python setup.py build
   ```

   This command invokes cx_Freeze using the `setup.py` script, which contains the necessary configuration to freeze the Python backend into an executable.

4. After the build process completes, the executable and its dependencies will be located in the `build/exe.[platform identifier].[python version]` directory.

   - The `[platform identifier]` represents the platform-specific build, such as `macosx-10.9-x86_64` for macOS or `win32` for Windows.
   - The `[python version]` indicates the Python version used for the build, such as `3.10`.

### Running the Executable

To run the executable, navigate to the `build/exe.[platform identifier].[python version]` directory and execute the appropriate executable file for your platform.

**Note:** If you encounter any issues during the build process or when running the executable, ensure that you have the necessary dependencies and that your environment is correctly set up.

Feel free to adjust the instructions as needed based on your project's specific requirements or directory structure.
