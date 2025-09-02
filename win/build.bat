@echo off
echo Building Astrometry.net for Windows...

REM Set environment variables
set CMAKE_PATH=C:\CMake\bin
set MINGW_PATH=C:\msys64\mingw64\bin
set PKG_CONFIG_PATH=C:\msys64\mingw64\lib\pkgconfig

REM Add to PATH
set PATH=%CMAKE_PATH%;%MINGW_PATH%;%PATH%

REM Create build directory
if not exist build mkdir build
cd build

REM Configure CMake
echo Configuring CMake...
cmake .. -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=Debug ^
    -DCMAKE_C_COMPILER=C:/msys64/mingw64/bin/gcc.exe ^
    -DCMAKE_CXX_COMPILER=C:/msys64/mingw64/bin/g++.exe

if %ERRORLEVEL% neq 0 (
    echo CMake configuration failed!
    exit /b 1
)

REM Build project
echo Building project...
cmake --build . --config Debug

if %ERRORLEVEL% neq 0 (
    echo Build failed!
    exit /b 1
)

echo Build completed successfully!
echo Executable location: %cd%\bin\solve-field.exe

REM Check for additional DLL dependencies
echo Checking for additional DLL dependencies...
if exist "C:\msys64\mingw64\bin\libgsl-25.dll" (
    copy "C:\msys64\mingw64\bin\libgsl-25.dll" bin\
    echo Copied libgsl-25.dll
)

if exist "C:\msys64\mingw64\bin\libgslcblas-0.dll" (
    copy "C:\msys64\mingw64\bin\libgslcblas-0.dll" bin\
    echo Copied libgslcblas-0.dll
)

if exist "C:\msys64\mingw64\bin\zlib1.dll" (
    copy "C:\msys64\mingw64\bin\zlib1.dll" bin\
    echo Copied zlib1.dll
)

echo.
echo Build process completed!
echo You can find the executable at: %cd%\bin\solve-field.exe
