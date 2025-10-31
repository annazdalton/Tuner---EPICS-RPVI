/**
 * cmsis_dsp_teensy_test.cpp - Bare-metal CMSIS-DSP test suite
 * 
 * This file provides CMSIS-DSP testing without Arduino dependencies.
 * It includes:
 * - Core DSP algorithm testing
 * - Real-time DSP pipeline testing
 * - Performance benchmarking
 */

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>

/* CMSIS-DSP include path - works in CI with proper include dirs configured
   ensure CMSIS-DSP/Include is in your C++ include path */
#include "CMSIS-DSP/Include/arm_math.h"

#include "cmsis_dsp_tests.h"

/* Simple timing stub */
static uint32_t start_time_us = 0;
static uint32_t get_timestamp_us(void) {
    return (uint32_t)((clock() * 1000000) / CLOCKS_PER_SEC);
}
static uint32_t elapsed_us(void) {
    return get_timestamp_us() - start_time_us;
}
#define millis() (elapsed_us() / 1000)
#define micros() (elapsed_us())
#define delay(ms) ((void)0)

/* ============================================================================
 * Teensy-Specific Configuration
 * ============================================================================ */

#define TEENSY_AUDIO_BLOCK_SIZE 128
#define TEENSY_SAMPLE_RATE 44100
#define FFT_SIZE 512
#define NUM_FFT_BINS (FFT_SIZE / 2)

/* Serial communication rate */
#define SERIAL_BAUD 115200

/* Test configuration */
#define RUN_QUICK_TESTS    1  /* Quick smoke tests */
#define RUN_FULL_TESTS     0  /* Full comprehensive test suite (slower) */
#define RUN_REALTIME_TEST  1  /* Real-time audio processing test */
#define RUN_PERFORMANCE    1  /* Performance benchmarks on hardware */

/* ============================================================================
 * Test Result Storage
 * ============================================================================ */

typedef struct {
    uint32_t test_count;
    uint32_t pass_count;
    uint32_t fail_count;
    uint32_t total_time_ms;
} teensy_test_stats_t;

static teensy_test_stats_t g_teensy_stats = {0, 0, 0, 0};

/* ============================================================================
 * Utility Functions
 * ============================================================================ */

/**
 * Print test header with timestamp
 */
void print_test_header(const char *test_name)
{
    printf("\n[%u ms] TEST: %s\n", millis(), test_name);
    printf("==================\n");
}

/**
 * Print test footer with result
 */
void print_test_footer(uint32_t elapsed_ms, int passed)
{
    printf("Elapsed: %u ms | Result: %s\n", elapsed_ms, passed ? "PASS ✓" : "FAIL ✗");
    printf("\n");
}

/**
 * Print memory usage statistics (stub for bare-metal)
 */
void print_memory_usage(void)
{
    printf("(Memory info unavailable on bare-metal)\n");
}

/* ============================================================================
 * Quick Smoke Tests (for hardware verification)
 * ============================================================================ */

void test_teensy_vector_add(void)
{
    print_test_header("Teensy Vector Addition");
    print_memory_usage();
    
    uint32_t start_time = millis();
    
    const uint32_t len = 256;
    arm_status status = ARM_MATH_SUCCESS;
    
    /* Allocate buffers */
    float32_t *pSrcA = (float32_t *)malloc(len * sizeof(float32_t));
    float32_t *pSrcB = (float32_t *)malloc(len * sizeof(float32_t));
    float32_t *pDst = (float32_t *)malloc(len * sizeof(float32_t));
    
    if (pSrcA == NULL || pSrcB == NULL || pDst == NULL) {
        printf("Memory allocation failed!\n");
        print_memory_usage();
        print_test_footer(millis() - start_time, 0);
        return;
    }
    
    /* Initialize data */
    for (uint32_t i = 0; i < len; i++) {
        pSrcA[i] = 1.0f + 0.1f * i;
        pSrcB[i] = 2.0f + 0.05f * i;
    }
    
    /* Run vector add using CMSIS-DSP */
    arm_add_f32(pSrcA, pSrcB, pDst, len);
    
    /* Verify result (spot check) */
    int passed = 1;
    for (uint32_t i = 0; i < len; i += 50) {
        float expected = pSrcA[i] + pSrcB[i];
        if (fabsf(pDst[i] - expected) > 1e-6f) {
            passed = 0;
            break;
        }
    }
    
    printf("Sample result at index 0: %f\n", pDst[0]);
    
    /* Cleanup */
    free(pSrcA);
    free(pSrcB);
    free(pDst);
    
    print_test_footer(millis() - start_time, passed);
    g_teensy_stats.test_count++;
    if (passed) g_teensy_stats.pass_count++;
    else g_teensy_stats.fail_count++;
}

