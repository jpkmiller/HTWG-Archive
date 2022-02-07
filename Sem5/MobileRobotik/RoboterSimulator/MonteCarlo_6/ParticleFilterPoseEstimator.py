# Partikel geben eine mögliche Pose vom Roboter an

import math as m
import numpy as np
import random as rnd
from RoboterSimulator.Robot_Simulator_V3 import Robot, sensorUtilities


class ParticleFilterPoseEstimator:

    def __init__(self, poseFrom, poseTo, n=200):
        """
        Erzeugt n zufällige Partikel in dem Roboter-Posen-Bereich
        """

        def rndPose(i):
            return rnd.uniform(poseFrom[i], poseTo[i])

        self.particles = [(rndPose(0), rndPose(1), rndPose(2)) for _ in range(n)]
        self.T = 0.1
        self.sigma_pose = np.zeros((3, 3))

    def integrateMovement(self, motion, sigma_motion):
        """
        Wendet auf alle Partikel den Bewegungsbefehl motion mit einem zufälligen Rauschen an.
        """
        v = motion[0]
        omega = motion[1]
        particles = self.particles
        particles_copy = []

        for p in particles:
            # v und omega verrauschen.
            v_noisy = v + rnd.gauss(0.0, m.sqrt(sigma_motion[0, 0]))  # Robot z. 109
            omega_noisy = omega + rnd.gauss(0.0, m.sqrt(sigma_motion[1, 1]))  # Robot z. 114
            x = p[0] + v_noisy * self.T * m.cos(p[2])
            y = p[1] + v_noisy * self.T * m.sin(p[2])
            t = p[2] + omega_noisy * self.T
            particles_copy.append((x, y, t))
        self.particles = particles_copy

    def integrateMeasurement(self, dist_list, alpha_list, distantMap):
        """
        Gewichtet alle Partikel mit dem Likelihoodfield-Algorithmus und führt ein Resampling durch.
        dist_list, alpha_list sind vom Roboter aufgenommene Laserdaten in Polarkoordinaten
        """

        # Likelihood
        weights = [ParticleFilterPoseEstimator.getWeightLikeliHood(particle, dist_list, alpha_list, distantMap) for
                   particle in self.particles]
        # hier sind alle Gewichte drinnen

        # Resampling - Ticketsystem
        N = len(self.particles)
        index = int(rnd.random() * N)
        listParticles = []
        maxw = max(weights)
        actw = 0.0
        for _ in range(N):
            actw += rnd.random() * maxw
            while actw > weights[index]:
                actw -= weights[index]
                index = (index + 1) % N
            listParticles.append(self.particles[index])
        self.particles = listParticles

    @staticmethod
    def getWeightLikeliHood(particle, dist_list, alpha_list, distantMap):
        def ndf(x, dist, sigma):
            c = 1 / (m.sqrt(2 * sigma * m.pi))
            return c * np.exp(-0.5 * (x - dist) ** 2 / sigma)

        coords = sensorUtilities.transformPolarCoordL2G(dist_list, alpha_list, particle)
        prob = 1.0
        for p in coords:
            dist = distantMap.getValue(p[0], p[1])
            if dist is None:
                continue
            prob *= ndf(0, dist, 0.4 ** 2)
        return prob

    def getPose(self):
        """
        Berechnet aus der Partikelmenge eine Durchschnittspose.
        """
        return np.mean([n for n in self.particles], axis=0)

    def getCovariance(self):
        """
        Berechnet die Kovarianz der Partikelmenge.
        """
        return np.cov([n for n in self.particles])

    def getParticles(self):
        return self.particles
