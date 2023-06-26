import sys
import os
from cx_Freeze import setup, Executable

# Define the submodule folder path
SUBMODULE_FOLDER = "python-backend"

# Define the output folder name
OUTPUT_FOLDER = "backend-build"

# Define the executables to build
executables = [Executable(script=SUBMODULE_FOLDER + "/bridge.py")]

base_dir = os.path.dirname(__file__)

sub_dirs = [
    "python-deps/mission_control_tower",
    "python-deps/nova_sdk",
    "python-deps/supernova_sdk/SupernovaSDK",
    "python-backend"
]

include_path = sys.path + [os.path.abspath(os.path.join(base_dir, sub_dir)) for sub_dir in sub_dirs]

# Set up the freezing options
options = {
    "build_exe": {
        "build_exe": "build_bridge",
        "path": include_path,
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
