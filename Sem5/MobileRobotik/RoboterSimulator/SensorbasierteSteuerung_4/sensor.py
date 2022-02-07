###########################
# Author1: Isabella Schön #
# Author2: Josef Müller   #
###########################


import math as m
from RoboterSimulator.Robot_Simulator_V3 import labyrinthWorld, simpleWorld, emptyWorld
from RoboterSimulator.Robot_Simulator_V3 import Robot
from RoboterSimulator.Robot_Simulator_V3 import World
from RoboterSimulator.Robot_Simulator_V3 import sensorUtilities
import numpy as np
import random as r


def wander(robot: Robot, v: float = 0.5, d: float = 0.5, returnOnWall: bool = False):
    while True:
        dist = removeNone(robot.sense())
        if len(dist) > 0 and None not in dist and min(dist) - d <= 0:
            if returnOnWall:
                break
            theta = m.radians(r.random() * 90 + 90)
            omega = min(m.pi, theta)
            time = theta / omega
            amountSteps = int(abs(time) / robot.getTimeStep())
            for _ in range(amountSteps):
                robot.move((0, omega))
        robot.move((v, 0))


def removeNone(array: list) -> list:
    return [i for i in array if i is not None]


def equ(v1, v2, tol):  # zwei Werte auf Gleichheit mit gewisser Toleranz vergleichen
    return abs(v2 - v1) <= tol


def debug(robot: Robot, world: World):
    _dst = robot.sense()
    _segments = sensorUtilities.extractSegmentsFromSensorData(_dst, robot.getSensorDirections())
    _transformedLines = sensorUtilities.transformPolylinesL2G(_segments, world.getTrueRobotPose())
    world.drawPolylines(_transformedLines)
    print(_segments)


def draw(segment, world: World, color: str = 'yellow'):
    tL = sensorUtilities.transformPolylinesL2G([segment], world.getTrueRobotPose())
    world.drawPolylines(tL, color=color)


def followWall(robot: Robot, world: World, v: float = 0.5, d: float = 0.5):
    def fixPose():
        _min = min(removeNone(robot.sense()))
        if _min - d < 0:
            # turn a little bit to the right
            robot.move((v, -0.02))
        elif _min - d > 0:
            # turn a little bit to the left
            robot.move((v, 0.01))

    def rotateTillParallel():
        while True:
            # rotation
            distances = robot.sense()
            segments = sensorUtilities.extractSegmentsFromSensorData(distances, robot.getSensorDirections())
            orientationWall = np.round(segments[-1][1], 10)
            if orientationWall[1] == -orientationWall[0] and round(orientationWall[1], 2) == round(d, 1):
                # robot is now parallel to wall!
                break
            robot.move((0, -0.05))

    def driveTillWall():
        endPoint = (19.5, 2)
        while True:
            trueRobotPose = world.getTrueRobotPose()
            if equ(endPoint[0], trueRobotPose[0], 0.5) and equ(endPoint[1], trueRobotPose[1], 0.5):
                print("Robot finished")
                return 0
            distances = robot.sense()
            segments = sensorUtilities.extractSegmentsFromSensorData(distances, robot.getSensorDirections())

            # calculate side wall
            indexSideWall = -1
            sideWall = segments[indexSideWall]  # side wall
            for index, value in enumerate(segments):
                for walue in value:
                    if equ(walue[0], walue[1], 0.01) and equ(walue[0], d, 0.2):
                        sideWall = segments[index]
                        indexSideWall = index
                        break
            draw(sideWall, world, 'yellow')

            if len(segments) > 1:

                # hier wollten wir einen robusten Algorithmus zum Finden des Front-Segmentes realisieren
                indexFrontWall = indexSideWall - 1
                frontWallSegment = segments[indexFrontWall]
                draw(frontWallSegment, world, 'green')  # front wall
                indexFrontSensor = int(m.floor(len(robot.getSensorDirections()) / 2))

                # use last sensor to check whether it has data values and turn if not
                if distances[indexFrontSensor] is not None and distances[indexFrontSensor] - d <= 0:
                # (equ(round(frontWallSegment[0][0], 1), round(frontWallSegment[1][0], 1), 0.3) and equ(round(frontWallSegment[0][0], 1), d, 0.05)) or  # hier wollten wir eine Lösung mit Segmenten realisieren. Leider hat am Ende die Zeit nicht gereicht :(
                    # turning on corner
                    return 1
                elif abs(round(sideWall[1][1], 2)) > round(m.sqrt(d ** 2 + d ** 2), 1) and min(removeNone(distances)) == removeNone(distances)[-1]:
                    # turning on tip
                    return 2

                """
                equ(round(sideWall[1][1], 5), round(-sideWall[1][0], 5), 0.001) and abs(round(sideWall[1][1], 2)) >= round(m.sqrt(d ** 2 + d ** 2), 1) !=
                """

            if not robot.move((v, 0)):
                break
            fixPose()  # sometimes rotations are not full 90 degrees

    def rotateOnCorner():
        # rotate -90 degrees
        for _ in range(10):
            robot.move((0, -m.pi / 2))

    def rotateOnTip():
        # rotate 90 degrees
        for _ in range(10):
            robot.move((0, m.pi / 2))

    rotateTillParallel()
    while True:
        howToTurn = driveTillWall()
        if howToTurn == 1:
            rotateOnCorner()
        elif howToTurn == 2:
            rotateOnTip()
        elif howToTurn == 0:
            break
        else:
            break


def solveMaze(robot: Robot, world: World, v: float = 0.5, d: float = 0.5):
    wander(robot, 0.2, d, True)
    followWall(robot, world, v, d)


if __name__ == '__main__':
    myRobot = Robot.Robot()
    myWorld = labyrinthWorld.buildWorld()
    myWorld.setRobot(myRobot, (2, 14, m.pi / 2))
    # eigene Testwelt erstellt:
    """
     myWorld.addLine(3, 10, 10, 10)
     myWorld.addLine(3, 6, 3, 10)
     myWorld.addLine(10, 6, 10, 10)
     """
    # wander(myRobot, 1)
    # followWall(myRobot, myWorld, 0.5, 0.5)
    solveMaze(myRobot, myWorld, 0.2, 0.5)
    myWorld.close()
