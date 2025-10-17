@echo off
echo Compiling tests...
mingw32\bin\gcc tuner_tests.c string_detection.c audio_processing.c audio_sequencer.c -o tuner_tests.exe -lm
if %errorlevel%==0 (
    echo.
    echo Running tests...
    echo ========================================
    tuner_tests.exe
) else (
    echo Build failed!
)
pause