/**
 * audio_sequencer.c - Audio sequencing implementation
 *
 * Generates appropriate audio feedback based on tuning results
 * Plays note names, cent values, and tuning directions
 * Implements dynamic beep rate feedback based on tuning accuracy
 */

#include <stdio.h>
#include <string.h>
#include <math.h>
#include "audio_sequencer.h"

static int is_playing = 0;
static const TuningResult* current_result = NULL;
static int playback_step = 0;

/* ============================================================================
 * DYNAMIC BEEP RATE FEEDBACK
 * ========================================================================== */

/** Beep rate configuration: maps cents offset to beep interval (ms) */
typedef struct {
	double max_cents;        /* Threshold for this beep rate */
	uint32_t beep_interval;  /* Time between beeps in milliseconds */
	uint32_t beep_duration;  /* Duration of each beep in milliseconds */
} BeepRateConfig;

/**
 * Beep rate thresholds (in order from worst to best tuning)
 * User plays note → detect frequency → calculate cents offset →
 * Look up beep rate → faster beeps = further from tune
 */
static const BeepRateConfig beep_rates[] = {
	{100.0,  100},   /* > 100 cents off: fastest (100 ms between beeps = 10 beeps/sec) */
	{75.0,   150},   /* 75-100 cents: very fast (150 ms = 6.7 beeps/sec) */
	{50.0,   200},   /* 50-75 cents: fast (200 ms = 5 beeps/sec) */
	{40.0,   300},   /* 40-50 cents: medium-fast (300 ms = 3.3 beeps/sec) */
	{25.0,   500},   /* 25-40 cents: medium (500 ms = 2 beeps/sec) */
	{15.0,   800},   /* 15-25 cents: slow (800 ms = 1.25 beeps/sec) */
	{5.0,    1200},  /* 5-15 cents: very slow (1200 ms = 0.83 beeps/sec) */
	{0.0,    0},     /* < 5 cents: no beep (in tune) */
};

#define NUM_BEEP_RATES (sizeof(beep_rates) / sizeof(BeepRateConfig))

/* State for beep timing */
static uint32_t last_beep_time = 0;
static uint32_t beep_end_time = 0;
static int beeping_active = 0;

/**
 * Calculate beep interval based on cents offset
 * 
 * @param cents_offset: How far the played note is from target (in cents)
 *                      Positive = too high, Negative = too low
 *                      Magnitude indicates error size
 * 
 * @return: Milliseconds between beeps (higher = further from tune)
 *          0 = in tune (no beeping)
 */
uint32_t calculate_beep_interval(double cents_offset) {
	double abs_cents = fabs(cents_offset);
	
	/* Find the appropriate beep rate for this offset */
	for (int i = 0; i < NUM_BEEP_RATES; i++) {
		if (abs_cents >= beep_rates[i].max_cents) {
			return beep_rates[i].beep_interval;
		}
	}
	
	/* Fallback (should not reach here) */
	return 0;
}

/**
 * Generate dynamic beep feedback based on tuning accuracy
 * 
 * Implements the user's requirement:
 * - Further from tune: faster beeping
 * - Closer to tune: slower beeping
 * - In tune (< 5 cents): no beeping
 * 
 * @param result: Current tuning result with cents_offset
 */
void generate_dynamic_beep_feedback(const TuningResult* result) {
	if (result == NULL) {
		beeping_active = 0;
		return;
	}
	
	uint32_t beep_interval = calculate_beep_interval(result->cents_offset);
	
	if (beep_interval == 0) {
		/* In tune - stop beeping */
		beeping_active = 0;
		printf("[BEEP] In tune! No beeping.\n");
	} else {
		beeping_active = 1;
		last_beep_time = 0;  /* Force immediate first beep */
		printf("[BEEP] Starting dynamic beeps at %ld ms interval (offset: %.1f cents)\n", 
		       beep_interval, result->cents_offset);
	}
	
	current_result = result;
}

void audio_sequencer_init(void) {
	printf("Audio sequencer initialized.\n");
	printf("  - Static feedback mode: generate_audio_feedback()\n");
	printf("  - Dynamic beep mode: generate_dynamic_beep_feedback()\n");
	is_playing = 0;
	playback_step = 0;
	beeping_active = 0;
	last_beep_time = 0;
	beep_end_time = 0;
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

/**
 * Update dynamic beep feedback based on current time
 * 
 * Call this frequently (e.g., every 10-50 ms) in your main loop
 * to maintain accurate beep timing.
 * 
 * @param current_time_ms: Current system time in milliseconds
 * 
 * Example usage in main loop:
 *   static uint32_t last_ms = 0;
 *   uint32_t now = millis();
 *   if (now >= last_ms + 10) {
 *       audio_sequencer_update_beeps(now);
 *       last_ms = now;
 *   }
 */
void audio_sequencer_update_beeps(uint32_t current_time_ms) {
	if (!beeping_active || current_result == NULL) {
		return;
	}
	
	uint32_t beep_interval = calculate_beep_interval(current_result->cents_offset);
	
	if (beep_interval == 0) {
		/* Tuning changed to in-tune, stop beeping */
		beeping_active = 0;
		return;
	}
	
	/* Check if it's time for a new beep */
	if (current_time_ms >= last_beep_time + beep_interval) {
		/* Time to beep! */
		printf("[BEEP]\n");  /* Placeholder - actual implementation would call tone/beep hardware */
		last_beep_time = current_time_ms;
		beep_end_time = current_time_ms + 50;  /* Beep duration: 50 ms */
	}
}
