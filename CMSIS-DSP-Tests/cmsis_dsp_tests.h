/**
 * cmsis_dsp_tests.h - Test suite for CMSIS-DSP functions
 * 
 * This header file defines the test infrastructure for validating CMSIS-DSP
 * functions on both native (Windows) and Teensy 4.1 platforms.
 * 
 * CMSIS-DSP is the Arm Cortex Microcontroller Software Interface Standard
 * Digital Signal Processing library - provides optimized DSP functions for
 * ARM Cortex-M processors.
 */

#ifndef CMSIS_DSP_TESTS_H
#define CMSIS_DSP_TESTS_H

#include <stdint.h>
#include <float.h>
#include <math.h>
#include <stdio.h>

/* Use mock CMSIS-DSP implementation on native platform */
#include "arm_math_mock.h"

#define TEST_TOLERANCE_F32 1e-6f      /* Tolerance for float32 comparisons */
#define TEST_TOLERANCE_Q31 1          /* Tolerance for Q31 fixed-point */
#define TEST_TOLERANCE_Q15 1          /* Tolerance for Q15 fixed-point */

/* Test result structure */
typedef struct {
    uint32_t tests_run;
    uint32_t tests_passed;
    uint32_t tests_failed;
    uint32_t total_errors;
} cmsis_dsp_test_results_t;

/* Global test results */
extern cmsis_dsp_test_results_t g_test_results;

/* Assertion macros with test tracking */
#define ASSERT_FLOAT_EQ(actual, expected, tolerance) \
    do { \
        g_test_results.tests_run++; \
        float diff = fabsf((actual) - (expected)); \
        if (diff <= (tolerance)) { \
            g_test_results.tests_passed++; \
        } else { \
            g_test_results.tests_failed++; \
            printf("  FAIL: Expected %.6f, got %.6f (diff: %.6f)\n", (float)(expected), (float)(actual), diff); \
        } \
    } while(0)

#define ASSERT_INT_EQ(actual, expected) \
    do { \
        g_test_results.tests_run++; \
        if ((actual) == (expected)) { \
            g_test_results.tests_passed++; \
        } else { \
            g_test_results.tests_failed++; \
            printf("  FAIL: Expected %d, got %d\n", (int)(expected), (int)(actual)); \
        } \
    } while(0)

#define ASSERT_NOT_NULL(ptr) \
    do { \
        g_test_results.tests_run++; \
        if ((ptr) != NULL) { \
            g_test_results.tests_passed++; \
        } else { \
            g_test_results.tests_failed++; \
            printf("  FAIL: Pointer is NULL\n"); \
        } \
    } while(0)

#define TEST_SECTION_START(name) \
    do { \
        printf("\n========================================\n"); \
        printf("TEST: %s\n", (name)); \
        printf("========================================\n"); \
    } while(0)

#define TEST_CASE(name) \
    printf("  Test Case: %s\n", (name));

#define TEST_PASS(msg) \
    printf("    [PASS] %s\n", (msg))

#define TEST_FAIL(msg) \
    do { \
        printf("    [FAIL] %s\n", (msg)); \
        g_test_results.total_errors++; \
    } while(0)

/* ============================================================================
 * Test Data Constants
 * ============================================================================ */

/* Signal lengths for testing */
#define TEST_SIGNAL_LEN_64    64
#define TEST_SIGNAL_LEN_128   128
#define TEST_SIGNAL_LEN_256   256
#define TEST_SIGNAL_LEN_512   512
#define TEST_SIGNAL_LEN_1024  1024

/* FFT sizes */
#define TEST_FFT_SIZE_64      64
#define TEST_FFT_SIZE_128     128
#define TEST_FFT_SIZE_256     256
#define TEST_FFT_SIZE_512     512
#define TEST_FFT_SIZE_1024    1024
#define TEST_FFT_SIZE_2048    2048

/* Fixed-point Q-formats */
typedef int32_t q31_t;
typedef int16_t q15_t;

/* ============================================================================
 * Vector Operation Tests
 * ============================================================================ */

/**
 * Test basic vector addition
 * Tests arm_add_f32 and arm_add_q31/q15 variants
 */
void test_vector_add(void);

/**
 * Test vector subtraction
 * Tests arm_sub_f32 and arm_sub_q31/q15 variants
 */
void test_vector_subtract(void);

/**
 * Test vector multiplication (element-wise)
 * Tests arm_mult_f32 and arm_mult_q31/q15 variants
 */
void test_vector_multiply(void);

/**
 * Test dot product computation
 * Tests arm_dot_prod_f32 and arm_dot_prod_q31/q15 variants
 */
void test_dot_product(void);

/**
 * Test vector scaling
 * Tests arm_scale_f32 and arm_scale_q31/q15 variants
 */
void test_vector_scale(void);

/**
 * Test vector absolute value
 * Tests arm_abs_f32 and arm_abs_q31/q15 variants
 */
void test_vector_abs(void);

/* ============================================================================
 * FFT and Frequency Domain Tests
 * ============================================================================ */

/**
 * Test FFT computation (Real FFT)
 * Tests arm_rfft_init_f32 and arm_rfft_f32
 */
void test_rfft_f32(void);

/**
 * Test complex FFT computation
 * Tests arm_cfft_f32 initialization and computation
 */
void test_cfft_f32(void);

/**
 * Test IFFT (Inverse FFT)
 * Tests arm_rifft_f32
 */
void test_rifft_f32(void);

/**
 * Test power spectrum computation
 * Tests arm_cmplx_mag_f32
 */
void test_power_spectrum(void);

/**
 * Test magnitude computation for complex numbers
 * Tests arm_cmplx_mag_squared_f32
 */
