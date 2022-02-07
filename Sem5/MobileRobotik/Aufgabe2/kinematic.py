import numpy as np
import math as m
import matplotlib.pyplot as plt
import Aufgabe2.transform as t

# ZIEL: tOP
# TOR = TOR

x = 2
y = 1
theta = 30

l = 0.6
h = 0.2
r = 0.1

a = 0.1
b = 0.1
alpha = 40

l1 = 0.5
beta1 = 30
l2 = 0.5
beta2 = -10


# -------------------------------------------------------------------------Aufgabe a)
# =========== TOR ===========

def get_OR(__x__, __y__, __r__, __theta__):
    Trans_OR = t.trans(np.array([__x__, __y__, __r__]))
    Rot_OR = t.rot2trans(t.rotz(__theta__))
    return Trans_OR @ Rot_OR


TOR = get_OR(x, y, r, theta)

print("Homogene Transformationsmatrix von O nach R ist:\n", TOR)


# =========== TRDB = TI((l/2 - a/2), 0, h) ===========

def get_RDB(__l__, __a__, __h__):
    return t.trans(np.array([(__l__ / 2.0) - (__a__ / 2.0), 0, __h__]))


TRDB = get_RDB(l, a, h)

print("Homogen. Transformationsmatrix von R nach BD ist:\n", TRDB)


#  =========== TDBD = TI(0, 0, b/2) * R(z, alpha) * R(x, 90) ===========

def get_DBD(__b__, __alpha__):
    Trans_DBD = t.trans(np.array([0, 0, __b__ / 2.0]))
    Rotz_DBD = t.rot2trans(t.rotz(__alpha__))
    Rotx_DBD = t.rot2trans(t.rotx(90))
    return Trans_DBD @ Rotz_DBD @ Rotx_DBD


TDBD = get_DBD(b, alpha)

print("Homogen. Transformationsmatrix von DB nach D ist:\n", TDBD)

# =========== TDA1 = TI(0, 0, a/2) * R(z, beta1) * TI(l1, 0, 0)
"""
genial... :D wir bereiten D auf A1 vor,
indem wir es 0.05 in x-Richtung verschieben, um beta1 drehen und anschließend bewegen wir uns ans Ende von A1
"""


def get_DA1(__l1__, __a__, __beta1__):
    Trans_DA1 = t.trans(np.array([0, 0, __a__ / 2.0]))
    Rotz_DA1 = t.rot2trans(t.rotz(__beta1__))
    Trans_A1l = t.trans(np.array([__l1__, 0, 0]))
    return Trans_DA1 @ Rotz_DA1 @ Trans_A1l


TDA1 = get_DA1(l1, a, beta1)

print("Homogen. Transformationsmatrix von D nach A1 ist:\n", TDA1)


# =========== TA1A2 = R(z, beta2) * TI(l2, 0, 0) ===========

def get_A1A2(__l2__, __beta2__):
    Rotz_A1A2 = t.rot2trans(t.rotz(__beta2__))
    Trans_A1A2 = t.trans(np.array([__l2__, 0, 0]))
    return Rotz_A1A2 @ Trans_A1A2


TA1A2 = get_A1A2(l2, beta2)

print("Homogen. Transformationsmatrix von A1 nach A2 ist:\n", TA1A2)

# =========== TOP = TOR * TRDB * TDBD * TDA1 * TA1A2 ===========
TOP = TOR @ TRDB @ TDBD @ TDA1 @ TA1A2

print("Homogen. Transformationsmatrix von TO nach A2 ist:\n", TOP)

# Punkt PA2
pA2 = np.array([0.0, 0.0, 0.0, 1.0])

# pO
pO = TOP @ pA2

print("Der Punkt p im globalen KS O ist:\n", pO)


# -------------------------------------------------------------------------Aufgabe b)

# (Px, Py, Pz) => (beta1, beta2, alpha)

