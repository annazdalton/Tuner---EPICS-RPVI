/*
 * 
 * This file contains the main program loop that integrates all tuner components:
 * - Audio input processing
 * - String detection
 * - Tuning analysis
 * - Audio feedback sequencing
 */

#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "string_detection.h"
#include "audio_processing.h"
#include "audio_sequencer.h"

// Add these missing function declarations
void print_tuning_result(TuningResult* result);
void check_user_input(void);
unsigned long millis(void);
void delay(unsigned long ms);

// Global variables for tuner state
static int current_target_string = 0; // 0 = auto-detect, 1-6 = specific string
static TunerState tuner_state = STATE_IDLE;

/**
 * Main initialization function
 * Sets up all subsystems for the tuner
 */
void setup() {
    printf("Initializing Guitar Tuner...\n");
    
    // Initialize audio processing subsystem
    audio_processing_init();
    
    // Initialize audio sequencer for playback
    audio_sequencer_init();
    
    // Initialize string detection module
    string_detection_init();
    
    printf("Tuner initialization complete.\n");
    printf("Ready for guitar input...\n");
}

/**
 * Process a single tuning cycle
 * This is the core function that runs each time we analyze audio input
 */
void process_tuning_cycle() {
    double detected_frequency;
    TuningResult tuning_result;
    
    // Step 1: Capture and process audio to get frequency
    if (audio_processing_capture(&detected_frequency)) {
        printf("Detected frequency: %.2f Hz\n", detected_frequency);
        
        // Step 2: Analyze tuning based on detected frequency
        if (current_target_string == 0) {
            // Auto-detect mode - find which string was played
            tuning_result = analyze_tuning_auto(detected_frequency);
        } else {
            // Specific string mode - tune to target string
            tuning_result = analyze_tuning(detected_frequency, current_target_string);
        }
        
        // Step 3: Display tuning results
        print_tuning_result(&tuning_result);
        
        // Step 4: Generate audio feedback
        generate_audio_feedback(&tuning_result);
        
    } else {
        printf("No valid frequency detected. Check audio input.\n");
    }
}

/**
 * Print tuning results to serial monitor for debugging
 */
void print_tuning_result(TuningResult* result) {
    printf("\n=== TUNING RESULTS ===\n");
    printf("Detected String: %d\n", result->detected_string);
    printf("Target String: %d\n", result->target_string);
    printf("Detected Frequency: %.2f Hz\n", result->detected_frequency);
    printf("Target Frequency: %.2f Hz\n", result->target_frequency);
    printf("Cents Offset: %.1f\n", result->cents_offset);
    printf("Tuning Direction: %s\n", result->direction);
    printf("========================\n\n");
}

/**
 * Set the target string for tuning
@param string_num: 0=auto, 1=E2, 2=A2, 3=D3, 4=G3, 5=B3, 6=E4 
 Also Param string num means  1-6 for specific strings and 0 for auto-detect
 */
void set_target_string(int string_num) {
    if (string_num >= 0 && string_num <= 6) {
        current_target_string = string_num;
        if (string_num == 0) {
            printf("Mode: Auto-detect\n");
        } else {
            printf("Target string set to: %d\n", string_num);
        }
    }
}

/**
 * Main program loop
 * Runs continuously on the Teensy microcontroller
 */
void loop() {
    static unsigned long last_analysis = 0;
    const unsigned long ANALYSIS_INTERVAL = 500; // ms between analyses
    
    // Check if it's time for another analysis
    if (millis() - last_analysis >= ANALYSIS_INTERVAL) {
        process_tuning_cycle();
        last_analysis = millis();
    }
    
    // Check for user input (button presses to select strings)
    check_user_input();
    
    // Update audio sequencer state
    audio_sequencer_update();
}

/**
 * Check for user input from physical buttons
 * This would be connected to actual GPIO pins on the Teensy
 */
void check_user_input() {
    // This is a placeholder - in actual implementation, 
    // this would read from physical buttons to select strings
    
    // Example: If button 1 pressed, set target to string 1
    // if (digitalRead(BUTTON_PIN_1) == HIGH) {
    //     set_target_string(1);
    // }
}

/**
 * Main entry point
 */
/*
int main() {
    setup();
    
    while (1) {
        loop();
        // Small delay to prevent overwhelming the processor
        delay(10);
    }
    
    return 0;
}
*/

// Arduino-compatible functions for Teensy
unsigned long millis() {
    // This would return the actual millisecond counter from Teensy
    // For simulation, we increment a counter
    static unsigned long counter = 0;
    return counter += 10;
}

void delay(unsigned long ms) {
    // This would implement an actual delay on Teensy
    // For simulation, we do nothing
}