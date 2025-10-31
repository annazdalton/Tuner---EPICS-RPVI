/**
 * cmsis_dsp_test_utilities.c - Helper functions for CMSIS-DSP testing
 * 
 * Provides test data generation, signal processing utilities, and
 * comparison functions for validating DSP operations.
 */

#include "cmsis_dsp_tests.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Global test results counter */
cmsis_dsp_test_results_t g_test_results = {0, 0, 0, 0};

/* ============================================================================
 * Array Comparison Functions
 * ============================================================================ */

int compare_float_arrays(const float *a, const float *b, uint32_t length, float tolerance)
{
    if (a == NULL || b == NULL) {
        return 0;
    }
    
    for (uint32_t i = 0; i < length; i++) {
        float diff = fabsf(a[i] - b[i]);
        if (diff > tolerance) {
            printf("    Array mismatch at index %u: %.6f vs %.6f (diff: %.6f)\n", 
                   i, a[i], b[i], diff);
            return 0;
        }
    }
    return 1;
}

int compare_q31_arrays(const q31_t *a, const q31_t *b, uint32_t length, int32_t tolerance)
{
    if (a == NULL || b == NULL) {
        return 0;
    }
    
    for (uint32_t i = 0; i < length; i++) {
        int32_t diff = (int32_t)labs((long)(a[i] - b[i]));
        if (diff > tolerance) {
            printf("    Array mismatch at index %u: %ld vs %ld (diff: %ld)\n", 
                   i, (long)a[i], (long)b[i], (long)diff);
            return 0;
        }
    }
    return 1;
}

/* ============================================================================
 * Signal Generation Functions
 * ============================================================================ */

void generate_sine_signal(float *output, uint32_t length, float frequency, 
                         float sampling_rate, float amplitude)
{
    if (output == NULL || length == 0 || sampling_rate <= 0) {
        return;
    }
    
    float phase_increment = 2.0f * 3.14159265359f * frequency / sampling_rate;
    float phase = 0.0f;
    
    for (uint32_t i = 0; i < length; i++) {
        output[i] = amplitude * sinf(phase);
        phase += phase_increment;
        
        /* Wrap phase to prevent precision loss */
        if (phase > 2.0f * 3.14159265359f) {
            phase -= 2.0f * 3.14159265359f;
        }
    }
}

void generate_multi_tone_signal(float *output, uint32_t length, 
                               const float *frequencies, const float *amplitudes,
                               uint32_t num_tones, float sampling_rate)
{
    if (output == NULL || length == 0 || frequencies == NULL || amplitudes == NULL) {
        return;
    }
    
    /* Initialize output to zero */
    memset(output, 0, length * sizeof(float));
    
    /* Generate each tone and add to output */
    float *temp_tone = (float *)malloc(length * sizeof(float));
    if (temp_tone == NULL) {
        return;
    }
    
    for (uint32_t tone_idx = 0; tone_idx < num_tones; tone_idx++) {
        generate_sine_signal(temp_tone, length, frequencies[tone_idx], 
                           sampling_rate, amplitudes[tone_idx]);
        
        /* Add to output */
        for (uint32_t i = 0; i < length; i++) {
            output[i] += temp_tone[i];
        }
    }
    
    free(temp_tone);
}

void add_gaussian_noise(float *signal, uint32_t length, float snr_db)
{
    if (signal == NULL || length == 0) {
        return;
    }
    
    /* Calculate signal power */
    float signal_power = 0.0f;
    for (uint32_t i = 0; i < length; i++) {
        signal_power += signal[i] * signal[i];
    }
    signal_power /= (float)length;
    
    /* Calculate noise standard deviation from SNR */
    float snr_linear = powf(10.0f, snr_db / 20.0f);
    float noise_std = sqrtf(signal_power / snr_linear);
    
    /* Add white Gaussian noise using Box-Muller transform */
    for (uint32_t i = 0; i < length - 1; i += 2) {
        float u1 = (float)rand() / RAND_MAX;
        float u2 = (float)rand() / RAND_MAX;
        
        /* Avoid log(0) */
        if (u1 < 1e-10f) u1 = 1e-10f;
        
        float z0 = sqrtf(-2.0f * logf(u1)) * cosf(2.0f * 3.14159265359f * u2);
        float z1 = sqrtf(-2.0f * logf(u1)) * sinf(2.0f * 3.14159265359f * u2);
        
        signal[i] += noise_std * z0;
        if (i + 1 < length) {
            signal[i + 1] += noise_std * z1;
        }
    }
}

/* ============================================================================
 * Printing Functions
 * ============================================================================ */

void print_float_array(const float *arr, uint32_t length, const char *name)
{
    if (arr == NULL || length == 0) {
        printf("%s: [NULL or empty]\n", name ? name : "Array");
        return;
    }
    
    printf("%s (length=%u):\n", name ? name : "Array", length);
    printf("  [");
    
    for (uint32_t i = 0; i < length && i < 20; i++) {
        printf("%.6f", arr[i]);
        if (i < length - 1 && i < 19) {
            printf(", ");
        }
    }
    
    if (length > 20) {
        printf(", ... (%u more elements)", length - 20);
    }
    printf("]\n");
}

void print_complex_array(const float *arr, uint32_t num_complex_pairs, const char *name)
{
    if (arr == NULL || num_complex_pairs == 0) {
        printf("%s: [NULL or empty]\n", name ? name : "Complex Array");
        return;
    }
    
    printf("%s (length=%u complex pairs = %u floats):\n", 
           name ? name : "Complex Array", num_complex_pairs, num_complex_pairs * 2);
    printf("  [");
    
    for (uint32_t i = 0; i < num_complex_pairs && i < 10; i++) {
        printf("%.4f+%.4fj", arr[2*i], arr[2*i+1]);
        if (i < num_complex_pairs - 1 && i < 9) {
            printf(", ");
        }
    }
    
    if (num_complex_pairs > 10) {
        printf(", ... (%u more pairs)", num_complex_pairs - 10);
    }
    printf("]\n");
}

/* ============================================================================
 * Test Framework Functions
 * ============================================================================ */

void cmsis_dsp_tests_init(void)
{
    reset_test_counters();
    printf("\n");
    printf("====================================================\n");
    printf("CMSIS-DSP Test Suite Initialization\n");
    printf("====================================================\n");
}

void reset_test_counters(void)
{
    g_test_results.tests_run = 0;
    g_test_results.tests_passed = 0;
    g_test_results.tests_failed = 0;
    g_test_results.total_errors = 0;
}

void print_test_results(void)
{
    printf("\n");
    printf("====================================================\n");
    printf("TEST RESULTS SUMMARY\n");
    printf("====================================================\n");
    printf("Total Assertions Run:    %u\n", g_test_results.tests_run);
    printf("Assertions Passed:       %u\n", g_test_results.tests_passed);
    printf("Assertions Failed:       %u\n", g_test_results.tests_failed);
    printf("Total Errors Detected:   %u\n", g_test_results.total_errors);
    
    if (g_test_results.tests_run > 0) {
        float pass_rate = (float)g_test_results.tests_passed / (float)g_test_results.tests_run * 100.0f;
        printf("Pass Rate:               %.1f%%\n", pass_rate);
    }
    
    printf("====================================================\n");
    
    if (g_test_results.tests_failed == 0 && g_test_results.total_errors == 0) {
        printf("STATUS: ALL TESTS PASSED ✓\n");
    } else {
        printf("STATUS: SOME TESTS FAILED ✗\n");
    }
    printf("====================================================\n");
}
