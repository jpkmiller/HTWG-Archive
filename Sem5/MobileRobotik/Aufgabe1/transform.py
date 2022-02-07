#!/usr/bin/env python

"""
    Transform.py enthält Matrixoperationen für Translationen, Rotationen und Transformationen
    https://github.com/jpkmiller/MobileRobotik/tree/master/Aufgabe1
"""

__author__ = "Josef Müller, Isabella Schön"

import numpy as np
import math


def rot(theta):
    """
    liefert eine 2D-Rotationsmatrix mit Drehwinkel theta zurück.
    :param theta: Winkel
    :return: 2D-Rotationsmatrix
    """
    rad_theta = math.radians(theta)
    return np.array([
        [np.cos(rad_theta), -np.sin(rad_theta)],
        [np.sin(rad_theta), np.cos(rad_theta)]
    ])


def rotx(theta):
    """
    liefert eine elementare 3D-Rotationsmatrix mit Drehwinkel theta um Drehachse z zurück.
    :param theta: Winkel
    :return: 3D-Rotationsmatrix
    """
    rad_theta = math.radians(theta)
    return np.array([
        [1, 0, 0],
        [0, np.cos(rad_theta), -np.sin(rad_theta)],
        [0, np.sin(rad_theta), np.cos(rad_theta)]
    ])


def roty(theta):
    """
    liefert eine elementare 3D-Rotationsmatrix mit Drehwinkel theta um Drehachse z zurück.
    :param theta: Winkel
    :return: 3D-Rotationsmatrix
    """
    rad_theta = math.radians(theta)
    return np.array([
        [np.cos(rad_theta), 0, np.sin(rad_theta)],
        [0, 1, 0],
        [-np.sin(rad_theta), 0, np.cos(rad_theta)]
    ])


def rotz(theta):
    """
    liefert eine elementare 3D-Rotationsmatrix mit Drehwinkel theta um Drehachse z zurück.
    :param theta: Winkel
    :return: 3D-Rotationsmatrix
    """
    rad_theta = math.radians(theta)
    return np.array([
        [np.cos(rad_theta), -np.sin(rad_theta), 0],
        [np.sin(rad_theta), np.cos(rad_theta), 0],
        [0, 0, 1]
    ])


def rot2trans(r):
    """
    wandelt die Rotationsmatrix r in eine homogene Transformationsmatrix um und liefert diese zurück.
    :param r: Rotationsmatrix
    :return: homogene Transformationsmatrix
    """
    shape = np.shape(r)
    return np.append(np.c_[r, np.zeros(shape[0])], [np.append(np.zeros(shape[1]), [1])], axis=0)


def trans(t):
    """
    liefert eine homogene Translationsmatrix mit Translation t zurück.
    :param t: Tuple der Größe 2 bzw. 3 für den 2D- bzw. 3D-Fall
    :return: homogene Translationsmatrix
    """
    shape = np.shape(t)
    return np.vstack([np.c_[np.identity(shape[0]), t], [np.append(np.zeros(shape[0]), [1])]])
