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

static int is_playing = 0;
static const TuningResult* current_result = NULL;
static int playback_step = 0;

void audio_sequencer_init(void) {
	printf("Audio sequencer initialized.\n");
	is_playing = 0;
	playback_step = 0;
}

void play_audio_file(const char* filename) {
	printf("[AUDIO] Playing: %s\n", filename);
}

const char* get_string_filename(int string_num) {
	switch (string_num) {
		case 1: return FILE_E;
		case 2: return FILE_B;
		case 3: return FILE_G;
		case 4: return FILE_D;
		case 5: return FILE_A;
		case 6: return FILE_E;
		default: return NULL;
	}
}

const char* get_cents_filename(double cents) {
	double abs_cents = fabs(cents);
	if (abs_cents < 5) {
		return NULL;
	} else if (abs_cents < 15) {
		return FILE_10_CENTS;
	} else if (abs_cents < 25) {
		return FILE_20_CENTS;
	} else {
		return FILE_20_CENTS;
	}
}

void generate_audio_feedback(const TuningResult* result) {
	printf("Generating audio feedback...\n");
	current_result = result;
	is_playing = 1;
	playback_step = 0;
}

void audio_sequencer_update(void) {
	if (!is_playing || current_result == NULL) {
		return;
	}
	switch (playback_step) {
		case 0: {
			const char* string_file = get_string_filename(current_result->detected_string);
			if (string_file) {
				play_audio_file(string_file);
			}
			playback_step++;
			break;
		}
		case 1:
			if (strcmp(current_result->direction, "IN_TUNE") != 0) {
				const char* cents_file = get_cents_filename(current_result->cents_offset);
				if (cents_file) {
					play_audio_file(cents_file);
				}
			}
			playback_step++;
			break;
		case 2:
			if (strcmp(current_result->direction, "IN_TUNE") != 0) {
				const char* direction_file = NULL;
				if (strcmp(current_result->direction, "UP") == 0) {
					direction_file = FILE_UP;
				} else if (strcmp(current_result->direction, "DOWN") == 0) {
					direction_file = FILE_DOWN;
				}
				if (direction_file) {
					play_audio_file(direction_file);
				}
			} else {
				play_audio_file(FILE_IN_TUNE);
			}
			playback_step++;
			break;
		default:
			is_playing = 0;
			playback_step = 0;
			printf("Audio feedback complete.\n");
			break;
	}
}
