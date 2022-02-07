###########################
# Author1: Isabella Schön #
# Author2: Josef Müller   #
###########################

from sympy import symbols, solve, Eq
import numpy as np
import matplotlib.pyplot as plt
import math as m

from RoboterSimulator.Robot_Simulator_V3 import emptyWorld  # Räume für Roboter
from RoboterSimulator.Robot_Simulator_V3 import Robot, World  # Bewegung des Roboters


# ----------------------------------------------------Aufgabe a)
def curveDrive(robot: Robot, world: World, v: float, r: float, delta_theta: float) -> np.ndarray.dtype:
    points = []
    if r == 0 or v == 0:
        omega = -0.01  # je kleiner omega desto genauer werden die Werte
    else:
        omega = v / r
    # omega_0 ist die Winkelgeschwindigkeit bzw. Rotationsgeschwindigkeit

    timeDelta = m.radians(delta_theta) / abs(omega)
    # Zeit pro Richtungsänderung

    steps = int(timeDelta / robot.getTimeStep())

    for _ in range(steps):
        points.append(world.getTrueRobotPose()[:2])
        robot.move([abs(v), omega])
    return points


def straightDrive(robot: Robot, world: World, v: float, l: float):
    points = []
    # getTimeStep gibt an, wie lang eine Bewegung des Roboters benötigt
    amountOfSteps = int((l / abs(v)) / robot.getTimeStep())
    for _ in range(amountOfSteps):
        points.append(world.getTrueRobotPose()[:2])
        robot.move([v, 0])
    return points


def kreis(robot: Robot, world: World, v: float, r: float):
    return curveDrive(robot, world, v, r, 360)  # Drehungen - Rechtskurve


def rechteck(robot: Robot, world: World, v: float, length: tuple):
    l1, l2 = length
    points = []
    for _ in range(2):
        points += straightDrive(robot, world, v, l1)
        points += curveDrive(robot, world, -v, 0.5, 90)
        points += straightDrive(robot, world, v, l2)
        points += curveDrive(robot, world, -v, 0.5, 90)
    return points


def fahrspurwechsel(robot: Robot, world: World, v: float, t: int, r: float):
    thetaDelta = m.degrees(((t / 2) / robot.getTimeStep()) * (abs(v) / r))
    points = []
    points += curveDrive(robot, world, -v, r, thetaDelta)
    points += curveDrive(robot, world, v, r, thetaDelta)
    return points


def test_Teil_A(robot: Robot, world: World):
    CD = np.asarray(curveDrive(robot, world, -0.1, 1, 90)).T
    plt.scatter(CD[0], CD[1])

    RE = np.asarray(rechteck(robot, world, 0.5, (2, 2))).T
    plt.scatter(RE[0], RE[1])

    SD = np.asarray(straightDrive(robot, world, 0.2, 5)).T
    plt.scatter(SD[0], SD[1])

    FW = np.asarray(fahrspurwechsel(robot, world, 0.2, 2, 1)).T
    plt.scatter(FW[0], FW[1])

    SD2 = np.asarray(straightDrive(robot, world, 0.2, 5)).T
    plt.scatter(SD2[0], SD2[1])

    K = np.asarray(kreis(robot, world, -0.5, 2)).T
    plt.scatter(K[0], K[1])
    plt.show()

    # ----------------------------------------------------Aufgabe b)


def followLine(robot: Robot, world: World, v: float, p1: tuple, p2: tuple):
    r = np.asarray(p2) - np.asarray(p1)
    rNorm = r / np.linalg.norm(r)
    robotPose = world.getTrueRobotPose()
    polyLine = []

    while not abfrageTol(robotPose, p2, 0.1):
        robotPose = world.getTrueRobotPose()
        robotTheta = robotPose[2]
        robotPoint = np.asarray(robotPose[:2])
        polyLine.append(robotPoint)
        pointOnR = calculatePoint(robotPoint, (rNorm[1], -rNorm[0]), p1, rNorm)
        n = pointOnR - robotPoint
        nr = robotPoint + n + rNorm
        theta_stern = m.atan2(nr[1] - robotPoint[1], nr[0] - robotPoint[0])
        k_omega = 1.0
        omega = min(m.pi, k_omega * diff(theta_stern, robotTheta))
        robot.move([abs(v), omega])
    world.drawPolyline(polyLine)


