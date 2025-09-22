#include "teensy_audio_io_h"

//add more setup stuff for file

teensy_audio_error_t init_audio_system(void) 
{
    if (!SD.begin(SD_CHIP_SELECT)) 
    {
        Serial.println("SD card initialization failed!");
        return TEENSY_AUDIO_NO_SD;
    }
   
    AudioMemory(20); //setup audio memory
    sgtl5000.enable();
    sgtl5000.volume(0.5); 
    
    Serial.println("Audio system initialized");
    return TEENSY_AUDIO_OK;
}


teensy_audio_error_t open_audio_file(teensy_audio_stream_t* stream, const char* filename) {
    stream->audio_file = SD.open(filename);
    if (!stream->audio_file) {
        Serial.print("Cannot open file: ");
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
    
    return TEENSY_AUDIO_OK;
}
