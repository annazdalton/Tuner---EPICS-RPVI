import math
import numpy as np
from scipy import signal # signal processing library
import matplotlib.pyplot as plt

# Following is not actual code, just testing the signal processing ability
# I'll take it out once it's done


# Parameters
fs = 1000 # Sampling frequency
t = np.arange(0, 1, 1/fs) # Time vector
frequency = 5 # Frequency of the sine wave
# Creating a sine wave
signal = np.sin(2 * np.pi * frequency * t)
# Plotting the signal
plt.plot(t, signal)
plt.title("Original Sine Wave Signal")
plt.xlabel("Time [s]")
plt.ylabel("Amplitude")
plt.grid()
plt.show()

# Adding noise

noise = np.random.normal(0, 0.5, signal.shape)
noisy_signal = signal + noise

# Plotting the noisy signal

plt.plot(t, noisy_signal)
plt.title("Noisy Sine Wave Signal")
plt.xlabel("Time [s]")
plt.ylabel("Amplitude")
plt.grid()
plt.show()

# Designing a low-pass Butterworth filter
cutoff_frequency = 10 # Cutoff frequency in Hz
# Designing a low-pass Butterworth filter
cutoff_frequency = 10 # Cutoff frequency in Hz
b, a = signal.butter(4, cutoff_frequency / (0.5 * fs), btype="low")

# Applying the filter
filtered_signal = signal.filtfilt(b, a, noisy_signal)
# Plotting the filtered signal
plt.plot(t, filtered_signal, label="Filtered Signal", color="red")

plt.plot(t, noisy_signal, label="Noisy Signal", color="gray", alpha=0.5)

plt.title("Filtered Sine Wave Signal")

plt.xlabel("Time [s]")

plt.ylabel("Amplitude")

plt.legend()

plt.grid()

plt.show()
# Applying the filter

filtered_signal = signal.filtfilt(b, a, noisy_signal)

# Plotting the filtered signal
plt.plot(t, filtered_signal, label="Filtered Signal", color="red")
plt.plot(t, noisy_signal, label="Noisy Signal", color="gray", alpha=0.5)
plt.title("Filtered Sine Wave Signal")
plt.xlabel("Time [s]")
plt.ylabel("Amplitude")
plt.legend()
plt.grid()
plt.show()

# Performing FFT
frequencies = np.fft.fftfreq(len(t), 1/fs)
fft_original = np.fft.fft(signal)
fft_filtered = np.fft.fft(filtered_signal)
# Plotting the frequency spectrum
plt.plot(frequencies[:len(frequencies)//2], np.abs(fft_original)[:len(frequencies)//2], label="Original Signal")
plt.plot(frequencies[:len(frequencies)//2], np.abs(fft_filtered)[:len(frequencies)//2], label="Filtered Signal", color="red")
plt.title("Frequency Spectrum")
plt.xlabel("Frequency [Hz]")
plt.ylabel("Magnitude")
plt.legend()
plt.grid()
plt.show()