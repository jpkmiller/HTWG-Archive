import numpy as np

reference_voltage_10 = np.array([0.987, 1.996, 3.004, 4.063, 4.976, 5.986, 7.044, 8.055, 9.065, 10.076])
multimeter_voltage_10 = np.array([0.983, 1.987, 2.992, 4.05, 4.96, 5.96, 7.02, 8.03, 9.04, 10.04])
red_lab_box_voltage_10 = np.array([0.986, 1.992, 2.998, 4.053, 4.960, 5.977, 7.041, 8.047, 9.053, 9.961])

reference_DA_input = np.arange(0.5, 5, 0.5)
oszi_voltage_5 = np.array([0.512, 1.016, 1.525, 2.043, 2.542, 3.049, 3.563, 4.071, 4.576, 5.064])

print(reference_DA_input)

BITS = 11


def measuring_error(data):
    return reference_voltage_10 - data


def get_accuracy(bits=BITS):
    return 1.0 / (2 ** bits)


def get_accuracy_in_volts(u_max=10, u_min=-10, bits=BITS):
    return (u_max - u_min) * get_accuracy(bits)


def get_std(input_data=multimeter_voltage_10):
    diff = measuring_error(input_data)
    std = np.sqrt((1 / (len(input_data) - 1)) * np.sum(diff ** 2))
    return std


error_ad = measuring_error(red_lab_box_voltage_10)
error_multi = measuring_error(multimeter_voltage_10)
# print(f"error A/D-Wanlder: {error_ad}")
# print(f"error Multimeter: {error_multi}")

print(f"avg_error_ad = {np.average(error_ad)}")
print(f"avg_error_multi = {np.average(error_multi)}")

print(f"_> A/D std= {get_std(input_data=red_lab_box_voltage_10)}")

print(f"_> Multimeter std= {get_std(input_data=multimeter_voltage_10)}")