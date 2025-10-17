/**
 * string_detection.c - Guitar string detection and tuning analysis
 * 
 * Implements algorithms for:
 * - Identifying which guitar string is being played
 * - Calculating cents offset from perfect tuning
 * - Determining tuning direction (up/down)
 */

#include <math.h>
#include <stdio.h>
#include <string.h>
#include "string_detection.h"

// Note frequency structure for the complete guitar range
typedef struct {
    double frequency;
    const char* note_name;
    int string_number;  // Primary string this note belongs to
    int octave;
} NoteFrequency;

/**
 * Complete note frequency table for standard guitar tuning
 * Covers E2 to E4 range with all intermediate notes
 */
const NoteFrequency guitar_notes[] = {
    // String 6 (Low E) - E2 to approx G3
    {82.41, "E", 6, 2}, // Frequency ,Note, String, Octave
    {87.31, "F", 6, 2}, 
    {92.50, "F#", 6, 2}, 
    {98.00, "G", 6, 2}, 
    {103.83, "G#", 6, 2},
    {87.31, "F", 6, 2}, 
    {92.50, "F#", 6, 2}, 
    {98.00, "G", 6, 2}, 
    {103.83, "G#", 6, 2}, 
    {87.31, "F", 6, 2}, 
    {92.50, "F#", 6, 2}, 
    {98.00, "G", 6, 2},
    {103.83, "G#", 6, 2}, 
    
    // String 5 (A) - A2 to approx C4
    {110.00, "A", 5, 2}, 
    {116.54, "A#", 5, 2}, 
    {123.47, "B", 5, 2},
    {130.81, "C", 5, 3}, 
    {138.59, "C#", 5, 3},
    
    // String 4 (D) - D3 to approx F4
    {146.83, "D", 4, 3}, 
    {155.56, "D#", 4, 3},
    {164.81, "E", 4, 3}, 
    {174.61, "F", 4, 3}, 
    {185.00, "F#", 4, 3},
    
    // String 3 (G) - G3 to approx A#4
    {196.00, "G", 3, 3}, 
    {207.65, "G#", 3, 3},
    {220.00, "A", 3, 3}, 
    {233.08, "A#", 3, 3},
    
    // String 2 (B) - B3 to approx D5
    {246.94, "B", 2, 3},
    {261.63, "C", 2, 4}, 
    {277.18, "C#", 2, 4},
    {293.66, "D", 2, 4}, 
    {311.13, "D#", 2, 4},
    
    // String 1 (High E) - E4 to beyond
    {329.63, "E", 1, 4}, 
    {349.23, "F", 1, 4}, 
    {369.99, "F#", 1, 4},
    {392.00, "G", 1, 4}, 
    {415.30, "G#", 1, 4},
    {440.00, "A", 1, 4}, 
    {466.16, "A#", 1, 4},
    {493.88, "B", 1, 4}, 
    {523.25, "C", 1, 5}
};

#define NUM_NOTES (sizeof(guitar_notes) / sizeof(guitar_notes[0]))

// Standard guitar string target frequencies
const double string_frequencies[] = {
    GUITAR_STRING_1_FREQ,  // String 1 - High E
    GUITAR_STRING_2_FREQ,  // String 2 - B
    GUITAR_STRING_3_FREQ,  // String 3 - G  
    GUITAR_STRING_4_FREQ,  // String 4 - D
    GUITAR_STRING_5_FREQ,  // String 5 - A
    GUITAR_STRING_6_FREQ   // String 6 - Low E
};

/**
 * Initialize string detection module
 */
void string_detection_init(void) {
    printf("String detection module initialized.\n");
    printf("Number of notes in database: %ld\n", NUM_NOTES);
}

/**
 * Calculate cents offset between detected frequency and target frequency
 * Cents formula: 1200 * log2(detected_freq / target_freq)
 * 
 * @param detected_freq: Measured frequency from microphone
 * @param target_freq: Ideal target frequency for perfect tuning
 * @return: Cents offset (negative = flat, positive = sharp)
 */
double calculate_cents_offset(double detected_freq, double target_freq) {
    if (target_freq <= 0 || detected_freq <= 0) {
        return 0.0;  // Invalid input
    }
    return 1200.0 * log2(detected_freq / target_freq);
}

/**
 * Determine tuning direction based on cents offset
 * 
 * @param cents: Cents offset from calculate_cents_offset()
 * @return: "UP" if flat (need to tighten string),
 *          "DOWN" if sharp (need to loosen string),
 *          "IN_TUNE" if within acceptable range
 */
