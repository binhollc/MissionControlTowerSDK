@echo off
setlocal enabledelayedexpansion

:: Check if a target directory argument is provided
if "%~1"=="" (
    echo Please provide the target directory as an argument.
    exit /b 1
)

:: Set variables based on input parameters
set "TARGET_DIR=%~f1"

:: Ensure the target directory ends with a backslash
if not "!TARGET_DIR:~-1!"=="\" set "TARGET_DIR=!TARGET_DIR!\"

:: Create the target directory if it does not exist
if not exist "!TARGET_DIR!" (
    mkdir "!TARGET_DIR!"
    if errorlevel 1 (
        echo Failed to create target directory: !TARGET_DIR!
        exit /b 1
    )
)

:: Iterate through each directory under the examples folder
for /d %%X in ("%~dp0examples\*") do (
    echo Staging example from: %%X
    set "RELATIVE_PATH=%%X"
    set "RELATIVE_PATH=!RELATIVE_PATH:%~dp0examples=!"
    set "SUB_TARGET_DIR=!TARGET_DIR!!RELATIVE_PATH!"
    if not exist "!SUB_TARGET_DIR!" (
        mkdir "!SUB_TARGET_DIR!"
        if errorlevel 1 (
            echo Failed to create subdirectory: !SUB_TARGET_DIR!
            exit /b 1
        )
    )
    if exist "%%X\CMakeLists.txt" (
        copy "%%X\CMakeLists.txt" "!SUB_TARGET_DIR!"
        if errorlevel 1 (
            echo Failed to copy CMakeLists.txt from %%X
            exit /b 1
        )
    ) else (
        echo Warning: CMakeLists.txt not found in %%X
    )
    if exist "%%X\main.cpp" (
        copy "%%X\main.cpp" "!SUB_TARGET_DIR!"
        if errorlevel 1 (
            echo Failed to copy main.cpp from %%X
            exit /b 1
        )
    ) else (
        if exist "%%X\sample_library.cpp" (
            copy "%%X\sample_library.cpp" "!SUB_TARGET_DIR!"
            copy "%%X\sample_library.h" "!SUB_TARGET_DIR!"
            if errorlevel 1 (
                echo Failed to copy sample_library.cpp from %%X
                exit /b 1
            )
        ) else (
            echo Warning: main.cpp not found in %%X
        )
    )
    if exist "%%X\build\Release\sample_app.exe" (
        copy "%%X\build\Release\sample_app.exe" "!SUB_TARGET_DIR!"
        if errorlevel 1 (
            echo Failed to copy sample_app.exe from %%X\build\Release
            exit /b 1
        )
    ) else (
        if exist "%%X\build\Release\sample_library.dll" (
            copy "%%X\build\Release\sample_library.dll" "!SUB_TARGET_DIR!"
            if errorlevel 1 (
                echo Failed to copy sample_app.exe or sample_library.dll from %%X\build\Release
                exit /b 1
            )
        ) else (
            echo Warning: sample_app.exe or sample_library.dll not found in %%X\build\Release
        )
    )
)

echo Staging completed successfully!

endlocal