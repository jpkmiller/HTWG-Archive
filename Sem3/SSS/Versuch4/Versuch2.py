import Versuch4.UtilityFunctions as util

import numpy as np

ref_paths_isy = ["AudioDaten/Referenzspektren/Hoch", "AudioDaten/Referenzspektren/Tief",
                 "AudioDaten/Referenzspektren/Links", "AudioDaten/Referenzspektren/Rechts"]

ref_paths_alex = ["AudioDaten/ReferenzspektrenAlex/Hoch", "AudioDaten/ReferenzspektrenAlex/Tief",
                  "AudioDaten/ReferenzspektrenAlex/Links", "AudioDaten/ReferenzspektrenAlex/Rechts"]

datensatz_isy = ["AudioDaten/TestdatensatzIsy/Hoch", "AudioDaten/TestdatensatzIsy/Tief",
                 "AudioDaten/TestdatensatzIsy/Links", "AudioDaten/TestdatensatzIsy/Rechts"]

datensatz_alex = ["AudioDaten/TestdatensatzAlexCopy/Hoch", "AudioDaten/TestdatensatzAlexCopy/Tief",
                  "AudioDaten/TestdatensatzAlexCopy/Links", "AudioDaten/TestdatensatzAlexCopy/Rechts"]

reference_data = util.get_reference_data_graphs(ref_paths_isy)
data_of_all_hertz_amps = []
for path in datensatz_isy:
    hertz_amp = []
    paths_of_file = util.get_paths_of_file(path)
    for file in paths_of_file:
        np_command_array = np.load(file)
        triggered = util.trigger(np_command_array)
        windowed = util.windowing(triggered)
        hertz_amp.append(util.get_hertz_amplitude(windowed))
    data_of_all_hertz_amps.append(hertz_amp)
util.compare_routine(reference_data, data_of_all_hertz_amps)

for i, spectrum_a in enumerate(reference_data):
    for j, spectrum_b in enumerate(reference_data[i:]):
        print("Bravais−Pearson [" + str(i) + ", " + str(i + j) + "] = %.5f"
              % util.get_correlation(spectrum_a[1], spectrum_b[1]))

print("done")
