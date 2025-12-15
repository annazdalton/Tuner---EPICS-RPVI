#ifndef AUDIO_PROCESSING_H
#define AUDIO_PROCESSING_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * audio_processing.h - Real-time FFT-based frequency detection
 * 
 * Uses ARM CMSIS-DSP library for optimized FFT computation
 * on Cortex-M4/M7 processors and native platforms.
 */

/* Audio processing configuration */
#define SAMPLE_RATE 10000      /* Hz - sampling frequency */
#define SAMPLE_SIZE 1024       /* Number of samples to process */
#define MIN_AMPLITUDE 50       /* Minimum amplitude to consider valid signal */

/* Function prototypes */

/**
 * Initialize audio processing subsystem
 * Must be called once before using other functions
 */
void audio_processing_init(void);

/**
 * Capture audio and detect fundamental frequency using real FFT
 * 
 * @param detected_frequency: Output parameter for detected frequency in Hz
 * @return: 1 if valid frequency detected, 0 if no valid signal
 */
int audio_processing_capture(double* detected_frequency);

/**
 * Compute real FFT and find peak frequency
 * 
 * @param samples: Array of audio samples (int16_t PCM data)
 * @param num_samples: Number of samples in array
 * @return: Peak frequency in Hz (0.0 if no valid signal)
 */
double apply_fft(const int16_t* samples, int num_samples);

/**
 * Remove DC offset from audio samples
 * DC bias can skew FFT results, so this preprocessing step is important
 * 
 * @param samples: Array of audio samples (modified in-place)
 * @param num_samples: Number of samples
 */
void remove_dc_offset(int16_t* samples, int num_samples);

/**
 * Apply gain with saturation to prevent clipping
 * Useful for normalizing weak signals
 * 
 * @param samples: Array of audio samples (modified in-place)
 * @param num_samples: Number of samples
 * @param gain_factor: Multiplication factor (e.g., 2.0 for 6dB gain)
 */
void apply_gain(int16_t* samples, int num_samples, float gain_factor);

#ifdef __cplusplus
}
#endif

#endif