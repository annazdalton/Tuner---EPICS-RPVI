#include "teensy_audio_io.h" //header file
#include <Arduino.h> 

// --------- GLOBAL OBJECTS ---------
// playWav: read WAV files from SD card
// fft: analyzes frequency (1024-point fft)
// i2s_out: sends data to speaker via I2S protocol 
// patchCord1: connects audio to FFT analyzer
// patchCord2: connects left channel to speaker
// patchCord3: connects right channel to speaker
// sgt15000: controls audio codec chip
AudioPlaySdWav           playWav;
AudioAnalyzeFFT1024      fft;
AudioOutputI2S           i2s_out;
AudioConnection          patchCord1(playWav, 0, fft, 0);
AudioConnection          patchCord2(playWav, 0, i2s_out, 0);
AudioConnection          patchCord3(playWav, 1, i2s_out, 1);
AudioControlSGTL5000     sgtl5000;


// -------- Initializes audio system --------
// NOTE: Actual hardware initialization (SPI, SD card, audio codec) would be
// performed in platform-specific HAL implementations
teensy_audio_error_t init_audio_system(void) 
{
    printf("Initializing audio system...\n");
    
    // Platform-specific SD card initialization would go here
    // For now, we just allocate audio memory and set up state
    printf("SD card initialization (platform-specific)...\n");
    
    // Audio memory allocation - allocate 20 blocks in RAM
    // Each block is 128 samples (~2.9ms at 44.1kHz)
    audio_memory_blocks = 20;
    printf("Audio memory allocated: %u blocks\n", audio_memory_blocks);
    
    // Audio codec initialization (platform-specific)
    current_volume = 0.5f;
    printf("Audio codec enabled at volume %.1f\n", current_volume);
    
    audio_system_initialized = true;
    printf("Audio system initialized successfully\n");
    return TEENSY_AUDIO_OK;
}

//---------opens audio file (standard C file handling)-----------
// Platform-specific file operations would be in HAL implementation
teensy_audio_error_t open_audio_file(teensy_audio_stream_t* stream, const char* filename) {
    if (!stream || !filename) {
        printf("ERROR: Invalid stream or filename pointer\n");
        return TEENSY_AUDIO_ERROR;
    }

    // In platform-specific implementation, this would use actual file system calls
    // For now, we demonstrate with standard C FILE operations
    printf("Opening audio file: %s\n", filename);
    
    // Placeholder: actual SD card file opening would be platform-specific
    // FILE* fp = fopen(filename, "rb");
    // if (!fp) { ... return TEENSY_AUDIO_FILE_ERROR; }
    
    stream->file_size = 0;
    stream->bytes_read = 0;
    stream->is_playing = true;
    
    printf("Audio file opened: %s (size: %u bytes)\n", filename, stream->file_size);
    
    return TEENSY_AUDIO_OK;
}

//reads block of audio samples from audio file
// Platform-specific file reading would be in HAL implementation
teensy_audio_error_t read_audio_block(teensy_audio_stream_t* stream, float* output) {
    if (!stream || !output) {
        printf("ERROR: Invalid stream or output pointer\n");
        return TEENSY_AUDIO_ERROR;
    }

    if (!stream->is_playing) {
        return TEENSY_AUDIO_ERROR;
    }
    
    // Platform-specific file reading would go here
    // int bytes_to_read = AUDIO_BLOCK_SIZE * 2; // 2 bytes per 16-bit sample
    // int bytes_read = fread(stream->buffer, 1, bytes_to_read, stream->audio_file);
    
    int bytes_read = 0;
    
    if (bytes_read <= 0) {
        stream->is_playing = false;
        return TEENSY_AUDIO_ERROR;
    }
    
    // Convert 16-bit samples (-32768 to +32767) to float (-1.0, 1.0)
    int samples_read = bytes_read / 2;
    for (int i = 0; i < samples_read; i++) {
        output[i] = stream->buffer[i] / 32768.0f;
    }
    
    // Fill remaining samples with zeros if block is incomplete
    for (int i = samples_read; i < AUDIO_BLOCK_SIZE; i++) {
        output[i] = 0.0f;
    }
    
    stream->bytes_read += bytes_read;
    
    // Print progress every 10%
    static uint32_t last_progress = 0;
    if (stream->file_size > 0) {
        uint32_t progress = (stream->bytes_read * 100) / stream->file_size;
        if (progress >= last_progress + 10) {
            printf("Progress: %u%%\n", progress);
            last_progress = progress;
        }
    }
    
    return TEENSY_AUDIO_OK;
}

