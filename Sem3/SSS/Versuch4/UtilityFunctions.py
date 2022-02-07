# UtilityFunctions.py

import numpy as np
import matplotlib.pyplot as plot
import pylab
import scipy.signal
import scipy.stats

SAMPLE_FREQ = 44100
TRIGGER = 1000
START = 10000
WINDOW_SIZE = 512
GAUSS_WINDOW = scipy.signal.gaussian(WINDOW_SIZE, std=4)


# read .npy file
def read_data(path):
    return np.load(path)


# cuts unnecessary Data from array
def trigger(data):
    ret_data = np.zeros(SAMPLE_FREQ)
    start = np.min(np.argwhere(data > TRIGGER))
    ret_data += data[start:start + SAMPLE_FREQ]
    return ret_data


# creates Windows and calculates the average value for each window
def windowing(sample_data: np.array):
    window_slices = [sample_data[window_start_index: window_start_index + WINDOW_SIZE]
                     for window_start_index in np.arange(0, sample_data.shape[0], int(WINDOW_SIZE / 2))]

    # multiply each window slice with a Gauss−Window and calculate the average of all window amplitude_spectrum
    window_sum = np.zeros(WINDOW_SIZE, dtype=complex)
    clipping = len(window_slices) - 2
    for window in window_slices[:clipping]:
        multiplied_window = np.multiply(window, GAUSS_WINDOW)
        window_sum += amplitude_spectrum(multiplied_window)

    window_average = np.divide(window_sum, len(window_slices))
    return window_average


def calc_amplitude_from_complex(complex_value):
    return np.sqrt(np.square(complex_value.real) + np.square(complex_value.imag))


# returns an array with hertz and amplitude values
def get_hertz_amplitude(data, with_alias=False):
    sampling_value = 1 / SAMPLE_FREQ
    amplitude = [calc_amplitude_from_complex(entry) for entry in data]
    signal_length = len(data)
    hertz = [idx / (signal_length * sampling_value) for idx, val in enumerate(data)]
    k_hertz = np.divide(hertz, 1000)
    if with_alias:
        return k_hertz, amplitude
    return k_hertz[:int(len(k_hertz) / 2)], amplitude[:int(len(amplitude) / 2)]


# calculate amplitude spectrum
def amplitude_spectrum(data):
    amp_spectrum = np.fft.fft(data)
    return amp_spectrum


# a) Für den Spracherkenner brauchen wir
# zunächst die Referenzspektren für die vier Befehle
# ”Hoch”, ”Tief”, ”Links” und ”Rechts”.
# Nehmen Sie dazu jeweils 5 Beispiele pro Befehl auf
# und berechnen Sie deren Spektren mit der Windowing-Methode aus Aufgabe 1.
# calculates the average spectrum
def average_amp_spec_with_windowing(path_type):
    files_of_type = list()
    for file in path_type:
        file = windowing(trigger(read_data(file)))
        files_of_type.append(file)
    files_of_type = np.array(files_of_type)
    average = np.average(files_of_type, axis=0)
    return average


# generates an array with file_paths
def get_paths_of_file(path):
    paths = list()
    for i in range(1, 6):
        filepath = path + str(i) + ".npy"
        paths.append(filepath)
    return paths


def get_correlation(spectrum_a, spectrum_b):
    return scipy.stats.pearsonr(spectrum_a, spectrum_b)[0]


# calculates the bravais−pearson coefficient and returns the closest match, as well as the accuracy
def recognise(dataset_reference, dataset_input_hertz_amp):
    detected_type_idx = -1
    accuracy = -1

    for dataset_reference_index, reference_type in enumerate(dataset_reference):
        current_accuracy = scipy.stats.pearsonr(reference_type[1], dataset_input_hertz_amp[1])[0]

        print("Compare sample with reference %d −> Bravais−Pearson coefficient = %.5f" % (
            dataset_reference_index, current_accuracy))
        if current_accuracy > accuracy:
            detected_type_idx = dataset_reference_index
            accuracy = current_accuracy

    print("Sample detected as %d" % detected_type_idx)
    return detected_type_idx


# routine to compare with different Samples
def compare_routine(reference_data, data_set_input):
    correct_recognised = 0
    amount_data_sets = 0

    # iterate over all recording from each type: Hoch -> 0,1, 2, 3, 4, Tief -> 0, ...
    for type_idx, data_set_type in enumerate(data_set_input):
        for data_set_hertz_amp in data_set_type:
            detected_type = recognise(reference_data, data_set_hertz_amp)

            if detected_type == type_idx:
                print("right reference detected")
                correct_recognised += 1
            else:
                print("wrong reference detected")

        amount_data_sets += len(data_set_type)

    correct_recognised = (correct_recognised / amount_data_sets)
    failure_rate = 1 - correct_recognised
    print("Detection rate: %d" % (correct_recognised * 100) + "%")
    print("failure rate: %d" % (failure_rate * 100) + "%")


# Das Referenzspektrum erhalten Sie durch Mittelung über die 5 Spektren.
# Stellen Sie Ihre 4 Referenzspektren im Protokoll graphisch dar.

def get_reference_data_graphs(paths):
    reference_data = []
    types = ["Hoch", "Tief", "Links", "Rechts"]
    for path_type in paths:
        files_of_type = get_paths_of_file(path_type)
        avg_amp_spec = average_amp_spec_with_windowing(files_of_type)
        data_hertz_amp = get_hertz_amplitude(avg_amp_spec)
        reference_data.append(data_hertz_amp)

    for idx, ref_data_set in enumerate(reference_data):
        plot_data(types[idx] + " Referenz Amplitudenspektrum", "Frequenz (kHz)", "Amplitude", ref_data_set[0],
                  ref_data_set[1], versuch=2)
    return reference_data


def plot_data(title, name_x_axis, name_y_axis, data_x, data_y=None, versuch=1):
    path = "v1_image/"
    if data_y is None:
        pylab.plot(data_x)
        plot.title(title)
        plot.xlabel(name_x_axis)
        plot.ylabel(name_y_axis)
        plot.savefig(path + title)
        plot.show()
        return
    pylab.plot(data_x, data_y)
    plot.title(title)
    plot.xlabel(name_x_axis)
    plot.ylabel(name_y_axis)
    if versuch == 2:
        path = "v2_image/"
    plot.savefig(path + title.replace(' -', '').replace(' ', '_'))
    plot.show()