void test_complex_magnitude(void);

/* ============================================================================
 * Filter Tests
 * ============================================================================ */

/**
 * Test FIR filter
 * Tests arm_fir_f32
 */
void test_fir_filter(void);

/**
 * Test IIR filter (Biquad)
 * Tests arm_biquad_cascade_df2T_f32
 */
void test_iir_filter(void);

/**
 * Test median filter
 * Tests arm_median_f32
 */
void test_median_filter(void);

/* ============================================================================
 * Statistical Tests
 * ============================================================================ */

/**
 * Test mean computation
 * Tests arm_mean_f32
 */
void test_mean(void);

/**
 * Test standard deviation
 * Tests arm_std_f32
 */
void test_standard_deviation(void);

/**
 * Test minimum value finding
 * Tests arm_min_f32
 */
void test_min_value(void);

/**
 * Test maximum value finding
 * Tests arm_max_f32
 */
void test_max_value(void);

/**
 * Test variance computation
 * Tests arm_var_f32
 */
void test_variance(void);

/* ============================================================================
 * Matrix Operation Tests (for advanced processing)
 * ============================================================================ */

/**
 * Test matrix multiplication
 * Tests arm_mat_mult_f32
 */
void test_matrix_multiply(void);

/**
 * Test matrix transpose
 * Tests arm_mat_trans_f32
 */
void test_matrix_transpose(void);

/**
 * Test matrix addition
 * Tests arm_mat_add_f32
 */
void test_matrix_add(void);

/* ============================================================================
 * Signal Generation Tests
 * ============================================================================ */

/**
 * Test sine wave generation
 * Tests arm_sin_f32 or equivalent
 */
void test_sine_generation(void);

/**
 * Test cosine wave generation
 * Tests arm_cos_f32 or equivalent
 */
void test_cosine_generation(void);

/**
 * Test signal copy
 * Tests arm_copy_f32
 */
void test_signal_copy(void);

/* ============================================================================
 * Windowing and Special Functions Tests
 * ============================================================================ */

/**
 * Test Hann window application
 * Tests arm_hann_f32
 */
void test_hann_window(void);

/**
 * Test Hamming window application
 * Tests arm_hamming_f32
 */
void test_hamming_window(void);

/**
 * Test modulation/demodulation
 * Tests arm_cmplx_conj_f32
 */
void test_complex_conjugate(void);

/* ============================================================================
 * Performance Benchmark Tests
 * ============================================================================ */

/**
 * Benchmark FFT performance
 * Measures execution time for FFT operations
 */
void benchmark_fft(void);

/**
 * Benchmark filtering operations
 * Measures execution time for FIR/IIR filters
 */
void benchmark_filter(void);

/**
 * Benchmark vector operations
 * Measures execution time for basic vector math
 */
void benchmark_vector_ops(void);

/* ============================================================================
 * Integration and Complete Pipeline Tests
 * ============================================================================ */

/**
 * Complete guitar tuner DSP pipeline test
 * Chain: windowing -> FFT -> magnitude -> peak finding -> frequency estimation
 */
void test_tuner_pipeline(void);

/**
 * Real-time audio processing simulation
 * Tests processing of overlapping buffers with windows
 */
void test_realtime_processing(void);

/**
 * Frequency estimation accuracy test
 * Tests how accurately we can extract tone frequencies
 */
void test_frequency_estimation(void);

/* ============================================================================
 * Test Utility Functions
 * ============================================================================ */

/**
 * Initialize test framework
 */
void cmsis_dsp_tests_init(void);

/**
 * Print current test results
 */
void print_test_results(void);

/**
 * Reset test counters
 */
void reset_test_counters(void);

/**
 * Compare two float arrays with tolerance
 * Returns 1 if all elements match within tolerance, 0 otherwise
 */
int compare_float_arrays(const float *a, const float *b, uint32_t length, float tolerance);

/**
 * Compare two Q31 arrays with tolerance
 * Returns 1 if all elements match within tolerance, 0 otherwise
 */
int compare_q31_arrays(const q31_t *a, const q31_t *b, uint32_t length, int32_t tolerance);

/**
 * Generate a sine test signal at given frequency
 * Parameters:
 *   - output: output buffer
 *   - length: number of samples
 *   - frequency: signal frequency in Hz
 *   - sampling_rate: sampling rate in Hz
 *   - amplitude: signal amplitude
 */
void generate_sine_signal(float *output, uint32_t length, float frequency, 
                         float sampling_rate, float amplitude);

/**
 * Generate a multi-tone test signal (sum of sinusoids)
 * Useful for testing FFT and frequency estimation
 */
void generate_multi_tone_signal(float *output, uint32_t length, 
                               const float *frequencies, const float *amplitudes,
                               uint32_t num_tones, float sampling_rate);

/**
 * Add Gaussian noise to a signal
 * Parameters:
 *   - signal: signal to add noise to (in-place)
 *   - length: number of samples
 *   - snr_db: signal-to-noise ratio in dB
 */
void add_gaussian_noise(float *signal, uint32_t length, float snr_db);

/**
 * Print array contents for debugging
 */
void print_float_array(const float *arr, uint32_t length, const char *name);

/**
 * Print complex array contents
 */
void print_complex_array(const float *arr, uint32_t num_complex_pairs, const char *name);

/* ============================================================================
 * Main Test Runner
 * ============================================================================ */

/**
 * Run all CMSIS-DSP tests
 * Returns 0 if all tests pass, non-zero if any fail
 */
int run_all_cmsis_dsp_tests(void);

#endif /* CMSIS_DSP_TESTS_H */
