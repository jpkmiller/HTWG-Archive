from math import *
from RoboterSimulator.Robot_Simulator_V3 import officeWorld
from RoboterSimulator.Robot_Simulator_V3 import obstacleWorld3
from RoboterSimulator.Robot_Simulator_V3 import Robot
from RoboterSimulator.Robot_Simulator_V3 import sensorUtilities


# Roboter in Office-World positionieren:
myWorld = officeWorld.buildWorld()
myRobot = Robot.Robot()
myWorld.setRobot(myRobot, [2, 5.5, pi/2])

#myWorld = obstacleWorld3.buildWorld()
#myWorld.setRobot(myRobot, [1, 6, 0])

# KeyboardController definieren:
myKeyboardController = myWorld.getKeyboardController()


# Bewege Roboter mit Cursor-Tasten:
while True:
    (motion, boxCmd, exit) = myKeyboardController.getCmd()
    if motion == None:
        break
    myRobot.move(motion)
    dists = myRobot.sense()
    directions = myRobot.getSensorDirections()
    lines_l = sensorUtilities.extractSegmentsFromSensorData(dists, directions)
    lines_g = sensorUtilities.transformPolylinesL2G(lines_l, myWorld.getTrueRobotPose())
    print(lines_l)
    for line in lines_l:
        d, alpha = OB_LineUtilities.getDistanceAndNormAngleToLocalLine(line)
        print("d, alpha", alpha*180/pi, d, line )
    myWorld.drawPolylines(lines_g)

# Simulation schliessen:
myWorld.close(False)