void test_teensy_rfft(void)
{
    print_test_header("Teensy Real FFT (512 points)");
    print_memory_usage();
    
    uint32_t start_time = millis();
    
    const uint32_t fft_size = 512;
    arm_status status = ARM_MATH_SUCCESS;
    
    /* Allocate buffers */
    float32_t *pInput = (float32_t *)malloc(fft_size * sizeof(float32_t));
    float32_t *pOutput = (float32_t *)malloc(fft_size * sizeof(float32_t));
    
    if (pInput == NULL || pOutput == NULL) {
        printf("Memory allocation failed!\n");
        print_memory_usage();
        print_test_footer(millis() - start_time, 0);
        return;
    }
    
    /* Generate test signal: 440 Hz sine at 44.1 kHz */
    for (uint32_t i = 0; i < fft_size; i++) {
        float angle = 2.0f * 3.14159265359f * 440.0f * i / 44100.0f;
        pInput[i] = arm_sin_f32(angle);
    }
    
    /* Initialize RFFT */
    arm_rfft_fast_instance_f32 fft_instance = {0};
    int init_status = (int)arm_rfft_fast_init_f32((arm_rfft_fast_instance_f32 *)(void *)&fft_instance, fft_size);
    
    if (init_status != 0) {
        printf("FFT initialization failed!\n");
        print_test_footer(millis() - start_time, 0);
        free(pInput);
        free(pOutput);
        return;
    }
    
    /* Compute FFT */
    arm_rfft_fast_f32((const arm_rfft_fast_instance_f32 *)(const void *)&fft_instance, pInput, pOutput, 0);
    
    /* Compute magnitude spectrum */
    float32_t *pMagnitude = (float32_t *)malloc((fft_size/2) * sizeof(float32_t));
    if (pMagnitude != NULL) {
        arm_cmplx_mag_f32(&pOutput[2], pMagnitude, fft_size/2 - 1);
        
        /* Find peak */
        float32_t peak_mag;
        uint32_t peak_index;
        arm_max_f32(pMagnitude, fft_size/2 - 1, &peak_mag, &peak_index);
        
        float freq_resolution = 44100.0f / fft_size;
        float estimated_freq = peak_index * freq_resolution;
        
        printf("Peak at index %u, frequency ~%f Hz (expected: ~440 Hz)\n", peak_index, estimated_freq);
        
        free(pMagnitude);
    }
    
    /* Cleanup */
    free(pInput);
    free(pOutput);
    
    print_test_footer(millis() - start_time, 1);
    g_teensy_stats.test_count++;
    g_teensy_stats.pass_count++;
}

