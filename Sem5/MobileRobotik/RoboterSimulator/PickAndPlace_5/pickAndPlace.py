from RoboterSimulator.Robot_Simulator_V3 import Robot, twoRoomsWorld, World
import math as m
import random as r
import numpy as np
from RoboterSimulator.Kinematik_3 import KinematikRobot as kr
from RoboterSimulator.Robot_Simulator_V3 import geometry as g

boxes_room1 = 3
boxes_room2 = 2
room1_barrier = 8


def removeNone(array: list) -> list:
    return [i for i in array if i is not None]


def diff(theta_stern, theta) -> float:  # Differenz zwischen neuem und alten Orientierungswinkel
    return (theta_stern - theta + m.pi) % (2 * m.pi) - m.pi


def equ(v1, v2, tol) -> bool:
    return abs(v2 - v1) <= tol


def equPoints(p1, p2, tol) -> bool:
    return equ(p1[0], p2[0], tol) and equ(p1[1], p2[1], tol)


def exitOnStall(robot, args) -> None:
    if not robot.move(args):
        exit(-1)


def countBoxes(position: tuple) -> None:
    global boxes_room1, boxes_room2
    if position[0] > room1_barrier:
        boxes_room2 -= 1
    else:
        boxes_room1 -= 1


def getRoom(position: tuple):
    if position[0] > room1_barrier:
        return 1
    else:
        return 2


def prioRoom() -> int:
    global boxes_room1, boxes_room2
    if boxes_room1 > 0:
        return 1
    elif boxes_room1 <= 0 and boxes_room2 >= 0:
        return 2
    else:
        return -1

def border(world: World, robot: Robot):
    line = [[15, 9], [15, 10], [15, 11], [15, 12]]
    line2 = [[4, 3], [4, 1], [4, 2]]
    robot_pose = world.getTrueRobotPose()
    pose = robot_pose[0]
    if prioRoom() == 2 and line2[0] >= pose >= line[0]:
        bounceOffWall(robot, True)


def wander(robot: Robot, world: World) -> None:
    while True:
        boxes = robot.senseBoxes()
        if boxes is not None and len(boxes) > 0:
            moveToBox(robot, world)
            bounceOffWall(robot, True)
            continue
        border(world, robot)
        bounceOffWall(robot)
        exitOnStall(robot, (0.2, 0))  # gerades Fahren


def bounceOffWall(robot: Robot, ignoreDist: bool=False):
    dist = removeNone(robot.sense())
    if (len(dist) > 0 and None not in dist and min(dist) - 0.3 <= 0) or ignoreDist:
        theta = m.radians(r.random() * 90 + 90)  # Intervall Orientierung: [0; 2pi)
        omega = min(m.pi, theta)  # Intervall Winkeldifferenz [-pi; +pi[
        time = theta / omega
        amountSteps = int(abs(time) / robot.getTimeStep())
        for _ in range(amountSteps):
            exitOnStall(robot, (0, omega))


def rotate(robot: Robot, angle: float) -> None:
    omega = min(m.pi, angle)
    time = angle / omega
    amount_of_steps = int(abs(time) / robot.getTimeStep())

    # rotate in direction of box
    for _ in range(amount_of_steps):
        exitOnStall(robot, (0, omega))  # Drehung


def moveToBox(robot: Robot, world: World) -> None:
    while not robot.boxInPickUpPosition():
        boxes = robot.senseBoxes()
        boxDistances = list(map(lambda b: b[0], boxes))  # in Liste packen(map(erstes Element, alle Boxen))
        box = boxes[boxDistances.index(min(boxDistances))]
        angle_box = box[1]  # boxes[distances, angles]
        rotate(robot, -angle_box)

        for _ in range(int(10 / robot.getTimeStep())):
            if robot.boxInPickUpPosition():
                break  # aufhören sich nach vorne zu bewegen
            exitOnStall(robot, (0.2, 0))  # gerade Bewegung zur Box

    print("Robot in pick-up position.")
    # pick up box and bring it to depot
    if robot.pickUpBox():
        countBoxes(world.getTrueRobotPose()[:2])
        print("Robot picked up box.")
        goToDepot(robot, world)


def goToDepot(robot: Robot, world: World):
    polyline = [(3, 12, 1), (3, 6, 1), (3, 3, 1), (5, 3, 1), (8.5, 3, 2), (12, 3, 2), (12, 9, 2), (16, 10, 3),
                (16, 12, 3)]
    origin_point, list_index = followPolyLine(robot, world, [(x, y) for x, y, _ in polyline])
    print("Finished with depot.")
    print("Start returning.")
    backToOriginPosition(robot, world, polyline, origin_point, list_index)
    print("Finished with returning.")


def followPolyLine(robot: Robot, world: World, points: list) -> tuple:
    def getIndexOfShortestDist(p_robot: tuple):
        distances = [g.dist(p, p_robot) for p in points]  # Differenz vom aktuellen und neuen Punkt
        return distances.index(min(distances))  # davon den minimalsten Wert

    pose_robot = world.getTrueRobotPose()  # aktuelle Pose des Roboters
    origin_point = pose_robot
    pos_robot = pose_robot[:2]  # nur x und y des Roboters
    list_index = getIndexOfShortestDist(pos_robot)  # get min point next to robot

    # Roboter muss von seinem Punkt zu dem Punkt der Polyline fahren
    for point in points[list_index:]:  # nächster Punkt

        while not equPoints(pos_robot, point, 0.7):
            pose_robot = world.getTrueRobotPose()
            pos_robot = pose_robot[:2]  # die x- und y-KOs des Roboters
            angle_robot = pose_robot[2]
            print(f"Robot choose {point}.")

            angle_new = diff(m.atan2(point[1] - pos_robot[1], point[0] - pos_robot[0]), angle_robot)
            rotate(robot, angle_new)

            pos_robot = world.getTrueRobotPose()[:2]  # aktuelle Koordinaten Roboter
            for _ in range(int(5 / robot.getTimeStep())):
                if equPoints(pos_robot, point, 0.7):
                    break  # aufhören sich nach vorne zu bewegen
                bounceOffWall(robot)
                exitOnStall(robot, (0.3, 0))
    print("Robot reached Depot.")
    robot.placeBox()
    print("Robot placed box.")
    return origin_point[:2] + [getRoom(world.getTrueRobotPose()[:2])], list_index


def backToOriginPosition(robot: Robot, world: World, points: list, origin_point: tuple, list_index: int):
    points = points[list_index:]
    points.reverse()
    reversedPoints = points + [list(origin_point)]
    print(reversedPoints)
    print(f"{prioRoom()} is prio")
    roomPoints = [(x, y) for x, y, room in reversedPoints if prioRoom() <= room]
    followPolyLine(robot, world, roomPoints)


if __name__ == '__main__':
    myWorld = twoRoomsWorld.buildWorld()
    myRobot = Robot.Robot()
    myWorld.setRobot(myRobot, (3, 8, m.pi / 2))
    wander(myRobot, myWorld)
