#include <math.h>
#include <fftw3.h>
#include <stdbool.h>

#define SAMPLE_SIZE //add
#define SAMPLING_FREQ //add 

//user inputs

int user_volume; //user volume selection
int feedback_button; //if correct note audio should be played, 1- yes, 0 - no

//local variables
bool note_feedback;
char user_note; 

void setup(int user_volume, int feedback_button) 
{
    //user selects which note they want to play
    

    //user selects if they want correct note audio to play
    if (feedback_button) {
        note_feedback = true;
    } else {
        note_feedback = false; 
    }
}