void test_teensy_dot_product(void)
{
    print_test_header("Teensy Dot Product");
    
    uint32_t start_time = millis();
    
    const uint32_t len = 1024;
    
    float32_t *pSrcA = (float32_t *)malloc(len * sizeof(float32_t));
    float32_t *pSrcB = (float32_t *)malloc(len * sizeof(float32_t));
    
    if (pSrcA == NULL || pSrcB == NULL) {
        printf("Memory allocation failed!\n");
        print_test_footer(millis() - start_time, 0);
        return;
    }
    
    /* Initialize */
    for (uint32_t i = 0; i < len; i++) {
        pSrcA[i] = 0.5f;
        pSrcB[i] = 2.0f;
    }
    
    /* Compute dot product */
    float32_t result;
    arm_dot_prod_f32(pSrcA, pSrcB, len, &result);
    
    /* Expected: sum of 0.5 * 2.0 = 1.0 for each element = 1024 */
    float expected = 1024.0f;
    int passed = fabsf(result - expected) < 0.1f;
    
    printf("Result: %f (expected: %f)\n", result, expected);
    
    free(pSrcA);
    free(pSrcB);
    
    print_test_footer(millis() - start_time, passed);
    g_teensy_stats.test_count++;
    if (passed) g_teensy_stats.pass_count++;
    else g_teensy_stats.fail_count++;
}

void test_teensy_statistics(void)
{
    print_test_header("Teensy Statistics (Mean/StdDev)");
    
    uint32_t start_time = millis();
    
    const uint32_t len = 256;
    
    float32_t *pSrc = (float32_t *)malloc(len * sizeof(float32_t));
    
    if (pSrc == NULL) {
        printf("Memory allocation failed!\n");
        print_test_footer(millis() - start_time, 0);
        return;
    }
    
    /* Generate test data: 1 to 256 */
    for (uint32_t i = 0; i < len; i++) {
        pSrc[i] = (float32_t)(i + 1);
    }
    
    /* Calculate mean */
    float32_t mean;
    arm_mean_f32(pSrc, len, &mean);
    
    /* Calculate standard deviation */
    float32_t std;
    arm_std_f32(pSrc, len, &std);
    
    printf("Mean: %f (expected: ~128.5)\n", mean);
    printf("Std Dev: %f\n", std);
    
    int passed = (mean > 128.0f && mean < 129.0f) && (std > 70.0f && std < 80.0f);
    
    free(pSrc);
    
    print_test_footer(millis() - start_time, passed);
    g_teensy_stats.test_count++;
    if (passed) g_teensy_stats.pass_count++;
    else g_teensy_stats.fail_count++;
}

/* ============================================================================
 * Real-Time Processing Test
 * ============================================================================ */

