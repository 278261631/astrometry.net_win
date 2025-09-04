@echo off
REM Windows wrapper for image2pnm Python script
REM This batch file calls the Python implementation of image2pnm

REM Try to find Python
set PYTHON_CMD=
if exist "C:\Python39\python.exe" set PYTHON_CMD=C:\Python39\python.exe
if exist "C:\Python310\python.exe" set PYTHON_CMD=C:\Python310\python.exe
if exist "C:\Python311\python.exe" set PYTHON_CMD=C:\Python311\python.exe
if exist "C:\Python312\python.exe" set PYTHON_CMD=C:\Python312\python.exe

REM Try python from PATH
if "%PYTHON_CMD%"=="" (
    python --version >nul 2>&1
    if %errorlevel%==0 set PYTHON_CMD=python
)

REM Try python3 from PATH
if "%PYTHON_CMD%"=="" (
    python3 --version >nul 2>&1
    if %errorlevel%==0 set PYTHON_CMD=python3
)

if "%PYTHON_CMD%"=="" (
    echo Error: Python not found. Please install Python or add it to PATH.
    exit /b 1
)

REM Get the directory where this batch file is located
set SCRIPT_DIR=%~dp0

REM Call the Python script
"%PYTHON_CMD%" "%SCRIPT_DIR%..\util\image2pnm.py" %*
