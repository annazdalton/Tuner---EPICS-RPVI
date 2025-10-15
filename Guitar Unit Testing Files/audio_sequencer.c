/**
 * audio_sequencer.c - Audio sequencing implementation
 * 
 * Generates appropriate audio feedback based on tuning results
 * Plays note names, cent values, and tuning directions
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "audio_sequencer.h"

// Current state of audio sequencer
static int is_playing = 0;
static const TuningResult* current_result = NULL;
static int playback_step = 0;

/**
 * Initialize audio sequencer
 */
void audio_sequencer_init(void) {
    printf("Audio sequencer initialized.\n");
    is_playing = 0;
    playback_step = 0;
}

/**
 * Play an audio file (simulated - in real implementation uses Audio library)
 */
void play_audio_file(const char* filename) {
    printf("[AUDIO] Playing: %s\n", filename);
    // In actual implementation:
    // playWav1.play(filename);
    // while (playWav1.isPlaying()) { }
}

/**
 * Get audio filename for a string number
 */
const char* get_string_filename(int string_num) {
    switch (string_num) {
        case 1: return FILE_E;  // High E
        case 2: return FILE_B;  // B
        case 3: return FILE_G;  // G
        case 4: return FILE_D;  // D
        case 5: return FILE_A;  // A
        case 6: return FILE_E;  // Low E (same filename, different octave)
        default: return NULL;
    }
}

/**
 * Get audio filename for cents value
 */
const char* get_cents_filename(double cents) {
    double abs_cents = fabs(cents);
    
    if (abs_cents < 5) {
        return NULL; // Close enough to in-tune
    } else if (abs_cents < 15) {
        return FILE_10_CENTS;
    } else if (abs_cents < 25) {
        return FILE_20_CENTS;
    } else {
        return FILE_20_CENTS; // For larger offsets, still use 20 cents file
    }
}

/**
 * Generate audio feedback based on tuning results
 * Sequences the appropriate audio files
 */
void generate_audio_feedback(const TuningResult* result) {
    printf("Generating audio feedback...\n");
    
    // Store result for sequenced playback
    current_result = result;
    is_playing = 1;
    playback_step = 0;
}

/**
 * Update audio sequencer state machine
 * Called regularly from main loop to manage audio playback
 */
void audio_sequencer_update(void) {
    if (!is_playing || current_result == NULL) {
        return;
    }
    
    // State machine for sequenced audio playback
    switch (playback_step) {
        case 0: // Play string name
            {
                const char* string_file = get_string_filename(current_result->detected_string);
                if (string_file) {
                    play_audio_file(string_file);
                }
                playback_step++;
                break;
            }
            
        case 1: // Play cents value if significantly out of tune
            if (strcmp(current_result->direction, "IN_TUNE") != 0) {
                const char* cents_file = get_cents_filename(current_result->cents_offset);
                if (cents_file) {
                    // Small delay between audio files
                    // delay(200);
                    play_audio_file(cents_file);
                }
            }
            playback_step++;
            break;
            
        case 2: // Play tuning direction
            if (strcmp(current_result->direction, "IN_TUNE") != 0) {
                const char* direction_file = NULL;
                if (strcmp(current_result->direction, "UP") == 0) {
                    direction_file = FILE_UP;
                } else if (strcmp(current_result->direction, "DOWN") == 0) {
                    direction_file = FILE_DOWN;
                }
                
                if (direction_file) {
                    // delay(200);
                    play_audio_file(direction_file);
                }
            } else {
                // Play "in tune" confirmation
                // delay(200);
                play_audio_file(FILE_IN_TUNE);
            }
            playback_step++;
            break;
            
        default:
            // Playback complete
            is_playing = 0;
            playback_step = 0;
            printf("Audio feedback complete.\n");
            break;
    }
}