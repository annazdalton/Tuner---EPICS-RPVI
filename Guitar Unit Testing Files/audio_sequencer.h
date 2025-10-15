/**
 * audio_sequencer.h - Audio feedback sequencing
 * 
 * Controls playback of WAV files for user feedback
 * Manages the sequence of audio messages
 */

#ifndef AUDIO_SEQUENCER_H
#define AUDIO_SEQUENCER_H

#include "string_detection.h"

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

// Function prototypes
void audio_sequencer_init(void);
void generate_audio_feedback(const TuningResult* result);
void audio_sequencer_update(void);
void play_audio_file(const char* filename);

#endif