import os
import re

# Get the current directory
current_directory = os.getcwd()

# Path to the file
file_path = os.path.join(current_directory, "requirements.txt.sample")

# Read the file
try:
    with open(file_path, "r") as file:
        data = file.read()
except FileNotFoundError as err:
    print(err)
else:
    # Replace the placeholder with the current directory
    result = re.sub(r'\$\{PROJECT_PATH\}', current_directory, data)

    # Path to the output file
    output_file_path = os.path.join(current_directory, "requirements.txt")

    # Write the result to the output file
    try:
        with open(output_file_path, "w") as file:
            file.write(result)
    except FileNotFoundError as err:
        print(err)
