@echo off
REM ============================================================================
REM run_cmsis_dsp_tests_teensy.bat
REM 
REM Build and upload CMSIS-DSP tests to Teensy 4.1
REM This script compiles the code and uploads it to the Teensy board
REM ============================================================================

setlocal enabledelayedexpansion

echo.
echo ============================================================================
echo Building CMSIS-DSP Test Suite for Teensy 4.1
echo ============================================================================
echo.

REM Get to the project root directory (go up one level from CMSIS-DSP-Tests)
cd /d "%~dp0.."

REM Check if platformio is available
where platformio >nul 2>&1
if errorlevel 1 (
    echo ERROR: PlatformIO not found in PATH
    echo Please install PlatformIO CLI from https://platformio.org/install/cli
    exit /b 1
)

echo Building for Teensy 4.1...
echo.

REM Build for teensy41 environment
call platformio run -e teensy41 -v

if errorlevel 1 (
    echo.
    echo ERROR: Build failed!
    exit /b 1
)

echo.
echo ============================================================================
echo Uploading to Teensy 4.1 Board
echo ============================================================================
echo.
echo Please ensure your Teensy 4.1 is connected to your computer.
echo.
pause

REM Upload to Teensy
call platformio run -e teensy41 --target upload

if errorlevel 1 (
    echo.
    echo ERROR: Upload failed!
    echo.
    echo Make sure:
    echo   1. Teensy 4.1 is plugged in
    echo   2. Teensy Loader is not already open
    echo   3. You have the correct USB cable connection
    exit /b 1
)

echo.
echo ============================================================================
echo Upload completed successfully!
echo ============================================================================
echo.
echo Opening serial monitor to view test results...
echo To run tests, press ENTER in the serial monitor
echo.

REM Open serial monitor
call platformio device monitor -e teensy41

exit /b 0
