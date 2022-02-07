import Versuch4.UtilityFunctions as util
import numpy as np

# read data and plot data
path = "AudioDaten/beliebigeSpracheingabe.npy"
data = util.read_data(path)
util.plot_data("Beliebige Spracheingabe", "Zeit [s]", "Amplitude",
               np.linspace(0, len(data) / 44100, len(data)), data)
# cut unnecessary parts out and plot
data = util.trigger(data)
util.plot_data("Ausschnitt Beliebige Spracheingabe", "Zeit [s]", "Amplitude",
               np.linspace(0, len(data) / 44100, len(data)), data)
# create an amplitude spectrum and plot it
amplitude_spectrum_hertz = util.get_hertz_amplitude(util.amplitude_spectrum(data), True)
util.plot_data("Volles Amplitudenspektrum - Beliebige Spracheingabe", "Frequenz (kHz)", "Amplitude",
               amplitude_spectrum_hertz[0],
               amplitude_spectrum_hertz[1])
length = len(amplitude_spectrum_hertz[0])
util.plot_data("Amplitudenspektrum - Beliebige Spracheingabe", "Frequenz (kHz)", "Amplitude",
               amplitude_spectrum_hertz[0][:int(length / 2)],
               amplitude_spectrum_hertz[1][:int(length / 2)])

# use windowing now and created amplitude spectrum afterwards
data = util.windowing(data)
data = util.get_hertz_amplitude(data, True)
length = len(data[0])
util.plot_data("Volles Amplitudenspektrum mit Windowing", "Frequenz (kHz)", "Amplitude", data[0], data[1])
util.plot_data("Amplitudenspektrum mit Windowing", "Frequenz (kHz)", "Amplitude",
               data[0][:int(length / 2)], data[1][:int(length / 2)])
