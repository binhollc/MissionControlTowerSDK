import sys
import os
from cx_Freeze import setup, Executable

# Define the submodule folder path
SUBMODULE_FOLDER = "python-backend"

# Define the output folder name
OUTPUT_FOLDER = "backend-build"

# Define the executables to build
executables = [Executable(script=SUBMODULE_FOLDER + "/bridge.py")]

# Set up the freezing options
options = {
    "build_exe": {
        "include_files": [(SUBMODULE_FOLDER + "/mock_command_adaptor.py", "mock_command_adaptor.py"), (SUBMODULE_FOLDER + "/mock_host_adapter_sdk.py", "mock_host_adapter_sdk.py")],
        "path": sys.path + [os.path.abspath(os.path.join(os.path.dirname(__file__), "mission-control-tower")), os.path.abspath(os.path.join(os.path.dirname(__file__), "python-backend"))],
        "excludes": ["tkinter"]
    }
}

# Perform the freezing process
setup(
    name="MissionControlTowerSDK",
    version="0.1",
    description="MissionControlTower SDK Standalone Distribution",
    executables=executables,
    options=options
)