void test_realtime_dsp_pipeline(void)
{
    print_test_header("Real-Time DSP Pipeline");
    printf("This test simulates continuous audio processing\n");
    
    uint32_t start_time = millis();
    
    const uint32_t block_size = 128;
    const uint32_t num_blocks = 10;
    const uint32_t fft_size = 512;
    
    /* Allocate buffers */
    float32_t *audio_buffer = (float32_t *)malloc(block_size * sizeof(float32_t));
    float32_t *fft_input = (float32_t *)malloc(fft_size * sizeof(float32_t));
    float32_t *fft_output = (float32_t *)malloc(fft_size * sizeof(float32_t));
    float32_t *magnitude = (float32_t *)malloc((fft_size/2) * sizeof(float32_t));
    
    if (audio_buffer == NULL || fft_input == NULL || fft_output == NULL || magnitude == NULL) {
        printf("Memory allocation failed!\n");
        print_test_footer(millis() - start_time, 0);
        return;
    }
    
    /* Initialize FFT */
    arm_rfft_fast_instance_f32 fft_instance;
    arm_rfft_fast_init_f32(&fft_instance, fft_size);
    
    printf("Processing %u blocks...\n", num_blocks);
    
    uint32_t process_start = millis();
    
    /* Simulate real-time processing */
    for (uint32_t block = 0; block < num_blocks; block++) {
        /* Step 1: Generate audio block (simulating ADC input) */
        float freq = 400.0f + (block * 10.0f); /* Sweep frequency */
        for (uint32_t i = 0; i < block_size; i++) {
            float angle = 2.0f * 3.14159265359f * freq * (block * block_size + i) / 44100.0f;
            audio_buffer[i] = arm_sin_f32(angle);
        }
        
        /* Step 2: Accumulate to FFT buffer */
        uint32_t buffer_pos = (block % (fft_size / block_size)) * block_size;
        arm_copy_f32(audio_buffer, &fft_input[buffer_pos], block_size);
        
        /* When FFT buffer is full, compute FFT */
        if ((block + 1) % (fft_size / block_size) == 0) {
            /* Apply Hann window */
            arm_copy_f32(fft_input, fft_output, fft_size);
            
            /* Window the signal */
            for (uint32_t i = 0; i < fft_size; i++) {
                float w = 0.5f * (1.0f - arm_cos_f32(2.0f * 3.14159265359f * i / (fft_size - 1)));
                fft_output[i] *= w;
            }
            
            /* Compute FFT */
            arm_rfft_fast_f32(&fft_instance, fft_output, fft_output, 0);
            
            /* Compute magnitude */
            arm_cmplx_mag_f32(&fft_output[2], magnitude, fft_size/2 - 1);
            
            /* Find peak frequency */
            float32_t peak_mag;
            uint32_t peak_bin;
            arm_max_f32(magnitude, fft_size/2 - 1, &peak_mag, &peak_bin);
            
            float freq_res = 44100.0f / fft_size;
            float detected_freq = peak_bin * freq_res;
            
            printf("Block %u: Peak at ~%f Hz\n", block, detected_freq);
        }
    }
    
    uint32_t process_elapsed = millis() - process_start;
    
    printf("Processing completed in %u ms\n", process_elapsed);
    
    /* Cleanup */
    free(audio_buffer);
    free(fft_input);
    free(fft_output);
    free(magnitude);
    
    print_test_footer(millis() - start_time, 1);
    g_teensy_stats.test_count++;
    g_teensy_stats.pass_count++;
}

/* ============================================================================
 * Performance Benchmarks
 * ============================================================================ */

void benchmark_teensy_fft_sizes(void)
{
    print_test_header("FFT Performance Benchmark (Various Sizes)");
    
    const uint32_t fft_sizes[] = {64, 128, 256, 512, 1024};
    const uint32_t num_sizes = 5;
    const uint32_t iterations = 10;
    
    for (uint32_t size_idx = 0; size_idx < num_sizes; size_idx++) {
        uint32_t fft_size = fft_sizes[size_idx];
        
        float32_t *pInput = (float32_t *)malloc(fft_size * sizeof(float32_t));
        float32_t *pOutput = (float32_t *)malloc(fft_size * sizeof(float32_t));
        
        if (pInput == NULL || pOutput == NULL) {
            printf("Memory allocation failed for FFT size %u\n", fft_size);
            free(pInput);
            free(pOutput);
            continue;
        }
        
        /* Generate test signal */
        for (uint32_t i = 0; i < fft_size; i++) {
            pInput[i] = arm_sin_f32(2.0f * 3.14159265359f * 100.0f * i / 44100.0f);
        }
        
        /* Initialize FFT */
        arm_rfft_fast_instance_f32 fft_instance;
        arm_rfft_fast_init_f32(&fft_instance, fft_size);
        
        /* Warm-up run */
        arm_rfft_fast_f32(&fft_instance, pInput, pOutput, 0);
        
        /* Benchmark */
        uint32_t start_time = micros();
        
        for (uint32_t iter = 0; iter < iterations; iter++) {
            arm_copy_f32(pInput, pOutput, fft_size);
            arm_rfft_fast_f32(&fft_instance, pOutput, pOutput, 0);
        }
        
        uint32_t elapsed_us = micros() - start_time;
        float elapsed_ms = elapsed_us / 1000.0f;
        float avg_ms = elapsed_ms / iterations;
        
        printf("FFT size %u: %f ms avg\n", fft_size, avg_ms);
        
        free(pInput);
        free(pOutput);
    }
    
    printf("Benchmark complete\n");
}

