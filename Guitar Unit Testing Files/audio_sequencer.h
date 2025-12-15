/**
 * audio_sequencer.h - Audio feedback sequencing
 * 
 * Controls playback of WAV files for user feedback
 * Manages the sequence of audio messages
 * Implements dynamic beep rate feedback based on tuning accuracy
 */

#ifndef AUDIO_SEQUENCER_H
#define AUDIO_SEQUENCER_H

#include "string_detection.h"
#include <stdint.h>

// Audio file names (these would be actual WAV files on SD card)
#define FILE_E "E.WAV"
#define FILE_A "A.WAV" 
#define FILE_D "D.WAV"
#define FILE_G "G.WAV"
#define FILE_B "B.WAV"
#define FILE_UP "UP.WAV"
#define FILE_DOWN "DOWN.WAV"
#define FILE_IN_TUNE "IN_TUNE.WAV"
#define FILE_10_CENTS "10CENTS.WAV"
#define FILE_20_CENTS "20CENTS.WAV"

/* ============================================================================
 * STATIC FEEDBACK MODE (Original Implementation)
 * ========================================================================== */

/**
 * Initialize audio sequencer
 * Supports both static feedback and dynamic beep modes
 */
void audio_sequencer_init(void);

/**
 * Generate audio feedback using pre-recorded files
 * Plays: "[String Name] [Cents] [Direction]" sequence
 * Example: "E ... Ten Cents ... UP"
 */
void generate_audio_feedback(const TuningResult* result);

/**
 * Update static audio playback state
 * Call this regularly to advance through audio file playback
 */
void audio_sequencer_update(void);

void play_audio_file(const char* filename);

/* ============================================================================
 * DYNAMIC BEEP FEEDBACK MODE (New Implementation)
 * ========================================================================== */

/**
 * Generate dynamic beep feedback based on tuning accuracy
 * 
 * Implements continuous beeping with rate proportional to tuning error:
 * - Further from tune (> 100 cents): Fast beeps (100 ms interval = 10 beeps/sec)
 * - Moderately off (50-100 cents): Medium-fast beeps (200-300 ms)
 * - Close to tune (15-50 cents): Slow beeps (500-800 ms)
 * - In tune (< 5 cents): No beeping
 * 
 * This provides real-time continuous feedback as the user tunes.
 * 
 * @param result: Current tuning result containing cents_offset
 * 
 * Usage:
 *   1. User plays a string
 *   2. FFT detects frequency and calculates cents offset
 *   3. Call: generate_dynamic_beep_feedback(&result)
 *   4. Call audio_sequencer_update_beeps() regularly in main loop
 *   5. Beeps play at rate based on how far user is from target
 *   6. As user adjusts pitch, beep rate automatically changes
 */
void generate_dynamic_beep_feedback(const TuningResult* result);

/**
 * Update dynamic beep timing
 * 
 * Call this frequently (10-50 ms intervals) in your main loop to maintain
 * accurate beep timing. It will emit beeps at the calculated rate based on
 * the current cents offset.
 * 
 * @param current_time_ms: Current system time in milliseconds (from millis())
 * 
 * Example in main loop:
 * ```
 *   void loop() {
 *       // ... other code ...
 *       audio_sequencer_update_beeps(millis());
 *   }
 * ```
 * 
 * Beep rate mapping (cents offset → beep interval):
 * - 100+ cents off: 100 ms between beeps (fastest feedback)
 * - 75-100 cents: 150 ms
 * - 50-75 cents: 200 ms  
 * - 40-50 cents: 300 ms
 * - 25-40 cents: 500 ms (medium)
 * - 15-25 cents: 800 ms
 * - 5-15 cents: 1200 ms (slowest beeping)
 * - <5 cents: No beeps (in tune!)
 */
void audio_sequencer_update_beeps(uint32_t current_time_ms);

/**
 * Calculate beep interval based on cents offset
 * 
 * Internal function - maps tuning error magnitude to beep frequency.
 * Used by generate_dynamic_beep_feedback() and audio_sequencer_update_beeps().
 * 
 * @param cents_offset: How far from target note (positive = too high, negative = too low)
 * @return: Milliseconds between beeps (0 = in tune, no beeping)
 */
uint32_t calculate_beep_interval(double cents_offset);

#endif