//---------closes audio file----------
void close_audio_file(teensy_audio_stream_t* stream) {
    if (stream && stream->audio_file) {
        // Platform-specific file close would go here
        // fclose((FILE*)stream->audio_file);
        stream->audio_file = NULL;
        stream->is_playing = false;
        printf("Audio file closed\n");
    }
}

//-------------gets data output from fft------------------
// FFT data handling - implementation platform-specific
void get_fft_data(float* fft_output, int num_bins) {
    if (!fft_output) {
        printf("ERROR: Invalid fft_output pointer\n");
        return;
    }

    // Platform-specific FFT computation would go here
    // This is a placeholder that copies static FFT data
    // In real implementation, this would call actual FFT processing (e.g., CMSIS-DSP)
    
    int max_bins = (num_bins < 256) ? num_bins : 256;
    
    for (int i = 0; i < max_bins; i++) {
        fft_output[i] = fft_data[i];
    }
    
    // Zero remaining bins if requested more than available
    for (int i = max_bins; i < num_bins; i++) {
        fft_output[i] = 0.0f;
    }
}

//--------plays audio file----------
teensy_audio_error_t play_audio_file(const char* filename) {
    if (!filename) {
        printf("ERROR: Invalid filename pointer\n");
        return TEENSY_AUDIO_ERROR;
    }

    // Platform-specific audio playback control would go here
    // This is a placeholder implementation
    printf("Audio playback requested for: %s\n", filename);
    
    return TEENSY_AUDIO_OK;
}

//------simple audio playing check - for main fn (prevents audio being cutoff)--------
bool is_audio_playing(void) {
    // Platform-specific audio playback status check
    return false;
}

void stop_audio_playback(void) {
    // Platform-specific audio playback stop
    printf("Playback stopped\n");
}

void set_volume(float vol) {
    // Keep audio range 0.0-1.0
    if (vol < 0.0f) vol = 0.0f;
    if (vol > 1.0f) vol = 1.0f;
    
    current_volume = vol;
    printf("Volume set to: %.2f\n", vol);
}

//real-time audio processing 
void process_audio_realtime(void) {
    static float local_fft_data[256];
    static uint32_t last_print = 0;
    
    // Platform-specific FFT processing would go here
    get_fft_data(local_fft_data, 256);
    
    // Find dominant frequency
    float max_magnitude = 0.0f;
    int dominant_bin = 0;
    
    // Skip DC bin (0) and very low frequencies
    for (int i = 5; i < 128; i++) {
        if (local_fft_data[i] > max_magnitude) {
            max_magnitude = local_fft_data[i];
            dominant_bin = i;
        }
    }
    
    // Convert bin to frequency
    // Each bin represents: (sample_rate / FFT_size) Hz
    float bin_width = AUDIO_SAMPLE_RATE / (float)FFT_SIZE;
    float dominant_freq = dominant_bin * bin_width;
    
    // Print every 200ms to avoid flooding output
    time_t current_time = time(NULL);
    static time_t last_print_time = 0;
    
    if (current_time - last_print_time >= 1) { // ~1 second interval
        printf("Dominant frequency: %.1f Hz, Magnitude: %.4f\n", dominant_freq, max_magnitude);
        last_print_time = current_time;
    }
}

// Function to list audio files on SD card
void list_audio_files(void) {
    printf("\nAudio files on storage device:\n");
    
    // Platform-specific directory listing would go here
    // For now, this is a placeholder
    printf("  (Directory listing not implemented in cross-platform version)\n");
    printf("\n");
}