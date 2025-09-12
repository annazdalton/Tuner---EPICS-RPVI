import math
import numpy as np
from scipy import signal # signal processing library
import matplotlib.pyplot as plt
from scipy.io import wavfile
from scipy.signal import butter, lfilter


# Following is not actual code, just a testing the signal processing ability

"""
#Noisy Sine Wave Signal Generation and Filtering

"""
# Parameters
fs = 1000 # Sampling frequency
t = np.arange(0, 1, 1/fs) # Time vector
frequency = 5 # Frequency of the sine wave
sine_wave = np.sin(2 * np.pi * frequency * t)
# Adding noise

noise = np.random.normal(0, 0.5, sine_wave.shape)
noisy_signal = sine_wave + noise

# Designing a low-pass Butterworth filter
cutoff_frequency = 10 # Cutoff frequency in Hz
# Designing a low-pass Butterworth filter
cutoff_frequency = 10 # Cutoff frequency in Hz
b, a = signal.butter(4, cutoff_frequency / (0.5 * fs), btype="low")

# Applying the filter
filtered_signal = signal.filtfilt(b, a, noisy_signal)

# Performing FFT
frequencies = np.fft.fftfreq(len(t), 1/fs)
fft_original = np.fft.fft(sine_wave)
fft_filtered = np.fft.fft(filtered_signal)

# Plot all 4 in one window
plt.figure(figsize=(12, 10))

# 1. Original Sine Wave
plt.subplot(2, 2, 1)
plt.plot(t, sine_wave)
plt.title("Original Sine Wave Signal")
plt.xlabel("Time [s]")
plt.ylabel("Amplitude")
plt.grid()

# 2. Noisy Sine Wave
plt.subplot(2, 2, 2)
plt.plot(t, noisy_signal)
plt.title("Noisy Sine Wave Signal")
plt.xlabel("Time [s]")
plt.ylabel("Amplitude")
plt.grid()

# 3. Filtered Sine Wave
plt.subplot(2, 2, 3)
plt.plot(t, filtered_signal, label="Filtered Signal", color="red")
plt.plot(t, noisy_signal, label="Noisy Signal", color="gray", alpha=0.5)
plt.title("Filtered Sine Wave Signal")
plt.xlabel("Time [s]")
plt.ylabel("Amplitude")
plt.legend()
plt.grid()

# 4. Frequency Spectrum
plt.subplot(2, 2, 4)
plt.plot(frequencies[:len(frequencies)//2], np.abs(fft_original)[:len(frequencies)//2], label="Original Signal")
plt.plot(frequencies[:len(frequencies)//2], np.abs(fft_filtered)[:len(frequencies)//2], label="Filtered Signal", color="red")
plt.title("Frequency Spectrum")
plt.xlabel("Frequency [Hz]")
plt.ylabel("Magnitude")
plt.legend()
plt.grid()

plt.tight_layout()
plt.show()

"""
 Audio File Filtering Example with System of a Down - Vicinity of Obscenity
"""

sample_rate, data = wavfile.read("system of a down-vicinity of obscenity.wav") # Replace with your audio file path

def butter_lowpass(cutoff, fs, order=5):
    nyq = 0.5 * fs
    normal_cutoff = cutoff / nyq
    b, a = butter(order, normal_cutoff, btype="low", analog=False)
    return b, a

def lowpass_filter(data, cutoff, fs, order=5):

    b, a = butter_lowpass(cutoff, fs, order=order)

    y = lfilter(b, a, data)

    return y

cutoff_frequency = 3000 # Set cutoff frequency to 3000 Hz

filtered_data = lowpass_filter(data, cutoff_frequency, sample_rate)
wavfile.write("system of a down-vicinity of obscenity.wav", sample_rate, filtered_data.astype(np.int16))

plt.figure(figsize=(12, 6))

plt.subplot(2, 1, 1)

plt.plot(data)

plt.title("Original Signal")

plt.subplot(2, 1, 2)

plt.plot(filtered_data)

plt.title("Filtered Signal")

plt.tight_layout()

plt.show()