//noise filtering functions
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

// insertion sort (for median filter)
float find_median(float *buffer, int size) {
    for (int i = 1; i < size; i++) {
        float key = buffer[i];
        int j = i - 1;
        
        while (j >= 0 && buffer[j] > key) {
            buffer[j + 1] = buffer[j];
            j--;
        }
        buffer[j + 1] = key;
    }

    //return median for median sort
    if (size % 2 == 0) {
        return (buffer[size/2 - 1] + buffer[size/2]) / 2.0f;
    } else {
        return buffer[size/2];
    }
}


// MEDIAN FILTER
// window size: smaller window size - less smoothing of data, bigger window size - more smoothing of data
void median_filter(const float *input, float *output, int length, int window_size) {
    if (window_size % 2 == 0) {
        window_size++; 
    }

    int half_window =  window_size / 2;

    float *window_buffer = (float *)malloc(window_size * sizeof(float));
    if(!window_buffer) {
        fprintf(stderr, "Memory allocation failed\n");
    }

    //iterate through every element of audio data array
    for (int i = 0; i < length; i++) {
        int actual_window_size = 0; 

        //stores neighboring data points of current index in window buffer
        for (int j = -half_window; j <= half_window; j++) {
            int index = i + j;
            
            if (index >= 0 && index < length) {
                window_buffer[actual_window_size++] = input[index];
            }
        }
        output[i] = find_median(window_buffer, actual_window_size);
    }
}
