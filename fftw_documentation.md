# FFTW Library - Use for Tuner Software
 
add functions we used + what they do

# DFT - Discrete Fourier Transform
fftw_plan_dft_r2c_1d() 

Inputs:
int n - Number of input samples (real values)
double *in - Pointer to input array (n real values)
fftw_complex *out - Pointer to output array
unsigned flags - Planning flags (FFTW_ESTIMATE, FFTW_MEASURE, etc.)

Output:
fftw_plan - A plan object for later execution

# FFT Execute
fft_execute()

executes plan made by fftw_plan_dft_r2c_1d()