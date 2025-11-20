//noise filtering functions
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

// insertion sort (for median filter)
float insertion_sort(float *buffer, int size) {
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
void median_filter(const float *input, float *output, int length, int window) {


}