const char* get_tuning_direction(double cents) {
    const double TUNING_TOLERANCE = 5.0; // ±5 cents considered "in tune"
    
    if (cents < -TUNING_TOLERANCE) {
        return "UP";      // Too flat - tune up
    } else if (cents > TUNING_TOLERANCE) {
        return "DOWN";    // Too sharp - tune down  
    } else {
        return "IN_TUNE"; // Within acceptable range
    }
}

/**
 * Find the closest note in our frequency database
 * 
 * @param frequency: Detected frequency to match
 * @param closest_freq: Output parameter for closest frequency found
 * @param string_num: Output parameter for string number of closest note
 * @return: Index of closest note in guitar_notes array, or -1 if not found
 */
int find_closest_string(double frequency, double* closest_freq) {
    double min_diff = 1000.0;
    int closest_string = -1;
    
    for (int i = 0; i < 6; i++) {
        double diff = fabs(frequency - string_frequencies[i]);
        if (diff < min_diff) {
            min_diff = diff;
            closest_string = i + 1;
            *closest_freq = string_frequencies[i];
        }
    }
    
    return closest_string;
}

/**
 * Find the closest note in our frequency database
 */
int find_closest_note(double frequency, double* closest_freq, int* string_num) {
    double min_diff = 1000.0; // Large initial value
    int closest_index = -1;
    
    for (int i = 0; i < NUM_NOTES; i++) {
        double diff = fabs(frequency - guitar_notes[i].frequency);
        if (diff < min_diff) {
            min_diff = diff;
            closest_index = i;
            *closest_freq = guitar_notes[i].frequency;
            *string_num = guitar_notes[i].string_number;
        }
    }
    
    return closest_index;
}
 
/**
 * Analyze tuning when user has selected a specific target string
 * 
 * @param detected_frequency: Frequency measured from microphone
 * @param target_string: Which string the user wants to tune (1-6)
 * @return: TuningResult structure with complete analysis
 */
TuningResult analyze_tuning(double detected_frequency, int target_string) {
    TuningResult result;
    
    // Validate input
    if (target_string < 1 || target_string > 6) {
        // Invalid string number - use auto-detect instead
        return analyze_tuning_auto(detected_frequency);
    }
    
    // Set target frequency based on selected string
    result.target_frequency = string_frequencies[target_string - 1];
    result.target_string = target_string;
    
    // Find which string was actually detected
    double detected_string_freq;
    result.detected_string = find_closest_string(detected_frequency, &detected_string_freq);
    
    // Calculate tuning metrics
    result.detected_frequency = detected_frequency;
    result.cents_offset = calculate_cents_offset(detected_frequency, result.target_frequency);
    result.direction = get_tuning_direction(result.cents_offset);
    
    // Find note information for display
    double closest_note_freq;
    int note_string;
    int note_index = find_closest_note(detected_frequency, &closest_note_freq, &note_string);
    if (note_index >= 0) {
        result.note_name = guitar_notes[note_index].note_name;
        result.octave = guitar_notes[note_index].octave;
    } else {
        result.note_name = "?";
        result.octave = 0;
    }
    
    return result;
}

/**
 * Analyze tuning in auto-detect mode
 * Automatically determines which string is being played
 * 
 * @param detected_frequency: Frequency measured from microphone
 * @return: TuningResult structure with complete analysis
 */
TuningResult analyze_tuning_auto(double detected_frequency) {
    TuningResult result;
    
    // Auto-detect which string is being played
    double target_freq;
    result.detected_string = find_closest_string(detected_frequency, &target_freq);
    result.target_string = result.detected_string; // In auto mode, target = detected
    
    // Calculate tuning metrics
    result.detected_frequency = detected_frequency;
    result.target_frequency = target_freq;
    result.cents_offset = calculate_cents_offset(detected_frequency, target_freq);
    result.direction = get_tuning_direction(result.cents_offset);
    
    // Find note information for display
    double closest_note_freq;
    int note_string;
    int note_index = find_closest_note(detected_frequency, &closest_note_freq, &note_string);
    if (note_index >= 0) {
        result.note_name = guitar_notes[note_index].note_name;
        result.octave = guitar_notes[note_index].octave;
    } else {
        result.note_name = "?";
        result.octave = 0;
    }
    
    return result;
}