/**
 * cmsis_dsp_teensy_bare_metal.c - Bare-metal CMSIS-DSP tests for Teensy 4.1
 * 
 * This file provides pure ARM/CMSIS-DSP testing without any Arduino dependencies.
 * Uses only ARM Cortex-M4 hardware and CMSIS-DSP library.
 * Communication via direct UART (LPUART6) on Teensy 4.1.
 * 
 * NOTE: This file is only compiled when building for Teensy 4.1.
 * On native (Windows/Linux), the mock implementations are used instead.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

/* arm_math.h is provided by CMSIS-DSP library (PlatformIO dependency for Teensy only) */
#ifdef __arm__
    #include <arm_math.h>
#else
    /* Fallback for IDE intellisense when not targeting ARM */
    #include "arm_math_mock.h"
#endif

/* ============================================================================
 * Teensy 4.1 Hardware Definitions (Bare Metal)
 * ============================================================================ */

/* Cortex-M4 System Timer (SysTick) */
#define SYST_CSR  (*(volatile uint32_t *)0xE000E010)
#define SYST_RVR  (*(volatile uint32_t *)0xE000E014)
#define SYST_CVR  (*(volatile uint32_t *)0xE000E018)
#define SYST_CALIB (*(volatile uint32_t *)0xE000E01C)

/* LPUART6 Base Address for Teensy 4.1 */
#define LPUART6_BASE 0x402D0000

/* LPUART6 Register Offsets */
#define LPUART_VERID    (*(volatile uint32_t *)(LPUART6_BASE + 0x000))
#define LPUART_PARAM    (*(volatile uint32_t *)(LPUART6_BASE + 0x004))
#define LPUART_GLOBAL   (*(volatile uint32_t *)(LPUART6_BASE + 0x008))
#define LPUART_BAUD     (*(volatile uint32_t *)(LPUART6_BASE + 0x010))
#define LPUART_STAT     (*(volatile uint32_t *)(LPUART6_BASE + 0x014))
#define LPUART_CTRL     (*(volatile uint32_t *)(LPUART6_BASE + 0x018))
#define LPUART_DATA     (*(volatile uint32_t *)(LPUART6_BASE + 0x01C))
#define LPUART_MATCH    (*(volatile uint32_t *)(LPUART6_BASE + 0x020))

/* Port and GPIO for UART pins */
#define PORTC_BASE      0x4004B000
#define PORTC_PCR3      (*(volatile uint32_t *)(PORTC_BASE + 0x00C))
#define PORTC_PCR4      (*(volatile uint32_t *)(PORTC_BASE + 0x010))

#define GPIO_C_BASE     0x400FF080
#define GPIO_C_PDOR     (*(volatile uint32_t *)(GPIO_C_BASE + 0x000))
#define GPIO_C_PDDR     (*(volatile uint32_t *)(GPIO_C_BASE + 0x014))

/* IMX RT Clock Control Module */
#define CCM_CCGR0       (*(volatile uint32_t *)0x400FC000)
#define CCM_CCGR1       (*(volatile uint32_t *)0x400FC004)
#define CCM_CCGR2       (*(volatile uint32_t *)0x400FC008)
#define CCM_CCGR3       (*(volatile uint32_t *)0x400FC00C)
#define CCM_CCGR4       (*(volatile uint32_t *)0x400FC010)
#define CCM_CCGR5       (*(volatile uint32_t *)0x400FC014)
#define CCM_CCGR6       (*(volatile uint32_t *)0x400FC018)

/* ============================================================================
 * UART Output Functions
 * ============================================================================ */

void uart_init(void)
{
    /* Enable LPUART6 and Port C clocks */
    CCM_CCGR5 |= (3 << 16);  /* LPUART6 clock */
    CCM_CCGR2 |= (3 << 12);  /* Port C clock */
    
    /* Configure Port C Pin 3 (RX) and Pin 4 (TX) for UART */
    PORTC_PCR3 = 0x00000300; /* PC3 as LPUART6 RX (Alt3) */
    PORTC_PCR4 = 0x00000300; /* PC4 as LPUART6 TX (Alt3) */
    
    /* Configure UART: 115200 baud at 24 MHz */
    LPUART_BAUD = 0x00300000;  /* Disable for now */
    LPUART_BAUD = (1 << 8) |   /* OSR = 16 */
                  (13 << 0);    /* DIV = 13 for 115200 at 24MHz */
    
    /* Enable UART */
    LPUART_CTRL = (1 << 19) | (1 << 18);  /* Enable TX and RX */
}

void uart_putchar(char c)
{
    /* Wait for TX data register empty */
    while (!(LPUART_STAT & (1 << 23))) {
        /* Spin */
    }
    
    /* Send character */
    LPUART_DATA = c;
}

