import numpy as np
import matplotlib.pyplot as plt

samples_per_s = 44100


# Analyse des Signals
def readMeasure(file):
    with open(file) as data:
        return np.genfromtxt(data,
                             skip_header=int(samples_per_s * 2.45), skip_footer=int(samples_per_s * 2.3),
                             delimiter=",").T


def getSignalDauer(zeit) -> float:
    return zeit[-1] - zeit[0]


def getPeriodenDauer(zeit: np.ndarray.dtype, amplitude: np.ndarray.dtype) -> float:
    zeitUndAmplitude = np.column_stack((zeit, amplitude))
    # sortiere alle Einträge und ziehe die Zeit der letzten beiden voneinander ab.
    sortierteZeitUndAmplitude = zeitUndAmplitude[zeitUndAmplitude[:, 1].argsort()]
    return abs(sortierteZeitUndAmplitude[-1][0] - sortierteZeitUndAmplitude[-2][0])


def getFrequenz(periodenDauer: float) -> float:
    return (1 / periodenDauer) * 1000


def getAbtastFrequenz(signalDauer: float, zeit) -> float:
    return 1 / ((signalDauer / 1000) / len(zeit))


def getAnzahlAbtastpunkte(zeit) -> int:
    return len(zeit)


def getAbtastInterval(signalDauer: float, anzahlAbtastPunkte: int) -> float:
    return (signalDauer / 1000) / anzahlAbtastPunkte


zeit, amplitude = readMeasure("fuenfSekAufnahme.csv")

signalDauer = getSignalDauer(zeit)
periodenDauer = getPeriodenDauer(zeit, amplitude)
frequenz = getFrequenz(periodenDauer)
abtastFrequenz = getAbtastFrequenz(signalDauer, zeit)
anzahlAbtastpunkte = getAnzahlAbtastpunkte(zeit)
abtastIntervall = getAbtastInterval(signalDauer, anzahlAbtastpunkte)

print("Grundperiode: {0} ms".format(periodenDauer))
print("Frequenz: {0} Hz".format(frequenz))
print("Signaldauer: {0} ms".format(signalDauer))
print("Abtastfrequenz: {0} Hz".format(abtastFrequenz))
print("Signallänge (Anzahl der Abtastzeitpunkte): {0}".format(anzahlAbtastpunkte))
print("Abtastintervall: {0} ms".format(abtastIntervall))

# Plotten mehrerer Perioden im Zeitbereich
fig1, ax1 = plt.subplots(figsize=(18, 6), dpi=300)
plt.grid()
plt.ylabel("Amplitude")
plt.xlabel("Zeit [ms]")
ax1.plot(zeit, amplitude)
plt.title("Signal im Zeitbereich")
plt.savefig("SignalImZeitbereich.png")
plt.show()

# Berechnung der Fouriertransformierten
fourier = np.fft.fft(amplitude)
spektrum = np.abs(fourier)

# das ist die das was f = n / (M-dt) macht (noch komplizierter konnte der Keppler die Formel auch nicht aufschreiben)
x_scale = np.arange(anzahlAbtastpunkte) / (signalDauer / 1000)
mid_idx = int(len(amplitude) / 2)
index_of_max = np.argmax(spektrum[:mid_idx])
print('calculated frequency: ' + str(x_scale[index_of_max]))

# Plotten des Amplitudenspektrum im Frequenzbereich
fig3, ax3 = plt.subplots(figsize=(18, 6), dpi=300)
plt.grid(True)
plt.ylabel("Amplitude")
plt.xlabel("Frequenz [Hz]")
plt.title("Amplitudenspektrum im Frequenzbereich der ersten 5 Harmonischen")
ax3.plot(x_scale[:index_of_max * 5], spektrum[:index_of_max * 5])
plt.savefig("Fouriertransformation.png")
plt.show()
