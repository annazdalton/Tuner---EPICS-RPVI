/**
 * string_detection.h - Header file for guitar string detection
 * 
 * Defines structures and functions for detecting which guitar string
 * is being played and analyzing tuning accuracy.
 */

#ifndef STRING_DETECTION_H
#define STRING_DETECTION_H

#include <stdint.h>

// Tuning result structure
typedef struct {
    int detected_string;      // Which string was detected (1-6)
    int target_string;        // Which string we're tuning to (1-6)
    double cents_offset;      // How far off from perfect tuning (-50 to +50 cents)
    const char* direction;    // "UP", "DOWN", or "IN_TUNE"
    double detected_frequency; // Actual measured frequency
    double target_frequency;  // Ideal target frequency
    const char* note_name;    // Note name (e.g., "E", "A", "D")
    int octave;              // Octave number
} TuningResult;

// Tuner states
typedef enum {
    STATE_IDLE,
    STATE_DETECTING,
    STATE_ANALYZING,
    STATE_PLAYING_FEEDBACK
} TunerState;

// Standard guitar string frequencies (E2, A2, D3, G3, B3, E4)
#define GUITAR_STRING_1_FREQ 329.63   // E4
#define GUITAR_STRING_2_FREQ 246.94   // B3  
#define GUITAR_STRING_3_FREQ 196.00   // G3
#define GUITAR_STRING_4_FREQ 146.83   // D3
#define GUITAR_STRING_5_FREQ 110.00   // A2
#define GUITAR_STRING_6_FREQ 82.41    // E2

// Function prototypes
void string_detection_init(void);
TuningResult analyze_tuning(double detected_frequency, int target_string);
TuningResult analyze_tuning_auto(double detected_frequency);
double calculate_cents_offset(double detected_freq, double target_freq);
const char* get_tuning_direction(double cents);
int find_closest_string(double frequency, double* closest_freq);
int find_closest_note(double frequency, double* closest_freq, int* string_num);

#endif
