# Tuner---EPICS-RPVI

Software for Purdue Engineering Projects in Community Serivce - Resources for People with Visual Impairments - Tuner Team

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
