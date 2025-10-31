@echo off
REM ============================================================================
REM run_cmsis_dsp_tests_native.bat
REM 
REM Test runner for CMSIS-DSP on native (Windows) platform
REM Builds and runs the comprehensive CMSIS-DSP test suite
REM ============================================================================

setlocal enabledelayedexpansion

echo.
echo ============================================================================
echo Building CMSIS-DSP Test Suite for Native Platform
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

echo Building for native platform (Windows)...
echo.

REM Build for native environment
call platformio run -e native -v

if errorlevel 1 (
    echo.
    echo ERROR: Build failed!
    exit /b 1
)

echo.
echo ============================================================================
echo Running CMSIS-DSP Tests
echo ============================================================================
echo.

REM Run the test executable
call .pio\build\native\program.exe

if errorlevel 1 (
    echo.
    echo Some tests failed!
    exit /b 1
)

echo.
echo ============================================================================
echo Test execution completed successfully!
echo ============================================================================
echo.

exit /b 0
