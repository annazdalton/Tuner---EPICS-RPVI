# Tuner — EPICS RPVI (Guitar Tuner)

Software for Purdue Engineering Projects in Community Service — Resources for People with Visual Impairments (Tuner team).

This repository contains a small C test harness and supporting modules that simulate a guitar tuner (string detection, cents calculation, audio sequencing). The project is configured to build the test binary using PlatformIO's `native` environment for easy development on Windows.

## Quick start (Windows, cmd.exe)

Prerequisites:
- PlatformIO Core (CLI) installed and on your PATH. If you use VS Code with the PlatformIO extension you'll already have it; otherwise install it from https://platformio.org/install/cli

From the repository root run the single helper script (recommended):

cd /d "C:\Users\User\OneDrive - purdue.edu\Desktop\EPCS 41200\Tuner---EPICS-RPVI"
run_pio_tests.bat

If you want to run the tests without changing directories, use the run-from-anywhere batch wrapper from anywhere on your machine:

```cmd
run_pio_tests_from_anywhere.bat
```

PowerShell users can use the provided PowerShell wrapper (run it from the repo root):

```powershell
Set-Location 'C:\Users\User\OneDrive - purdue.edu\Desktop\EPCS 41200\Tuner---EPICS-RPVI'
.\run_pio_tests.ps1
```

This script will:
- build the `native` PlatformIO environment (`platformio run -e native`)
- execute the produced test binary at `.pio\build\native\program.exe`

If you prefer to run the steps manually:

```cmd
platformio run -e native -v
.pio\build\native\program.exe
```
---- THIS SECTION STATES HOW TO RUN IT----------------------------------------------------------------------------
Notes:
- Use `-v` (verbose) during the build if you want to see full compile/link commands.
- If you change C source files in `src/` you only need to re-run `platformio run -e native` to rebuild.

# Tuner — EPICS RPVI (Guitar Tuner)

Use these precise commands to build and run the test suite on a Windows machine that has this repository checked out.

Prerequisites:
- PlatformIO Core (CLI) must be installed and on PATH.

Canonical command to run (the one you should use):

```cmd
cd /d "C:\Users\User\OneDrive - purdue.edu\Desktop\EPCS 41200\Tuner---EPICS-RPVI"
run_pio_tests.bat
```

Notes:
- Run those two lines from cmd.exe. Replace the path above with the path where the repo is checked out on another computer if it's different.
- If PlatformIO is not found, run `platformio --version` to confirm installation, or install PlatformIO Core: https://platformio.org/install/cli
- The project also includes helper scripts (`run_pio_tests_from_anywhere.bat`, `run_pio_tests.ps1`, `run_pio_tests_ps.bat`) if you prefer running from other shells or without changing directories.

If anything fails, copy the terminal output here and I'll diagnose.
- Linker errors about multiple definitions: the repo uses a `src/` layout that provides a single definition per symbol. If you copied sources into multiple places, remove duplicates so the same function isn't compiled twice.

# Software Flowchart:
![Flowchart](img/Software%20Flowchart.drawio.png "Software Flowchart")

# Libraries Used:
main.c:
- FFTW,  https://www.fftw.org/, used for Fast Fourier Transform
- math.h, used for math

# Instruments covered:

# Senior Design Implementation (Fall 2025 - Spring 2026)
- Get user into for which string they are tuning for
- Map string to expected note
  - Low E string 
    - F, F#, G, G#, A, A#, B, C, C#, D, D#, E,
  - A string
    - A#, B, C, C#, D, D#, E, F, F#, G, G#, A
  - D string
    - D#, E, F, F#, G, G#, A, A#, B, C, C#, D
  - G string: 
    - G#, A, A#, B, C, C#, D, D#, E, F, F#, G
  - B string
    - C, C#, D, D#, E, F, F#, G, G#, A, A#, B
  - High E string
  - F, F#, G, G#, A, A#, B, C, C#, D, D#, E
- Calculate tuning directions based on cents offset
  - negative = flat = tune up 
  - positive = sharp = tune down
- Play the whole sequence