def inverseKinematik(p, h, l, l1, l2):
    x, y, z = p
    x = x - l / 2
    z = z - h

    # calculate a, b, c
    a = m.sqrt(x ** 2 + y ** 2 + z ** 2)
    assert a <= l1 + l2, "Not solvable! Because a > l1 + l2"
    c = ((a ** 2 - l1 ** 2 - l2 ** 2) / (2 * l1))
    b = -m.sqrt(l2 ** 2 - c ** 2)

    inv_alpha = m.atan2(y, x)
    inv_beta1 = m.atan2(z, x) - m.atan2(b, l1 + c)
    inv_beta2 = m.atan2(b, c)

    return m.degrees(inv_alpha), m.degrees(inv_beta1), m.degrees(inv_beta2)


def vorwaertsKinematik(alpha, beta1, beta2, xR, yR, r, a, b, h, l, l1, l2, theta):
    return (get_OR(xR, yR, r, theta) @
            get_RDB(l, a, h) @
            get_DBD(b, alpha) @
            get_DA1(l1, a, beta1) @
            get_A1A2(l2, beta2) @
            np.array([0, 0, 0, 1]))


# ============Berechnung mit Vorwärtskinematik===============

# =========== TRP = TRDB * TDBD * TDA1 * TA1A2 ===========
pR = vorwaertsKinematik(alpha, beta1, beta2, 0, 0, 0, 0, 0, h, l, l1, l2, 0)
print("Der Punkt p im KS R ist:\n", pR)
inv_alpha, inv_beta1, inv_beta2 = inverseKinematik((pR[0], pR[1], pR[2]), h, l, l1, l2)
print("Die Winkel hierfür lauten: alpha: {}, beta1: {}, beta2: {}\n".format(inv_alpha, inv_beta1, inv_beta2))


# -------------------------------------------------------------------------Aufgabe c)


def createCircle(resolution, radius, px):
    def rotate(x, y, r):
        rx = (x * m.cos(r)) - (y * m.sin(r))
        ry = (y * m.cos(r)) + (x * m.sin(r))
        return (rx, ry)

    circle = []
    arc = (2 * m.pi) / resolution  # what is the angle between two of the points
    for p in range(0, resolution):
        (py, pz) = rotate(0, radius, arc * p)
        circle.append((px, py, pz))
    return np.array(circle)


def applyInverseKinematik(circ, __h__, __l__, __l1__, __l2__):
    angles = []
    for __x__, __y__, __z__ in circ:
        tx, ty, tz = np.array([__x__, __y__, __z__])
        angles.append(inverseKinematik((tx, ty, tz), __h__, __l__, __l1__, __l2__))
    return np.array(angles).T


def applyVorwaertsKinematik(angl, xR, yR, r, a, b, h, l, l1, l2, theta):
    points = []
    for alpha, beta1, beta2 in angl:
        points.append(vorwaertsKinematik(alpha, beta1, beta2, xR, yR, r, a, b, h, l, l1, l2, theta))
    return np.array(points).T


# render circle for reference
circle = createCircle(50, 0.5, 1)
x, y, z = circle.T
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')
ax.scatter(x, y, z, zdir='z', s=20, c=None, depthshade=True)
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Z')
plt.show()

# render angles alpha, beta1, beta2
inv_alpha, inv_beta1, inv_beta2 = applyInverseKinematik(circle, h, l, l1, l2)
"""
plt.plot(inv_alpha, label="alpha")
plt.plot(inv_beta1, label="beta1")
plt.plot(inv_beta2, label="beta2")
plt.legend()
plt.show()
"""

# render circle drawn by robot
fig = plt.figure()
ko = fig.add_subplot(111, projection='3d')
angles = np.array([inv_alpha, inv_beta1, inv_beta2]).T
vor_x, vor_y, vor_z = applyVorwaertsKinematik(angles, 0, 0, 0, 0, 0, h, l, l1, l2, 0)[:3]
ko.scatter(np.round(vor_x), vor_y, vor_z, zdir='z')
ko.set_xlabel('X')
ko.set_ylabel('Y')
ko.set_zlabel('Z')
plt.show()

"""
Bei uns stimmt die Berechnung von beta1 nicht ganz. Es gibt große Abweichungen vom eigentlichen beta1 Wert.
"""
