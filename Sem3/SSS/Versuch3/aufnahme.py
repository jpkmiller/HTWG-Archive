import pyaudio
import numpy as np
import matplotlib.pyplot as plt

# Parameter setzen
FORMAT = pyaudio.paInt16
SAMPLEFREQ = 44100
FRAMESIZE = 44100 * 5
NOFFRAMES = 1

# Signal aufnehmen
p = pyaudio.PyAudio()
print("running")
stream = p.open(format=FORMAT, channels=1, rate=SAMPLEFREQ,
                input=True, frames_per_buffer=FRAMESIZE)
data = stream.read(NOFFRAMES * FRAMESIZE)
decoded = np.fromstring(data, 'Int16')

stream.stop_stream()
stream.close()
p.terminate()

# Signal plotten
steps = np.arange(start=0, stop=5000, step=(1 / 44.1))
np.savetxt("fuenfSekAufnahme.csv", np.asarray([steps, decoded]).T, delimiter=",")
plt.plot(steps, decoded)
plt.title("Aufnahme (5 Sek.)")
plt.show()
