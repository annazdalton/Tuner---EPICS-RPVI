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


// -------- initalizes hardware and software needed for audio --------
// main parts of function:
// - SD card initalization (returns error if SD card isn't inserted or is corrupted/has issues)
// - audio memory allocation 
teensy_audio_error_t init_audio_system(void) 
{
    //set up SPI for accessing SD card -- pins 11 and 13 on Teensy 4.1
    SPI.setMOSI(11);  
    SPI.setSCK(13);

    if (!SD.begin(SD_CHIP_SELECT)) 
    {
        Serial.println("SD card initialization failed!");
        Serial.println("Check that SD card is inserted.");
        return TEENSY_AUDIO_NO_SD;
    } else {
        Serial.println("SD card initialized successfully.");
    }
   
    //initalize audio memory blocks - allocated 20 blocks in RAM
    //each block is 128 samples (~2.9ms at 44.1kHz). 
    //more blocks means code can handle more complex audio chains but uses more RAM
    AudioMemory(20); 
    sgtl5000.enable();
    sgtl5000.volume(0.5); 
    sgtl5000.inputSelect(AUDIO_INPUT_LINEIN);  //this can be switched to input mic if we want that
    Serial.println("Audio codec enabled");
    
    Serial.println("Audio system initialized");
    return TEENSY_AUDIO_OK;
}

//---------opens audio file (on SD card)--------------
//pretty self explanatory, it returns error if it fails
teensy_audio_error_t open_audio_file(teensy_audio_stream_t* stream, const char* filename) {
    //open file thats stored on SD card
    stream->audio_file = SD.open(filename);

    if (!stream->audio_file) {
        Serial.print("ERROR: Cannot open file: ");
        Serial.println(filename);
        return TEENSY_AUDIO_FILE_ERROR;
    }
    
    stream->file_size = stream->audio_file.size();
    stream->bytes_read = 0;
    stream->is_playing = true;
    
    Serial.print("Opened audio file: ");
    Serial.print(filename);
    Serial.print(", size: ");
    Serial.println(stream->file_size);
    Serial.println(" bytes");
    
    return TEENSY_AUDIO_OK;
}

//reads block of audio samples from audio file
teensy_audio_error_t read_audio_block(teensy_audio_stream_t* stream, float* output) {
    if (!stream->is_playing || !stream->audio_file.available()) {
        stream->is_playing = false;
        return TEENSY_AUDIO_ERROR;
    }
    
    //read raw audio data, this assumes 16-bit WAV but can be changed
    int bytes_to_read = AUDIO_BLOCK_SIZE * 2; //2 bytes per 16-bit sample
    int bytes_read = stream->audio_file.read((uint8_t*)stream->buffer, bytes_to_read);
    
    if (bytes_read <= 0) {
        stream->is_playing = false;
        return TEENSY_AUDIO_ERROR;
    }
    
    //convert 16-bit samples (-32768 to +32767) to float (-1.0, 1.0)
    int samples_read = bytes_read / 2;
    for (int i = 0; i < samples_read; i++) {
        output[i] = stream->buffer[i] / 32768.0f;
    }
    
    //if it gets to the end of the file and the block is less an 16-bits, fill end with 0s
    for (int i = samples_read; i < AUDIO_BLOCK_SIZE; i++) {
        output[i] = 0.0f;
    }
    
    stream->bytes_read += bytes_read;
    
    //print progress every 10% -- also an error check if it isnt loading
    static uint32_t last_progress = 0;
    uint32_t progress = (stream->bytes_read * 100) / stream->file_size;
    if (progress >= last_progress + 10) {
        Serial.print("Progress: ");
        Serial.print(progress);
        Serial.println("%");
        last_progress = progress;
    }
    
    return TEENSY_AUDIO_OK;
}

//---------closes audio file----------
//self explanatory
void close_audio_file(teensy_audio_stream_t* stream) {
    if (stream->audio_file) {
        stream->audio_file.close();
        stream->is_playing = false;
        Serial.println("Audio file closed");
    }
}

//-------------gets data output from fft------------------
//copies FFT results from the Teensy Audio Library into array
//fft updates contstantly, this prevents it from being overwritten
void get_fft_data(float* fft_output, int num_bins) {
    //checks if new data is available - fft runs continously but needs abt 23 ms to finish processing
    if (fft.available()) {
        //teensy FFT provides 256 bins for FFT512
        int max_bins = (num_bins < 256) ? num_bins : 256;
        
        for (int i = 0; i < max_bins; i++) {
            fft_output[i] = fft.read(i);
        }
        
        //zero remaining bins if requested more than available
        for (int i = max_bins; i < num_bins; i++) {
            fft_output[i] = 0.0f;
        }
    }
}

//--------plays audio file----------
teensy_audio_error_t play_audio_file(const char* filename) {
    //stop any currently playing audio
    if (playWav.isPlaying()) {
        playWav.stop();
        delay(50); 
    }
    
    // Start playing new file
    bool result = playWav.play(filename);
    if (!result) {
        Serial.print("ERROR: Could not play file: ");
        Serial.println(filename);
        return TEENSY_AUDIO_FILE_ERROR;
    }
    
    Serial.print("Now playing: ");
    Serial.println(filename);
    
    delay(10);
    
    return TEENSY_AUDIO_OK;
}

//------simple audio playing check - for main fn (prevents audio being cutoff)--------
bool is_audio_playing(void) {
    return playWav.isPlaying();
}

void stop_audio_playback(void) {
    if (playWav.isPlaying()) {
        playWav.stop();
        Serial.println("Playback stopped");
    }
}

void set_volume(float vol) {
    //keeps audio range 0.0-1.0
    if (vol < 0.0f) vol = 0.0f;
    if (vol > 1.0f) vol = 1.0f;
    
    sgtl5000.volume(vol);
    Serial.print("Volume set to: ");
    Serial.println(vol);
}

//real-time audio processing 
void process_audio_realtime(void) {
    static float fft_data[256];
    static uint32_t last_print = 0;
    
    if (fft.available()) {
        get_fft_data(fft_data, 256);
        
        //find dominant frequency
        float max_magnitude = 0.0f;
        int dominant_bin = 0;
        
        //skip DC bin (0) and very low frequencies
        for (int i = 5; i < 128; i++) {
            if (fft_data[i] > max_magnitude) {
                max_magnitude = fft_data[i];
                dominant_bin = i;
            }
        }
        
        //convert bin to frequency
        //each bin represents: (sample_rate / FFT_size) Hz
        float bin_width = AUDIO_SAMPLE_RATE / (float)FFT_SIZE;
        float dominant_freq = dominant_bin * bin_width;
        
        //print every 200ms to avoid flooding serial
        if (millis() - last_print > 200) {
            Serial.print("Dominant frequency: ");
            Serial.print(dominant_freq, 1);
            Serial.print(" Hz, Magnitude: ");
            Serial.println(max_magnitude, 4);
            last_print = millis();
        }
        
        //add more stuff to improve audio processing
    }
}

//function to list files on SD card
void list_audio_files(void) {
    Serial.println("\nAudio files on SD card:");
    File root = SD.open("/");
    
    while (true) {
        File entry = root.openNextFile();
        if (!entry) break;
        
        if (!entry.isDirectory()) {
            String filename = entry.name();
            if (filename.endsWith(".wav") || filename.endsWith(".WAV")) {
                Serial.print("  - ");
                Serial.print(filename);
                Serial.print(" (");
                Serial.print(entry.size());
                Serial.println(" bytes)");
            }
        }
        entry.close();
    }
    root.close();
    Serial.println();
}