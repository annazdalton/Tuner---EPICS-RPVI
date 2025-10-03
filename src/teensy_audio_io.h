#define TEENSY_AUDIO_IO_H

//libraries
#include <Audio.h> 
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

// Teensy-specific configuration
#define AUDIO_BLOCK_SIZE 128 //this is teensy specific block size -- verify that this is correct !!!!
#define FFT_SIZE 512 //this is specific for real time FFT - verify that this is correct !!!!!
#define AUDIO_SAMPLE_RATE 44100 //check that this is correct
#define SD_CHIP_SELECT 
#define BUILTIN_SDCARD

//audio streaming structure
typedef struct {
    File audio_file;
    bool is_playing;
    uint32_t file_size;
    uint32_t bytes_read;
    int16_t buffer[AUDIO_BLOCK_SIZE];
    float fft_buffer[FFT_SIZE];
} teensy_audio_stream_t;

//error codes 
typedef enum {
    TEENSY_AUDIO_OK = 0,
    TEENSY_AUDIO_ERROR = -1,
    TEENSY_AUDIO_NO_SD = -2,
    TEENSY_AUDIO_FILE_ERROR = -3
} teensy_audio_error_t;

// Function declarations
teensy_audio_error_t init_audio_system(void);
teensy_audio_error_t open_audio_file(teensy_audio_stream_t* stream, const char* filename);
teensy_audio_error_t read_audio_block(teensy_audio_stream_t* stream, float* output);
void close_audio_file(teensy_audio_stream_t* stream);
void get_fft_data(float* fft_output, int num_bins);
teensy_audio_error_t play_audio_file(const char* filename);
void process_audio_realtime(void);
bool is_audio_playing(void);
void stop_audio_playback(void);
void set_volume(float vol);
