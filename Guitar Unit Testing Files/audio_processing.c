/**
 * audio_processing.c - Audio processing implementation
 * 
 * Simulates FFT processing for frequency detection
 * In actual implementation, this would use the arduinoFFT library
 */

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include "audio_processing.h"
#include <stdlib.h>  // Add this line for abs() function

float pi = 3.14159265358979323846;
/**
 * Initialize audio processing subsystem
 */
void audio_processing_init(void) {
    printf("Audio processing initialized.\n");
    printf("Sample rate: %d Hz, Buffer size: %d samples\n", SAMPLE_RATE, SAMPLE_SIZE);
}

/**
 * Remove DC offset from audio samples
 * Calculates average and subtracts from all samples
 */
void remove_dc_offset(int16_t* samples, int num_samples) {
    long sum = 0;
    
    // Calculate DC offset (average value)
    for (int i = 0; i < num_samples; i++) {
        sum += samples[i];
    }
    int16_t dc_offset = sum / num_samples;
    
    // Remove DC offset from all samples
    for (int i = 0; i < num_samples; i++) {
        samples[i] -= dc_offset;
    }
}

/**
 * Apply gain to amplify the signal
 */
void apply_gain(int16_t* samples, int num_samples, float gain_factor) {
    for (int i = 0; i < num_samples; i++) {
        samples[i] = (int16_t)(samples[i] * gain_factor);
        
        // Clamp to prevent overflow
        if (samples[i] > 32767) samples[i] = 32767;
        if (samples[i] < -32768) samples[i] = -32768;
    }
}

/**
 * Simulated FFT processing - in real implementation, use arduinoFFT
 * This is a simplified version that returns a simulated frequency
 */
double apply_fft(const int16_t* samples, int num_samples) {
    // Test frequency for noe
    
    // Simulate detecting different frequencies based on sample characteristics
    double simulated_freq = 0.0;
    
    // Calculate some simple metrics from samples
    int max_amplitude = 0;
    for (int i = 0; i < num_samples; i++) {
        int amplitude = abs(samples[i]);
        if (amplitude > max_amplitude) {
            max_amplitude = amplitude;
        }
    }
    
    // If signal is too weak, return 0
    if (max_amplitude < MIN_AMPLITUDE) {
        return 0.0;
    }
    
    // For demonstration, cycle through different guitar string frequencies
    // In real implementation, this would be the actual FFT peak detection
    static int demo_counter = 0;
    double demo_frequencies[] = {82.41, 110.00, 146.83, 196.00, 246.94, 329.63};
    simulated_freq = demo_frequencies[demo_counter % 6];
    demo_counter++;
    
    printf("FFT detected frequency: %.2f Hz\n", simulated_freq);
    return simulated_freq;
}

/**
 * Capture and process audio to detect frequency
 * 
 * @param detected_frequency: Output parameter for detected frequency
 * @return: 1 if successful, 0 if no valid frequency detected
 */
int audio_processing_capture(double* detected_frequency) {
    // In actual implementation, this would:
    // 1. Read samples from microphone (A2 pin)
    // 2. Remove DC offset
    // 3. Apply gain
    // 4. Perform FFT
    // 5. Find dominant frequency
    
    // For simulation, we'll create dummy samples
    int16_t samples[SAMPLE_SIZE];
    
    // Generate simulated samples (in real code, read from ADC)
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        // Simulate some audio input
        samples[i] = (int16_t)(1000 * sin(2 * pi * 440.0 * i / SAMPLE_RATE)); // A4 note
    }
    
    // Process samples
    remove_dc_offset(samples, SAMPLE_SIZE);
    apply_gain(samples, SAMPLE_SIZE, 2.0); // 2x gain
    
    // Apply FFT to find frequency
    double freq = apply_fft(samples, SAMPLE_SIZE);
    
    if (freq > 0) {
        *detected_frequency = freq;
        return 1; // Success
    } else {
        return 0; // No valid frequency detected
    }
}