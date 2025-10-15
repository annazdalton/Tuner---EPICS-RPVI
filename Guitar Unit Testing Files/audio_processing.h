#ifndef AUDIO_PROCESSING_H
#define AUDIO_PROCESSING_H

// Audio processing configuration
#define SAMPLE_RATE 10000      // Hz - sampling frequency
#define SAMPLE_SIZE 1024       // Number of samples for FFT
#define MIN_AMPLITUDE 50       // Minimum amplitude to consider valid signal

// Function prototypes
void audio_processing_init(void);
int audio_processing_capture(double* detected_frequency);
double apply_fft(const int16_t* samples, int num_samples);
void remove_dc_offset(int16_t* samples, int num_samples);
void apply_gain(int16_t* samples, int num_samples, float gain_factor);

#endif