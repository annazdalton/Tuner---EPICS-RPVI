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

void audio_processing_init(void) {
	printf("Audio processing initialized.\n");
	printf("Sample rate: %d Hz, Buffer size: %d samples\n", SAMPLE_RATE, SAMPLE_SIZE);
}

void remove_dc_offset(int16_t* samples, int num_samples) {
	long sum = 0;
	for (int i = 0; i < num_samples; i++) {
		sum += samples[i];
	}
	int16_t dc_offset = sum / num_samples;
	for (int i = 0; i < num_samples; i++) {
		samples[i] -= dc_offset;
	}
}

void apply_gain(int16_t* samples, int num_samples, float gain_factor) {
	for (int i = 0; i < num_samples; i++) {
		samples[i] = (int16_t)(samples[i] * gain_factor);
		if (samples[i] > 32767) samples[i] = 32767;
		if (samples[i] < -32768) samples[i] = -32768;
	}
}

double apply_fft(const int16_t* samples, int num_samples) {
	int max_amplitude = 0;
	for (int i = 0; i < num_samples; i++) {
		int amplitude = abs(samples[i]);
		if (amplitude > max_amplitude) {
			max_amplitude = amplitude;
		}
	}
	if (max_amplitude < MIN_AMPLITUDE) {
		return 0.0;
	}
	static int demo_counter = 0;
	double demo_frequencies[] = {82.41, 110.00, 146.83, 196.00, 246.94, 329.63};
	double simulated_freq = demo_frequencies[demo_counter % 6];
	demo_counter++;
	printf("FFT detected frequency: %.2f Hz\n", simulated_freq);
	return simulated_freq;
}

int audio_processing_capture(double* detected_frequency) {
	int16_t samples[SAMPLE_SIZE];
	for (int i = 0; i < SAMPLE_SIZE; i++) {
		samples[i] = (int16_t)(1000 * sin(2 * pi * 440.0 * i / SAMPLE_RATE));
	}
	remove_dc_offset(samples, SAMPLE_SIZE);
	apply_gain(samples, SAMPLE_SIZE, 2.0);
	double freq = apply_fft(samples, SAMPLE_SIZE);
	if (freq > 0) {
		*detected_frequency = freq;
		return 1;
	} else {
		return 0;
	}
}
