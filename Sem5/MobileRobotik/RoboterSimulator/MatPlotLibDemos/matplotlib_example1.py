# Damit numpy und matplotlib gefunden werden, muss in Preferences der
# project Interpreter gesetzt sein und die entsprecheneden Pakete installiert sein.
#
# O. Bittel; 20.10.2017; Python 3.5

import numpy as np
import matplotlib.pyplot as plt

# Plotting:
x = np.arange(0, 5, 0.1)
y = np.sin(x)
# im plotfenster: verschieben/zoomen: mit linker/rechter Maustaste ziehen
plt.plot(x, y, '.b')
plt.plot(x, y+1, ':r')
plt.plot(x, y-1, '-g')
plt.show()

