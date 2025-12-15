/**
 * main.cpp - Arduino-style entry point for Teensy 4.1 guitar tuner
 * Uses Arduino framework for startup but avoids Arduino library dependencies
 */

#include <cstdio>
#include "audio_processing.h"
#include "string_detection.h"

extern "C" {
    void setup(void);
    void loop(void);
}

void setup(void) {
    printf("=== Teensy 4.1 Guitar Tuner ===\n");
    
    // Initialize audio processing (FFT)
    audio_processing_init();
    
    // Initialize string detection
    string_detection_init();
    
    printf("Tuner initialized successfully!\n");
}

void loop(void) {
    // Main tuner processing would go here
    // For now, just a placeholder
}
