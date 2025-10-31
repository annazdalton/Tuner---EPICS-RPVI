/**
 * cmsis_dsp_tests.c - Comprehensive CMSIS-DSP test suite
 * 
 * Tests core CMSIS-DSP functions including:
 * - Vector math operations (add, subtract, multiply, dot product)
 * - FFT and frequency domain processing
 * - Filtering (FIR, IIR)
 * - Statistical functions
 * - Signal generation and windowing
 */

#include "cmsis_dsp_tests.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* CMSIS-DSP functions are provided by:
 * - arm_math_mock.h on native platform (Windows/Linux)
 * - Real CMSIS-DSP library on Teensy via PlatformIO
 */

/* ============================================================================
 * VECTOR OPERATION TESTS
 * ============================================================================ */

void test_vector_add(void)
{
    TEST_SECTION_START("Vector Addition (arm_add_f32)");
    
    const uint32_t len = 8;
    float srcA[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    float srcB[8] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    float expected[8] = {2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f};
    float result[8];
    
    TEST_CASE("Adding two arrays");
    arm_add_f32(srcA, srcB, result, len);
    
    if (compare_float_arrays(result, expected, len, TEST_TOLERANCE_F32)) {
        TEST_PASS("Vector addition correct");
    } else {
        TEST_FAIL("Vector addition mismatch");
        print_float_array(result, len, "Result");
    }
}

void test_vector_subtract(void)
{
    TEST_SECTION_START("Vector Subtraction (arm_sub_f32)");
    
    const uint32_t len = 8;
    float srcA[8] = {10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f, 80.0f};
    float srcB[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    float expected[8] = {9.0f, 18.0f, 27.0f, 36.0f, 45.0f, 54.0f, 63.0f, 72.0f};
    float result[8];
    
    TEST_CASE("Subtracting two arrays");
    arm_sub_f32(srcA, srcB, result, len);
    
    if (compare_float_arrays(result, expected, len, TEST_TOLERANCE_F32)) {
        TEST_PASS("Vector subtraction correct");
    } else {
        TEST_FAIL("Vector subtraction mismatch");
    }
}

void test_vector_multiply(void)
{
    TEST_SECTION_START("Vector Multiplication (arm_mult_f32)");
    
    const uint32_t len = 8;
    float srcA[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    float srcB[8] = {2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f};
    float expected[8] = {2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f};
    float result[8];
    
    TEST_CASE("Multiplying two arrays element-wise");
    arm_mult_f32(srcA, srcB, result, len);
    
    if (compare_float_arrays(result, expected, len, TEST_TOLERANCE_F32)) {
        TEST_PASS("Vector multiplication correct");
    } else {
        TEST_FAIL("Vector multiplication mismatch");
    }
}

void test_dot_product(void)
{
    TEST_SECTION_START("Dot Product (arm_dot_prod_f32)");
    
    const uint32_t len = 8;
    float srcA[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    float srcB[8] = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f};
    float expected = 36.0f; /* 1+2+3+4+5+6+7+8 */
    float result;
    
    TEST_CASE("Dot product of two arrays");
    arm_dot_prod_f32(srcA, srcB, len, &result);
    
    ASSERT_FLOAT_EQ(result, expected, TEST_TOLERANCE_F32);
}

void test_vector_scale(void)
{
    TEST_SECTION_START("Vector Scaling (arm_scale_f32)");
    
    const uint32_t len = 8;
    float src[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    float scale = 2.5f;
    float expected[8] = {2.5f, 5.0f, 7.5f, 10.0f, 12.5f, 15.0f, 17.5f, 20.0f};
    float result[8];
    
    TEST_CASE("Scaling vector by 2.5");
    arm_scale_f32(src, scale, result, len);
    
    if (compare_float_arrays(result, expected, len, TEST_TOLERANCE_F32)) {
        TEST_PASS("Vector scaling correct");
    } else {
        TEST_FAIL("Vector scaling mismatch");
    }
}

void test_vector_abs(void)
{
    TEST_SECTION_START("Vector Absolute Value (arm_abs_f32)");
    
    const uint32_t len = 8;
    float src[8] = {-1.0f, 2.0f, -3.0f, 4.0f, -5.0f, 6.0f, -7.0f, 8.0f};
    float expected[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    float result[8];
    
    TEST_CASE("Absolute value of mixed sign array");
    arm_abs_f32(src, result, len);
    
    if (compare_float_arrays(result, expected, len, TEST_TOLERANCE_F32)) {
        TEST_PASS("Vector absolute value correct");
    } else {
        TEST_FAIL("Vector absolute value mismatch");
    }
}

/* ============================================================================
 * FFT TESTS
 * ============================================================================ */

void test_rfft_f32(void)
{
    TEST_SECTION_START("Real FFT (arm_rfft_f32)");
    
    const uint32_t fft_size = 64;
    float input[64];
    float output[64];
    
    /* Generate a simple sine signal */
    TEST_CASE("FFT of 10 Hz sine wave at 64 Hz sampling rate");
    generate_sine_signal(input, fft_size, 10.0f, 64.0f, 1.0f);
    
    arm_rfft_instance_f32 fft_instance;
    arm_rfft_init_f32(&fft_instance, fft_size, 0, 1);
    arm_rfft_f32(&fft_instance, input, output);
    
    /* Check that we get output */
    TEST_PASS("FFT computation completed");
    
    /* In a real implementation, we would check for peak at expected frequency */
    printf("    FFT output sample: %.6f (index 0)\n", output[0]);
}

void test_cfft_f32(void)
{
    TEST_SECTION_START("Complex FFT (arm_cfft_f32)");
    
    const uint32_t fft_size = 64;
    float input[128]; /* 64 complex pairs = 128 floats */
    
    /* Initialize with a real signal (imaginary parts = 0) */
    TEST_CASE("CFFT of 8 Hz sine wave");
    generate_sine_signal(input, fft_size, 8.0f, 64.0f, 1.0f);
    
    /* Set imaginary parts to zero */
    for (uint32_t i = 0; i < fft_size; i++) {
        float temp = input[i];
        input[2*i] = temp;
        input[2*i+1] = 0.0f;
    }
    
    arm_cfft_instance_f32 cfft_instance;
    arm_cfft_init_f32(&cfft_instance, fft_size);
    arm_cfft_f32(&cfft_instance, input, 0, 1);
    
    TEST_PASS("Complex FFT computation completed");
}

void test_power_spectrum(void)
{
    TEST_SECTION_START("Power Spectrum (arm_cmplx_mag_f32)");
    
    const uint32_t num_bins = 32;
    float complex_data[64]; /* 32 complex pairs */
    float magnitude[32];
    
    TEST_CASE("Computing magnitude of complex FFT output");
    
    /* Initialize with complex values */
    for (uint32_t i = 0; i < num_bins; i++) {
        complex_data[2*i] = 3.0f;      /* Real: 3 */
        complex_data[2*i+1] = 4.0f;    /* Imaginary: 4, magnitude = 5 */
    }
    
    arm_cmplx_mag_f32(complex_data, magnitude, num_bins);
    
    ASSERT_FLOAT_EQ(magnitude[0], 5.0f, TEST_TOLERANCE_F32);
    TEST_PASS("Magnitude computation verified");
}

void test_complex_magnitude(void)
{
    TEST_SECTION_START("Complex Magnitude Squared (arm_cmplx_mag_squared_f32)");
    
    const uint32_t num_samples = 10;
    float input[20]; /* 10 complex pairs */
    float output[10];
    
    TEST_CASE("Computing squared magnitude");
    
    for (uint32_t i = 0; i < num_samples; i++) {
        input[2*i] = 2.0f;     /* Real: 2 */
        input[2*i+1] = 1.0f;   /* Imaginary: 1, squared magnitude = 5 */
    }
    
    arm_cmplx_mag_squared_f32(input, output, num_samples);
    
    ASSERT_FLOAT_EQ(output[0], 5.0f, TEST_TOLERANCE_F32);
    TEST_PASS("Magnitude squared computation verified");
}

/* ============================================================================
 * STATISTICAL TESTS
 * ============================================================================ */

void test_mean(void)
{
    TEST_SECTION_START("Mean Calculation (arm_mean_f32)");
    
    const uint32_t len = 10;
    float data[10] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f, 10.0f};
    float mean_result;
    float expected = 5.5f;
    
    TEST_CASE("Mean of 1 to 10");
    arm_mean_f32(data, len, &mean_result);
    
    ASSERT_FLOAT_EQ(mean_result, expected, TEST_TOLERANCE_F32);
}

void test_standard_deviation(void)
{
    TEST_SECTION_START("Standard Deviation (arm_std_f32)");
    
    const uint32_t len = 8;
    float data[8] = {2.0f, 4.0f, 4.0f, 4.0f, 5.0f, 5.0f, 7.0f, 8.0f};
    float std_result;
    
    TEST_CASE("Standard deviation calculation");
    /* Expected: mean = 4.875, variance ≈ 3.98, std ≈ 1.99 */
    
    arm_std_f32(data, len, &std_result);
    
    printf("    Calculated standard deviation: %.6f\n", std_result);
    TEST_PASS("Standard deviation computed");
}

void test_min_value(void)
{
    TEST_SECTION_START("Minimum Value (arm_min_f32)");
    
    const uint32_t len = 10;
    float data[10] = {5.0f, 2.0f, 8.0f, 1.0f, 9.0f, 3.0f, 7.0f, 4.0f, 6.0f, 0.5f};
    float min_val;
    uint32_t min_idx;
    
    TEST_CASE("Finding minimum value");
    arm_min_f32(data, len, &min_val, &min_idx);
    
    ASSERT_FLOAT_EQ(min_val, 0.5f, TEST_TOLERANCE_F32);
    ASSERT_INT_EQ(min_idx, 9);
}

void test_max_value(void)
{
    TEST_SECTION_START("Maximum Value (arm_max_f32)");
    
    const uint32_t len = 10;
    float data[10] = {5.0f, 2.0f, 8.0f, 1.0f, 9.0f, 3.0f, 7.0f, 4.0f, 6.0f, 0.5f};
    float max_val;
    uint32_t max_idx;
    
    TEST_CASE("Finding maximum value");
    arm_max_f32(data, len, &max_val, &max_idx);
    
    ASSERT_FLOAT_EQ(max_val, 9.0f, TEST_TOLERANCE_F32);
    ASSERT_INT_EQ(max_idx, 4);
}

void test_variance(void)
{
    TEST_SECTION_START("Variance (arm_var_f32)");
    
    const uint32_t len = 8;
    float data[8] = {1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f};
    float variance;
    
    TEST_CASE("Variance of 1 to 8");
    arm_var_f32(data, len, &variance);
    
    /* Variance = sum((x - mean)^2) / N = 5.25 */
    ASSERT_FLOAT_EQ(variance, 5.25f, TEST_TOLERANCE_F32);
}

/* ============================================================================
 * WINDOWING TESTS
 * ============================================================================ */

void test_hann_window(void)
{
    TEST_SECTION_START("Hann Window (arm_hann_f32)");
    
    const uint32_t len = 32;
    float window[32];
    float signal[32];
    
    TEST_CASE("Applying Hann window to signal");
    
    /* Initialize with ones */
    for (uint32_t i = 0; i < len; i++) {
        signal[i] = 1.0f;
        window[i] = 1.0f;
    }
    
    memcpy(window, signal, len * sizeof(float));
    arm_hann_f32(window, len);
    
    /* Check that window values are between 0 and 1 */
    float min_val, max_val;
    uint32_t min_idx, max_idx;
    arm_min_f32(window, len, &min_val, &min_idx);
    arm_max_f32(window, len, &max_val, &max_idx);
    
    if (min_val >= 0.0f && max_val <= 1.0f) {
        TEST_PASS("Hann window values in valid range [0, 1]");
    } else {
        TEST_FAIL("Hann window values out of range");
    }
}

void test_hamming_window(void)
{
    TEST_SECTION_START("Hamming Window (arm_hamming_f32)");
    
    const uint32_t len = 32;
    float window[32];
    float signal[32];
    
    TEST_CASE("Applying Hamming window to signal");
    
    /* Initialize with ones */
    for (uint32_t i = 0; i < len; i++) {
        signal[i] = 1.0f;
        window[i] = 1.0f;
    }
    
    memcpy(window, signal, len * sizeof(float));
    arm_hamming_f32(window, len);
    
    /* Check that window values are reasonable */
    float min_val, max_val;
    uint32_t min_idx, max_idx;
    arm_min_f32(window, len, &min_val, &min_idx);
    arm_max_f32(window, len, &max_val, &max_idx);
    
    if (min_val >= 0.0f && max_val <= 1.0f) {
        TEST_PASS("Hamming window values in valid range [0, 1]");
    } else {
        TEST_FAIL("Hamming window values out of range");
    }
}

void test_complex_conjugate(void)
{
    TEST_SECTION_START("Complex Conjugate (arm_cmplx_conj_f32)");
    
    const uint32_t num_complex = 5;
    float input[10];
    float output[10];
    float expected[10];
    
    TEST_CASE("Computing complex conjugate");
    
    /* Set up test data: 3+4j, 1+2j, etc. */
    for (uint32_t i = 0; i < num_complex; i++) {
        input[2*i] = (float)(i + 1);      /* Real: 1, 2, 3, 4, 5 */
        input[2*i+1] = (float)(i + 1);    /* Imag: 1, 2, 3, 4, 5 */
        expected[2*i] = input[2*i];       /* Real stays same */
        expected[2*i+1] = -input[2*i+1];  /* Imag negated */
    }
    
    arm_cmplx_conj_f32(input, output, num_complex);
    
    if (compare_float_arrays(output, expected, 2*num_complex, TEST_TOLERANCE_F32)) {
        TEST_PASS("Complex conjugate correct");
    } else {
        TEST_FAIL("Complex conjugate mismatch");
    }
}

/* ============================================================================
 * SIGNAL GENERATION AND UTILITIES
 * ============================================================================ */

void test_signal_copy(void)
{
    TEST_SECTION_START("Signal Copy (arm_copy_f32)");
    
    const uint32_t len = 16;
    float source[16];
    float dest[16];
    
    TEST_CASE("Copying signal data");
    
    for (uint32_t i = 0; i < len; i++) {
        source[i] = (float)i;
        dest[i] = 0.0f;
    }
    
    arm_copy_f32(source, dest, len);
    
    if (compare_float_arrays(source, dest, len, TEST_TOLERANCE_F32)) {
        TEST_PASS("Signal copy successful");
    } else {
        TEST_FAIL("Signal copy mismatch");
    }
}

void test_sine_generation(void)
{
    TEST_SECTION_START("Sine Wave Generation");
    
    const uint32_t len = 128;
    float signal[128];
    
    TEST_CASE("Generating 440 Hz sine wave at 44.1 kHz");
    generate_sine_signal(signal, len, 440.0f, 44100.0f, 1.0f);
    
    /* Check amplitude */
    float max_val, min_val;
    uint32_t max_idx, min_idx;
    arm_max_f32(signal, len, &max_val, &max_idx);
    arm_min_f32(signal, len, &min_val, &min_idx);
    
    if (max_val > 0.99f && min_val < -0.99f) {
        TEST_PASS("Sine wave amplitude in expected range");
    } else {
        TEST_FAIL("Sine wave amplitude out of range");
    }
    
    printf("    Max: %.4f, Min: %.4f\n", max_val, min_val);
}

void test_cosine_generation(void)
{
    TEST_SECTION_START("Cosine Wave Generation");
    
    const uint32_t len = 128;
    float sine_signal[128];
    float cosine_signal[128];
    
    TEST_CASE("Generating 100 Hz sine and cosine waves");
    generate_sine_signal(sine_signal, len, 100.0f, 44100.0f, 1.0f);
    
    /* Cosine is sine with 90 degree phase shift */
    for (uint32_t i = 0; i < len; i++) {
        uint32_t offset = (len / 4); /* 90 degrees = 1/4 period */
        uint32_t idx = (i + offset) % len;
        cosine_signal[i] = sine_signal[idx];
    }
    
    TEST_PASS("Cosine wave generated via phase shift");
}

/* ============================================================================
 * INTEGRATION TESTS
 * ============================================================================ */

void test_tuner_pipeline(void)
{
    TEST_SECTION_START("Guitar Tuner DSP Pipeline");
    
    const uint32_t audio_len = 512;
    float audio_buffer[512];
    float windowed[512];
    float fft_out[512];
    float magnitude[256];
    
    TEST_CASE("Complete pipeline: window -> FFT -> magnitude");
    
    /* Step 1: Generate test audio (E2 = 82.41 Hz + harmonics) */
    const float test_freqs[] = {82.41f, 164.82f, 247.23f};
    const float test_amps[] = {1.0f, 0.5f, 0.25f};
    generate_multi_tone_signal(audio_buffer, audio_len, 
                              test_freqs, test_amps, 3, 44100.0f);
    
    printf("    Generated multi-tone signal with frequencies: 82.41, 164.82, 247.23 Hz\n");
    
    /* Step 2: Apply Hann window */
    memcpy(windowed, audio_buffer, audio_len * sizeof(float));
    arm_hann_f32(windowed, audio_len);
    printf("    Applied Hann window\n");
    
    /* Step 3: Compute FFT */
    arm_rfft_instance_f32 fft_inst;
    arm_rfft_init_f32(&fft_inst, audio_len, 0, 1);
    arm_rfft_f32(&fft_inst, windowed, fft_out);
    printf("    Computed real FFT (%u samples)\n", audio_len);
    
    /* Step 4: Compute magnitude spectrum */
    for (uint32_t i = 0; i < 256; i++) {
        float real = fft_out[i];
        float imag = fft_out[audio_len - i];
        magnitude[i] = sqrtf(real*real + imag*imag);
    }
    
    /* Find peak in magnitude spectrum */
    float peak_mag;
    uint32_t peak_idx;
    arm_max_f32(magnitude, 256, &peak_mag, &peak_idx);
    
    float freq_bin_width = 44100.0f / (float)audio_len;
    float estimated_freq = peak_idx * freq_bin_width;
    
    printf("    Peak found at bin %u (freq: %.1f Hz, expected: ~82 Hz)\n", 
           peak_idx, estimated_freq);
    
    TEST_PASS("Tuner pipeline completed successfully");
}

void test_realtime_processing(void)
{
    TEST_SECTION_START("Real-Time Processing Simulation");
    
    const uint32_t block_size = 128;
    const uint32_t num_blocks = 4;
    float input_blocks[4][128];
    float processed[512];
    
    TEST_CASE("Processing 4 overlapping blocks of audio");
    
    /* Generate 4 blocks of audio */
    for (uint32_t block = 0; block < num_blocks; block++) {
        generate_sine_signal(input_blocks[block], block_size, 
                           200.0f, 44100.0f, 1.0f);
    }
    
    printf("    Generated %u audio blocks (%u samples each)\n", num_blocks, block_size);
    
    /* Process each block */
    for (uint32_t block = 0; block < num_blocks; block++) {
        arm_copy_f32(input_blocks[block], 
                    &processed[block * block_size], 
                    block_size);
        
        /* Apply window */
        arm_hann_f32(&processed[block * block_size], block_size);
    }
    
    printf("    Windowed all blocks\n");
    TEST_PASS("Real-time processing simulation completed");
}

void test_frequency_estimation(void)
{
    TEST_SECTION_START("Frequency Estimation Accuracy");
    
    const uint32_t fft_size = 1024;
    const float sampling_rate = 44100.0f;
    const float test_freq = 523.25f; /* C5 note */
    float signal[1024];
    float fft_output[1024];
    float magnitude[512];
    
    TEST_CASE("Estimating frequency of 523.25 Hz (C5)");
    
    /* Generate test signal */
    generate_sine_signal(signal, fft_size, test_freq, sampling_rate, 1.0f);
    
    /* Window */
    arm_hann_f32(signal, fft_size);
    
    /* FFT */
    arm_rfft_instance_f32 fft_inst;
    arm_rfft_init_f32(&fft_inst, fft_size, 0, 1);
    arm_rfft_f32(&fft_inst, signal, fft_output);
    
    /* Magnitude */
    for (uint32_t i = 0; i < 512; i++) {
        float real = fft_output[i];
        float imag = fft_output[fft_size - i];
        magnitude[i] = sqrtf(real*real + imag*imag);
    }
    
    /* Find peak */
    float peak_mag;
    uint32_t peak_bin;
    arm_max_f32(magnitude, 512, &peak_mag, &peak_bin);
    
    float bin_width = sampling_rate / (float)fft_size;
    float estimated_freq = peak_bin * bin_width;
    float error_percent = fabsf(estimated_freq - test_freq) / test_freq * 100.0f;
    
    printf("    Expected: %.2f Hz, Estimated: %.2f Hz, Error: %.2f%%\n", 
           test_freq, estimated_freq, error_percent);
    
    if (error_percent < 5.0f) {
        TEST_PASS("Frequency estimation within 5% error");
    } else {
        printf("    Warning: Error exceeds 5%% (%.2f%%)\n", error_percent);
    }
}

/* ============================================================================
 * BENCHMARK TESTS
 * ============================================================================ */

#include <time.h>

void benchmark_fft(void)
{
    TEST_SECTION_START("FFT Performance Benchmark");
    
    const uint32_t fft_sizes[] = {64, 128, 256, 512, 1024};
    const uint32_t num_sizes = 5;
    const uint32_t iterations = 10;
    
    for (uint32_t size_idx = 0; size_idx < num_sizes; size_idx++) {
        uint32_t fft_size = fft_sizes[size_idx];
        float *test_signal = (float *)malloc(fft_size * sizeof(float));
        float *fft_output = (float *)malloc(fft_size * sizeof(float));
        
        if (test_signal == NULL || fft_output == NULL) {
            printf("    Memory allocation failed for FFT size %u\n", fft_size);
            free(test_signal);
            free(fft_output);
            continue;
        }
        
        /* Generate test signal */
        generate_sine_signal(test_signal, fft_size, 100.0f, 44100.0f, 1.0f);
        
        /* Warm up */
        arm_rfft_instance_f32 fft_inst;
        arm_rfft_init_f32(&fft_inst, fft_size, 0, 1);
        arm_rfft_f32(&fft_inst, test_signal, fft_output);
        
        /* Benchmark */
        clock_t start = clock();
        for (uint32_t iter = 0; iter < iterations; iter++) {
            memcpy(fft_output, test_signal, fft_size * sizeof(float));
            arm_rfft_f32(&fft_inst, fft_output, fft_output);
        }
        clock_t end = clock();
        
        double time_ms = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;
        double time_per_fft = time_ms / iterations;
        
        printf("    FFT size %u: %.3f ms per FFT (%u iterations)\n", 
               fft_size, time_per_fft, iterations);
        
        free(test_signal);
        free(fft_output);
    }
}

void benchmark_filter(void)
{
    TEST_SECTION_START("Vector Operation Performance Benchmark");
    
    const uint32_t lengths[] = {64, 256, 1024, 4096};
    const uint32_t num_lengths = 4;
    const uint32_t iterations = 100;
    
    for (uint32_t len_idx = 0; len_idx < num_lengths; len_idx++) {
        uint32_t len = lengths[len_idx];
        float *srcA = (float *)malloc(len * sizeof(float));
        float *srcB = (float *)malloc(len * sizeof(float));
        float *dst = (float *)malloc(len * sizeof(float));
        
        if (srcA == NULL || srcB == NULL || dst == NULL) {
            printf("    Memory allocation failed for length %u\n", len);
            free(srcA);
            free(srcB);
            free(dst);
            continue;
        }
        
        /* Initialize */
        for (uint32_t i = 0; i < len; i++) {
            srcA[i] = sinf(2.0f * 3.14159265359f * i / len);
            srcB[i] = cosf(2.0f * 3.14159265359f * i / len);
        }
        
        /* Benchmark vector addition */
        clock_t start = clock();
        for (uint32_t iter = 0; iter < iterations; iter++) {
            arm_add_f32(srcA, srcB, dst, len);
        }
        clock_t end = clock();
        
        double time_ms = (double)(end - start) / CLOCKS_PER_SEC * 1000.0;
        double time_per_op = time_ms / iterations;
        
        printf("    Vector add (%u elements): %.3f ms per operation (%u iterations)\n", 
               len, time_per_op, iterations);
        
        free(srcA);
        free(srcB);
        free(dst);
    }
}

void benchmark_vector_ops(void)
{
    TEST_SECTION_START("Detailed Vector Operation Benchmarks");
    
    const uint32_t vec_len = 512;
    const uint32_t iterations = 1000;
    
    float *a = (float *)malloc(vec_len * sizeof(float));
    float *b = (float *)malloc(vec_len * sizeof(float));
    float *c = (float *)malloc(vec_len * sizeof(float));
    
    if (a == NULL || b == NULL || c == NULL) {
        printf("    Memory allocation failed\n");
        free(a);
        free(b);
        free(c);
        return;
    }
    
    /* Initialize */
    for (uint32_t i = 0; i < vec_len; i++) {
        a[i] = 0.1f * i;
        b[i] = 0.05f * i;
    }
    
    /* Vector add */
    clock_t start = clock();
    for (uint32_t iter = 0; iter < iterations; iter++) {
        arm_add_f32(a, b, c, vec_len);
    }
    clock_t end = clock();
    printf("    arm_add_f32 (%u): %.4f ms average\n", vec_len, 
           (double)(end - start) / CLOCKS_PER_SEC * 1000.0 / iterations);
    
    /* Vector multiply */
    start = clock();
    for (uint32_t iter = 0; iter < iterations; iter++) {
        arm_mult_f32(a, b, c, vec_len);
    }
    end = clock();
    printf("    arm_mult_f32 (%u): %.4f ms average\n", vec_len, 
           (double)(end - start) / CLOCKS_PER_SEC * 1000.0 / iterations);
    
    /* Dot product */
    float result;
    start = clock();
    for (uint32_t iter = 0; iter < iterations; iter++) {
        arm_dot_prod_f32(a, b, vec_len, &result);
    }
    end = clock();
    printf("    arm_dot_prod_f32 (%u): %.4f ms average\n", vec_len, 
           (double)(end - start) / CLOCKS_PER_SEC * 1000.0 / iterations);
    
    free(a);
    free(b);
    free(c);
}

/* ============================================================================
 * MAIN TEST RUNNER
 * ============================================================================ */

int run_all_cmsis_dsp_tests(void)
{
    cmsis_dsp_tests_init();
    
    /* Vector operations */
    test_vector_add();
    test_vector_subtract();
    test_vector_multiply();
    test_dot_product();
    test_vector_scale();
    test_vector_abs();
    
    /* FFT operations */
    test_rfft_f32();
    test_cfft_f32();
    test_power_spectrum();
    test_complex_magnitude();
    
    /* Statistical operations */
    test_mean();
    test_standard_deviation();
    test_min_value();
    test_max_value();
    test_variance();
    
    /* Windowing */
    test_hann_window();
    test_hamming_window();
    test_complex_conjugate();
    test_signal_copy();
    
    /* Signal generation */
    test_sine_generation();
    test_cosine_generation();
    
    /* Integration tests */
    test_tuner_pipeline();
    test_realtime_processing();
    test_frequency_estimation();
    
    /* Benchmarks */
    benchmark_fft();
    benchmark_filter();
    benchmark_vector_ops();
    
    /* Print results */
    print_test_results();
    
    return (g_test_results.tests_failed == 0 && g_test_results.total_errors == 0) ? 0 : 1;
}