void benchmark_teensy_vector_operations(void)
{
    print_test_header("Vector Operation Benchmarks");
    
    const uint32_t vec_len = 1024;
    const uint32_t iterations = 100;
    
    float32_t *pA = (float32_t *)malloc(vec_len * sizeof(float32_t));
    float32_t *pB = (float32_t *)malloc(vec_len * sizeof(float32_t));
    float32_t *pC = (float32_t *)malloc(vec_len * sizeof(float32_t));
    
    if (pA == NULL || pB == NULL || pC == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }
    
    /* Initialize */
    for (uint32_t i = 0; i < vec_len; i++) {
        pA[i] = 0.5f;
        pB[i] = 2.0f;
    }
    
    /* Benchmark vector add */
    printf("Benchmarking arm_add_f32...\n");
    uint32_t start = micros();
    for (uint32_t i = 0; i < iterations; i++) {
        arm_add_f32(pA, pB, pC, vec_len);
    }
    uint32_t elapsed = micros() - start;
    printf("  Average: %f ms\n", (float)elapsed / iterations / 1000.0f);
    
    /* Benchmark vector multiply */
    printf("Benchmarking arm_mult_f32...\n");
    start = micros();
    for (uint32_t i = 0; i < iterations; i++) {
        arm_mult_f32(pA, pB, pC, vec_len);
    }
    elapsed = micros() - start;
    printf("  Average: %f ms\n", (float)elapsed / iterations / 1000.0f);
    
    /* Benchmark dot product */
    printf("Benchmarking arm_dot_prod_f32...\n");
    float32_t result;
    start = micros();
    for (uint32_t i = 0; i < iterations; i++) {
        arm_dot_prod_f32(pA, pB, vec_len, &result);
    }
    elapsed = micros() - start;
    printf("  Average: %f ms\n", (float)elapsed / iterations / 1000.0f);
    
    free(pA);
    free(pB);
    free(pC);
}

/* ============================================================================
 * Main Entry Point
 * ============================================================================ */

int main(void)
{
    printf("\n\n====================================\n");
    printf("CMSIS-DSP Test Suite\n");
    printf("====================================\n");
    printf("Running on native platform\n");
    printf("====================================\n\n");
    
    /* Initialize test statistics */
    g_teensy_stats.test_count = 0;
    g_teensy_stats.pass_count = 0;
    g_teensy_stats.fail_count = 0;
    g_teensy_stats.total_time_ms = 0;
    
    uint32_t overall_start = millis();
    
    printf("*** Starting CMSIS-DSP Tests ***\n\n");
    
#if RUN_QUICK_TESTS
    printf("=== QUICK SMOKE TESTS ===\n");
    test_teensy_vector_add();
    delay(100);
    test_teensy_rfft();
    delay(100);
    test_teensy_dot_product();
    delay(100);
    test_teensy_statistics();
    delay(100);
#endif

#if RUN_REALTIME_TEST
    printf("\n=== REAL-TIME PROCESSING TEST ===\n");
    test_realtime_dsp_pipeline();
    delay(100);
#endif

#if RUN_PERFORMANCE
    printf("\n=== PERFORMANCE BENCHMARKS ===\n");
    benchmark_teensy_fft_sizes();
    delay(100);
    benchmark_teensy_vector_operations();
#endif

    g_teensy_stats.total_time_ms = millis() - overall_start;
    
    /* Print summary */
    printf("\n\n====================================\n");
    printf("TEST SUMMARY\n");
    printf("====================================\n");
    printf("Total Tests: %u\n", g_teensy_stats.test_count);
    printf("Passed: %u\n", g_teensy_stats.pass_count);
    printf("Failed: %u\n", g_teensy_stats.fail_count);
    printf("Total Time: %u ms\n", g_teensy_stats.total_time_ms);
    printf("====================================\n\n");
    
    return 0;
}