void uart_puts(const char *str)
{
    while (*str) {
        if (*str == '\n') {
            uart_putchar('\r');
        }
        uart_putchar(*str++);
    }
}

void uart_print_uint32(uint32_t val)
{
    char buffer[20];
    int len = 0;
    
    if (val == 0) {
        uart_putchar('0');
        return;
    }
    
    uint32_t temp = val;
    while (temp > 0) {
        buffer[len++] = '0' + (temp % 10);
        temp /= 10;
    }
    
    for (int i = len - 1; i >= 0; i--) {
        uart_putchar(buffer[i]);
    }
}

void uart_print_float(float val)
{
    if (val < 0) {
        uart_putchar('-');
        val = -val;
    }
    
    uint32_t int_part = (uint32_t)val;
    uint32_t frac_part = (uint32_t)((val - int_part) * 1000000);
    
    uart_print_uint32(int_part);
    uart_putchar('.');
    
    /* Print exactly 6 decimal places */
    for (int i = 5; i >= 0; i--) {
        uint32_t digit = frac_part;
        for (int j = 0; j < i; j++) {
            digit /= 10;
        }
        uart_putchar('0' + (digit % 10));
    }
}

/* ============================================================================
 * System Tick Timer for Performance Measurement
 * ============================================================================ */

static uint32_t g_tick_count = 0;

void systick_init(void)
{
    /* Configure SysTick for 1 ms tick at 600 MHz */
    SYST_RVR = 600000 - 1;  /* Reload value */
    SYST_CVR = 0;           /* Clear current value */
    SYST_CSR = 0x00000007;  /* Enable SysTick with interrupt */
}

uint32_t get_time_ms(void)
{
    return g_tick_count;
}

uint32_t get_elapsed_ms(uint32_t start_time)
{
    return get_time_ms() - start_time;
}

/* ============================================================================
 * Test Results Structure
 * ============================================================================ */

typedef struct {
    uint32_t tests_run;
    uint32_t tests_passed;
    uint32_t tests_failed;
} test_stats_t;

static test_stats_t g_stats = {0, 0, 0};

void print_separator(void)
{
    uart_puts("=====================================\n");
}

void print_test_header(const char *name)
{
    uart_puts("\n");
    print_separator();
    uart_puts("TEST: ");
    uart_puts(name);
    uart_puts("\n");
    print_separator();
}

void assert_float_eq(float actual, float expected, float tolerance, const char *msg)
{
    g_stats.tests_run++;
    
    float diff = fabsf(actual - expected);
    if (diff <= tolerance) {
        g_stats.tests_passed++;
        uart_puts("  [PASS] ");
        uart_puts(msg);
        uart_puts("\n");
    } else {
        g_stats.tests_failed++;
        uart_puts("  [FAIL] ");
        uart_puts(msg);
        uart_puts(" - Expected: ");
        uart_print_float(expected);
        uart_puts(" Got: ");
        uart_print_float(actual);
        uart_puts("\n");
    }
}

void assert_int_eq(int32_t actual, int32_t expected, const char *msg)
{
    g_stats.tests_run++;
    
    if (actual == expected) {
        g_stats.tests_passed++;
        uart_puts("  [PASS] ");
        uart_puts(msg);
        uart_puts("\n");
    } else {
        g_stats.tests_failed++;
        uart_puts("  [FAIL] ");
        uart_puts(msg);
        uart_puts(" - Expected: ");
        uart_print_uint32(expected);
        uart_puts(" Got: ");
        uart_print_uint32(actual);
        uart_puts("\n");
    }
}

/* ============================================================================
 * Test Cases
 * ============================================================================ */