def test_followLine(robot: Robot, world: World):
    p1 = (0, 1)
    p2 = (10, 8)
    world.drawPolyline([p1, p2])
    followLine(robot, world, 1, p1, p2)


def gotoGlobal(robot: Robot, world: World, v: float, point: tuple, tol: float):
    pose = world.getTrueRobotPose()
    polyLine = []
    while not abfrageTol(pose, point, tol):
        pose = world.getTrueRobotPose()
        robotTheta = pose[2]
        robotPoint = np.asarray(pose[:2])  # x-KO und y-KO
        polyLine.append(robotPoint)
        zielPoint = np.asarray(point[:2])
        theta_stern = m.atan2(zielPoint[1] - robotPoint[1], zielPoint[0] - robotPoint[0])
        k_omega = 1.0
        omega = min(m.pi, k_omega * diff(theta_stern, robotTheta))
        robot.move([abs(v), omega])
    return polyLine


def test_gotoGlobal(robot: Robot, world: World):
    world.addBox(8, 15)
    gotoGlobal(robot, world, 0.5, (8, 15), tol=0.1)
    world.addBox(10, 5)
    gotoGlobal(robot, world, 0.5, (10, 5), tol=0.1)
    world.addBox(1, 6)
    gotoGlobal(robot, world, 0.5, (1, 6), tol=0.1)


def followPolyline(robot, world, v, poly):
    polyLine = []
    for x, y in poly:
        world.addBox(x, y)
        points = gotoGlobal(robot, world, v, point=(x, y), tol=0.3)
        polyLine += points
    world.drawPolyline(polyLine)


def test_followPolyline(robot: Robot, world: World):
    line = [[2, 2], [4, 6], [8, 10], [2, 4]]
    followPolyline(robot, world, 0.5, line)


def diff(theta_stern, theta):
    return (theta_stern - theta + m.pi) % (2 * m.pi) - m.pi


def abfrageTol(pose, point, tol):
    return (point[0] - tol <= pose[0] <= point[0] + tol) and (point[1] - tol <= pose[1] <= point[1] + tol)


def calculatePoint(robot: tuple, n: tuple, p: tuple, r: tuple):  # b1)
    t, s = symbols('t, s')
    eq1 = Eq(robot[0] + t * n[0], p[0] + s * r[0])
    eq2 = Eq(robot[1] + t * n[1], p[1] + s * r[1])
    tResolved = list(solve((eq1, eq2), (t, s)).items())[0][1]
    return robot[0] + tResolved * n[0], robot[1] + tResolved * n[1]
    # Isa - orthogonale Projektion
    # betrag = sqrt(rVektor[0] ** 2 + rVektor[1] ** 2)  # Betrag ausrechnen
    # skalar = robotP[0] * rVektor[0] + robotP[1] * rVektor[1]
    # orthogonalProjektion = (skalar / betrag) * (rVektor / betrag)
    # print(orthogonalProjektion) -> [3.9261745  2.74832215]


def calculateAngle(n: tuple):  # für b1)
    _scalarProduct = np.dot((1, 0), n)
    _hLength = 1
    _nLength = m.sqrt(n[0] ** 2 + n[1] ** 2)
    return m.acos(abs(_scalarProduct) / (_hLength * _nLength))


def test_robot():
    myRobot = Robot.Robot()
    myWorld = emptyWorld.buildWorld()
    myWorld.setRobot(myRobot, [2, 5.5, m.pi / 2])  # (myRobot,[x,y,theta])
    test_Teil_A(myRobot, myWorld)  # gesamter Teil a)
    test_followLine(myRobot, myWorld)  # Teil b) Nummer 1
    test_gotoGlobal(myRobot, myWorld)  # Teil b) Nummer 2
    test_followPolyline(myRobot, myWorld)  # Teil b) Nummer 3
    myWorld.close()


if __name__ == '__main__':
    test_robot()
