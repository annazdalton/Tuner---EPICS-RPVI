/**
 * cmsis_dsp_native_main.c - Native platform test runner
 * 
 * This is the entry point for running CMSIS-DSP tests on Windows/Linux
 * using the native platform in PlatformIO.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmsis_dsp_tests.h"

int main(int argc, char *argv[])
{
    printf("\n");
    printf("============================================================\n");
    printf("         CMSIS-DSP Test Suite - Native Platform            \n");
    printf("                   (Windows/Linux)                          \n");
    printf("============================================================\n");
    printf("\n");
    
    /* Parse command line arguments */
    int verbose = 0;
    int quick_only = 0;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = 1;
            printf("Verbose mode enabled\n");
        } else if (strcmp(argv[i], "-q") == 0 || strcmp(argv[i], "--quick") == 0) {
            quick_only = 1;
            printf("Quick tests only (no benchmarks)\n");
        }
    }
    
    printf("\n");
    
    /* Run all tests */
    int result = run_all_cmsis_dsp_tests();
    
    printf("\n");
    printf("============================================================\n");
    printf("Test execution completed\n");
    printf("============================================================\n");
    
    if (result == 0) {
        printf("\n* All tests passed!\n\n");
        return EXIT_SUCCESS;
    } else {
        printf("\n* Some tests failed!\n\n");
        return EXIT_FAILURE;
    }
}