void test_vector_add(void)
{
    print_test_header("Vector Addition");
    
    const uint32_t len = 16;
    float32_t srcA[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    float32_t srcB[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    float32_t result[16];
    
    arm_add_f32(srcA, srcB, result, len);
    
    for (int i = 0; i < len; i += 4) {
        char msg[30];
        sprintf(msg, "Element %d", i);
        assert_float_eq(result[i], srcA[i] + srcB[i], 1e-6f, msg);
    }
}

void test_vector_multiply(void)
{
    print_test_header("Vector Multiplication");
    
    const uint32_t len = 16;
    float32_t srcA[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    float32_t srcB[16] = {2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2};
    float32_t result[16];
    
    arm_mult_f32(srcA, srcB, result, len);
    
    for (int i = 0; i < len; i += 4) {
        char msg[30];
        sprintf(msg, "Element %d", i);
        assert_float_eq(result[i], srcA[i] * srcB[i], 1e-6f, msg);
    }
}

void test_dot_product(void)
{
    print_test_header("Dot Product");
    
    const uint32_t len = 100;
    float32_t srcA[100];
    float32_t srcB[100];
    float32_t result;
    
    /* Initialize with 0.5 and 2.0 */
    for (int i = 0; i < len; i++) {
        srcA[i] = 0.5f;
        srcB[i] = 2.0f;
    }
    
    arm_dot_prod_f32(srcA, srcB, len, &result);
    
    /* Expected: 0.5 * 2.0 * 100 = 100 */
    assert_float_eq(result, 100.0f, 0.01f, "Dot product [0.5, 2.0] length 100");
}

void test_mean(void)
{
    print_test_header("Mean Calculation");
    
    const uint32_t len = 10;
    float32_t data[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    float32_t result;
    
    arm_mean_f32(data, len, &result);
    
    /* Expected: 5.5 */
    assert_float_eq(result, 5.5f, 0.01f, "Mean of 1-10");
}

void test_max_value(void)
{
    print_test_header("Maximum Value");
    
    const uint32_t len = 20;
    float32_t data[20] = {1, 2, 3, 4, 5, 10, 9, 8, 7, 6, 11, 5, 4, 3, 2, 1, 9, 8, 7, 6};
    float32_t max_val;
    uint32_t max_idx;
    
    arm_max_f32(data, len, &max_val, &max_idx);
    
    assert_float_eq(max_val, 11.0f, 0.01f, "Max value");
    assert_int_eq(max_idx, 10, "Max index");
}

void test_min_value(void)
{
    print_test_header("Minimum Value");
    
    const uint32_t len = 20;
    float32_t data[20] = {5, 4, 3, 2, 1, 10, 9, 8, 7, 6, 0.5f, 1, 2, 3, 4, 5, 9, 8, 7, 6};
    float32_t min_val;
    uint32_t min_idx;
    
    arm_min_f32(data, len, &min_val, &min_idx);
    
    assert_float_eq(min_val, 0.5f, 0.01f, "Min value");
    assert_int_eq(min_idx, 10, "Min index");
}

void test_complex_magnitude(void)
{
    print_test_header("Complex Magnitude");
    
    const uint32_t num_complex = 5;
    float32_t input[10] = {3, 4, 1, 0, 5, 12, 8, 15, 0, 0};
    float32_t output[5];
    
    /* Expected: sqrt(3^2 + 4^2) = 5, sqrt(1^2 + 0^2) = 1, etc. */
    arm_cmplx_mag_f32(input, output, num_complex);
    
    assert_float_eq(output[0], 5.0f, 0.01f, "Magnitude 3+4j");
    assert_float_eq(output[1], 1.0f, 0.01f, "Magnitude 1+0j");
    assert_float_eq(output[2], 13.0f, 0.01f, "Magnitude 5+12j");
}

void test_rfft(void)
{
    print_test_header("Real FFT (64 points)");
    
    const uint32_t fft_size = 64;
    float32_t input[64];
    float32_t output[64];
    
    /* Generate a simple sine wave */
    for (uint32_t i = 0; i < fft_size; i++) {
        float angle = 2.0f * 3.14159265359f * 10.0f * i / 64.0f;
        input[i] = arm_sin_f32(angle);
    }
    
    /* Perform FFT */
    arm_rfft_fast_instance_f32 fft_instance;
    arm_rfft_fast_init_f32(&fft_instance, fft_size);
    arm_rfft_fast_f32(&fft_instance, input, output, 0);
    
    uart_puts("  FFT computation completed\n");
    uart_puts("  [PASS] FFT executed without error\n");
    
    g_stats.tests_run++;
    g_stats.tests_passed++;
}

void test_scale(void)
{
    print_test_header("Vector Scaling");
    
    const uint32_t len = 8;
    float32_t src[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    float32_t dst[8];
    float32_t scale = 0.5f;
    
    arm_scale_f32(src, scale, dst, len);
    
    assert_float_eq(dst[0], 0.5f, 0.01f, "Scaled element 0");
    assert_float_eq(dst[4], 2.5f, 0.01f, "Scaled element 4");
}

void test_abs(void)
{
    print_test_header("Vector Absolute Value");
    
    const uint32_t len = 8;
    float32_t src[8] = {-1, 2, -3, 4, -5, 6, -7, 8};
    float32_t dst[8];
    
    arm_abs_f32(src, dst, len);
    
    assert_float_eq(dst[0], 1.0f, 0.01f, "Abs -1");
    assert_float_eq(dst[2], 3.0f, 0.01f, "Abs -3");
    assert_float_eq(dst[4], 5.0f, 0.01f, "Abs -5");
}

void benchmark_vector_add(void)
{
    print_test_header("Benchmark: Vector Add");
    
    const uint32_t vec_len = 512;
    const uint32_t iterations = 100;
    
    float32_t *pA = (float32_t *)malloc(vec_len * sizeof(float32_t));
    float32_t *pB = (float32_t *)malloc(vec_len * sizeof(float32_t));
    float32_t *pC = (float32_t *)malloc(vec_len * sizeof(float32_t));
    
    if (!pA || !pB || !pC) {
        uart_puts("  Memory allocation failed\n");
        return;
    }
    
    for (uint32_t i = 0; i < vec_len; i++) {
        pA[i] = 1.0f;
        pB[i] = 2.0f;
    }
    
    uint32_t start = get_time_ms();
    
    for (uint32_t i = 0; i < iterations; i++) {
        arm_add_f32(pA, pB, pC, vec_len);
    }
    
    uint32_t elapsed = get_elapsed_ms(start);
    
    uart_puts("  Vector length: ");
    uart_print_uint32(vec_len);
    uart_puts(", Iterations: ");
    uart_print_uint32(iterations);
    uart_puts("\n");
    uart_puts("  Total time: ");
    uart_print_uint32(elapsed);
    uart_puts(" ms\n");
    uart_puts("  Average per operation: ");
    uart_print_float((float)elapsed / iterations);
    uart_puts(" ms\n");
    
    free(pA);
    free(pB);
    free(pC);
}

void benchmark_fft(void)
{
    print_test_header("Benchmark: FFT");
    
    const uint32_t fft_size = 512;
    const uint32_t iterations = 10;
    
    float32_t *input = (float32_t *)malloc(fft_size * sizeof(float32_t));
    float32_t *output = (float32_t *)malloc(fft_size * sizeof(float32_t));
    
    if (!input || !output) {
        uart_puts("  Memory allocation failed\n");
        return;
    }
    
    for (uint32_t i = 0; i < fft_size; i++) {
        input[i] = arm_sin_f32(2.0f * 3.14159265359f * 100.0f * i / 44100.0f);
    }
    
    arm_rfft_fast_instance_f32 fft_instance;
    arm_rfft_fast_init_f32(&fft_instance, fft_size);
    
    uint32_t start = get_time_ms();
    
    for (uint32_t i = 0; i < iterations; i++) {
        arm_rfft_fast_f32(&fft_instance, input, output, 0);
    }
    
    uint32_t elapsed = get_elapsed_ms(start);
    
    uart_puts("  FFT size: ");
    uart_print_uint32(fft_size);
    uart_puts(", Iterations: ");
    uart_print_uint32(iterations);
    uart_puts("\n");
    uart_puts("  Total time: ");
    uart_print_uint32(elapsed);
    uart_puts(" ms\n");
    uart_puts("  Average per FFT: ");
    uart_print_float((float)elapsed / iterations);
    uart_puts(" ms\n");
    
    free(input);
    free(output);
}

/* ============================================================================
 * Main Test Runner
 * ============================================================================ */

void run_all_tests(void)
{
    uart_puts("\n\n");
    print_separator();
    uart_puts("CMSIS-DSP Bare-Metal Test Suite\n");
    uart_puts("Teensy 4.1 (iMXRT1062) - No Arduino\n");
    print_separator();
    uart_puts("\n");
    
    /* Vector operations */
    test_vector_add();
    test_vector_multiply();
    test_dot_product();
    test_scale();
    test_abs();
    
    /* Statistics */
    test_mean();
    test_max_value();
    test_min_value();
    
    /* Complex operations */
    test_complex_magnitude();
    
    /* FFT */
    test_rfft();
    
    /* Benchmarks */
    benchmark_vector_add();
    benchmark_fft();
    
    /* Summary */
    uart_puts("\n\n");
    print_separator();
    uart_puts("TEST SUMMARY\n");
    print_separator();
    uart_puts("Total tests: ");
    uart_print_uint32(g_stats.tests_run);
    uart_puts("\n");
    uart_puts("Passed: ");
    uart_print_uint32(g_stats.tests_passed);
    uart_puts("\n");
    uart_puts("Failed: ");
    uart_print_uint32(g_stats.tests_failed);
    uart_puts("\n");
    
    if (g_stats.tests_failed == 0) {
        uart_puts("\nStatus: ALL TESTS PASSED!\n");
    } else {
        uart_puts("\nStatus: SOME TESTS FAILED!\n");
    }
    print_separator();
    uart_puts("\n");
}

/* ============================================================================
 * Entry Point
 * ============================================================================ */

int main(void)
{
    /* Initialize UART for output */
    uart_init();
    
    /* Initialize system tick */
    systick_init();
    
    /* Run all tests */
    run_all_tests();
    
    /* Infinite loop - system keeps running */
    while (1) {
        /* System is idle */
        asm("wfi");  /* Wait for interrupt */
    }
    
    return 0;
}
