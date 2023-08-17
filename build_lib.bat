set PROJECT_DIR=%CD%
set BUILD_DIR=build

rem Configure CMake

cd %PROJECT_DIR%

if not exist "%BUILD_DIR%\" (
    mkdir %BUILD_DIR%
)
cd %BUILD_DIR%
cmake .. -G "Visual Studio 17 2022" -A x64

rem Build

cd %PROJECT_DIR%
cd %BUILD_DIR%
cmake --build . --config Release
