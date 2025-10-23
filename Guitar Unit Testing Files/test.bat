@echo off
echo Compiling tests...
REM Prefer PlatformIO if available; otherwise fall back to MinGW
where platformio >nul 2>&1run_pio_tests.bat
if errorlevel 1 goto use_mingw

echo PlatformIO detected - building with PlatformIO (native environment)...
REM Change to repo root (parent of this script's directory) so platformio sees platformio.ini
pushd "%~dp0.."
platformio run -e native
set PIO_RESULT=%errorlevel%

if %PIO_RESULT% NEQ 0 (
    popd
    echo PlatformIO build failed; falling back to MinGW
    goto use_mingw
)

echo.
echo Running tests from PlatformIO build output...
echo ========================================

REM Search for any executable in .pio\build\native (prefer .exe on Windows)
set "EXE_PATH="
for %%F in (".pio\build\native\*.exe") do if not defined EXE_PATH set "EXE_PATH=%%~fF"
if not defined EXE_PATH (
    for %%F in (".pio\build\native\*") do if not defined EXE_PATH if exist "%%~fF" set "EXE_PATH=%%~fF"
)

popd

if defined EXE_PATH (
    echo Found PlatformIO executable: %EXE_PATH%
    "%EXE_PATH%"
    goto end
)

echo PlatformIO build succeeded but no executable found; falling back to MinGW

:use_mingw
echo Using MinGW gcc to build tests...
mingw32\bin\gcc src\tuner_tests.c src\string_detection.c src\audio_processing.c src\audio_sequencer.c -o tuner_tests.exe -lm
if errorlevel 1 (
    echo Build failed!
    goto end
)

echo.
echo Running tests...
echo ========================================
tuner_tests.exe

:end
pause