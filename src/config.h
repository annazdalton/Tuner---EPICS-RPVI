/**
 * config.h - Hardware configuration for Teensy 4.1 Guitar Tuner
 *
 * Defines GPIO pins, audio configuration, and hardware-specific constants
 * for the Teensy 4.1 microcontroller implementation.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ============================================================================
 * STRING SELECTION BUTTONS
 * Maps guitar strings to physical tactile buttons
 * String 1 (E4) through String 6 (E2)
 * ========================================================================== */

#define STRING_1_BUTTON_PIN 2   // E4 string (high E)
#define STRING_2_BUTTON_PIN 3   // B3 string
#define STRING_3_BUTTON_PIN 4   // G3 string
#define STRING_4_BUTTON_PIN 5   // D3 string
#define STRING_5_BUTTON_PIN 6   // A2 string
#define STRING_6_BUTTON_PIN 7   // E2 string (low E)

/* ============================================================================
 * CHROMATIC NOTE SELECTION (Alternative to string-only buttons)
 * Optional: For users who want to select sharps/flats
 * ========================================================================== */

#define CHROMATIC_BUTTON_BASE_PIN 8     // Starting pin for chromatic buttons
#define NUM_CHROMATIC_BUTTONS 12        // C, C#, D, D#, E, F, F#, G, G#, A, A#, B

/* ============================================================================
 * VOLUME CONTROL (Rotary Potentiometer or Encoder)
 * ========================================================================== */

#define VOLUME_POTENTIOMETER_PIN 14     // ADC pin for volume knob
#define VOLUME_ADC_RESOLUTION 12        // 12-bit ADC = 4096 levels
#define VOLUME_UPDATE_INTERVAL_MS 50    // Update volume every 50ms

/* Alternative: Rotary Encoder with Click */
#define ROTARY_ENCODER_CLK_PIN 15       // Rotation phase A
#define ROTARY_ENCODER_DT_PIN  16       // Rotation phase B
#define ROTARY_ENCODER_SW_PIN  17       // Push button (click)

/* ============================================================================
 * AUDIO HARDWARE
 * Teensy 4.1 I²S interface for stereo audio output
 * ========================================================================== */

/* I²S Audio Output Pins (fixed on Teensy 4.1) */
#define AUDIO_I2S_BCLK_PIN  20  // Bit Clock
#define AUDIO_I2S_LRCLK_PIN 21  // Left/Right Clock (frame sync)
#define AUDIO_I2S_OUT_PIN   22  // Serial Data Out

/* Optional: Power amplifier enable/shutdown pin */
#define AUDIO_AMP_ENABLE_PIN 23 // GPIO to enable external amplifier

/* ============================================================================
 * AUDIO CONFIGURATION
 * ========================================================================== */

#define AUDIO_SAMPLE_RATE       44100   // Professional quality (Hz)
#define AUDIO_BLOCK_SIZE        128     // ~2.9ms per block at 44.1kHz
#define AUDIO_BIT_DEPTH         16      // 16-bit PCM audio
#define AUDIO_CHANNELS          2       // Stereo (both speaker & headphone)

/* Volume control parameters */
#define VOLUME_MIN              0.0f    // Silent
#define VOLUME_MAX              1.0f    // Full volume
#define VOLUME_DEFAULT          0.7f    // Recommended startup volume

/* ============================================================================
 * SD CARD / AUDIO FILE STORAGE
 * ========================================================================== */

#define SD_CHIP_SELECT_PIN  10  // SPI chip select for SD card
#define AUDIO_FILES_PATH    "/AUDIO/"
#define MAX_FILENAME_LENGTH 256

/* Pre-recorded audio file names */
#define AUDIO_FILE_E_STRING     "/AUDIO/STRING_E.wav"
#define AUDIO_FILE_B_STRING     "/AUDIO/STRING_B.wav"
#define AUDIO_FILE_G_STRING     "/AUDIO/STRING_G.wav"
#define AUDIO_FILE_D_STRING     "/AUDIO/STRING_D.wav"
#define AUDIO_FILE_A_STRING     "/AUDIO/STRING_A.wav"

#define AUDIO_FILE_10_CENTS     "/AUDIO/CENTS_10.wav"
#define AUDIO_FILE_20_CENTS     "/AUDIO/CENTS_20.wav"

#define AUDIO_FILE_TUNE_UP      "/AUDIO/TUNE_UP.wav"
#define AUDIO_FILE_TUNE_DOWN    "/AUDIO/TUNE_DOWN.wav"
#define AUDIO_FILE_IN_TUNE      "/AUDIO/IN_TUNE.wav"

/* ============================================================================
 * DIGITAL SIGNAL PROCESSING (FFT)
 * ========================================================================== */

#define FFT_SIZE                256     // 256-point FFT
#define FFT_INPUT_SAMPLE_RATE   10000   // Downsampled to 10kHz for guitar tuning
#define FFT_HZ_PER_BIN          (FFT_INPUT_SAMPLE_RATE / FFT_SIZE)  // ~39 Hz resolution

/* Frequency detection range (Hz) - covers all guitar strings */
#define MIN_DETECTABLE_FREQ     50.0f   // Below low E (82.41 Hz)
#define MAX_DETECTABLE_FREQ     400.0f  // Well above high E (329.63 Hz)

/* ============================================================================
 * TUNING PARAMETERS
 * ========================================================================== */

#define TUNING_TOLERANCE_CENTS  2.0     // ±2 cents considered "in tune"
#define CENTS_THRESHOLD_WARN    10.0    // Warn if > 10 cents off
#define CENTS_THRESHOLD_CRITICAL 50.0   // Critical if > 50 cents off

/* ============================================================================
 * MICROPHONE INPUT
 * ========================================================================== */

#define MICROPHONE_INPUT_PIN    24      // ADC pin for microphone
#define MICROPHONE_SAMPLE_RATE  44100   // 44.1 kHz recording rate
#define MICROPHONE_GAIN         20      // dB gain amplification

/* ============================================================================
 * TEENSY HARDWARE SPECIFICS
 * ========================================================================== */

#define TEENSY_VERSION          41      // Teensy 4.1
#define TEENSY_CLOCK_SPEED      600     // MHz
#define TEENSY_RAM_KB           1024    // 1 MB RAM available

/* Temporary buffer sizes for audio processing */
#define AUDIO_BUFFER_SIZE       4096    // 4K sample buffer
#define FFT_BUFFER_SIZE         FFT_SIZE * sizeof(float)

/* ============================================================================
 * DEBUG & DIAGNOSTICS
 * ========================================================================== */

#define ENABLE_DEBUG_PRINTS     1       // Set to 0 to disable console output
#define SERIAL_BAUD_RATE        115200  // USB serial monitor speed
#define DEBUG_FFT_OUTPUT        0       // Print FFT magnitude spectrum
#define DEBUG_TUNING_RESULTS    1       // Print tuning analysis results

#ifdef __cplusplus
}
#endif

#endif /* CONFIG_H */
