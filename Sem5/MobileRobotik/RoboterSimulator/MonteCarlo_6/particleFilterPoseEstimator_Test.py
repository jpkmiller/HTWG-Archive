from RoboterSimulator.PoseEstimator import plotUtilities
from RoboterSimulator.Robot_Simulator_V3 import Robot, simpleWorld, World
from RoboterSimulator.Robot_Simulator_V3.graphics import *
import math as m
import ParticleFilterPoseEstimator as pf


def diff(theta_stern, theta) -> float:
    return (theta_stern - theta + m.pi) % (2 * m.pi) - m.pi


def auf1(robot: Robot):
    for motion in createMotionCircle(robot, -180, 0.1):
        robot.move(motion)


def auf2(world: World):
    myGrid = world.getDistanceGrid()
    myGrid.drawGrid()


def createMotionCircle(robot: Robot, theta, factor: float = 0.1):
    theta_radians = m.radians(theta)
    omega = min(m.pi, theta_radians) * factor
    amountOfSteps = int(((abs(theta_radians) / abs(omega)) / robot.getTimeStep()))
    return [(1, omega) for _ in range(amountOfSteps)]


def drawParticles(world: World, particles: list, boxes=None):
    if boxes is None:
        boxes = set()
    x = [0, 0]
    y = [0, 0]
    for box in boxes:
        box.undraw()
    for p in particles:
        x[0] = p[0]
        y[0] = p[1]
        box = Circle(Point(p[0], p[1]), 0.1)
        boxes.add(box)
        box.draw(world._win)
    return boxes


# Aufgabe 4a
def test_movement(robot: Robot, world: World):
    """
    Integrieren Sie mehrere Bewegungsschritte mit integrateMovement. Stellen Sie die Partikelmenge davor und danach dar.
    Überprüfen Sie, dass die Streuung der Partikelmenge zunimmt.
    """
    poseRobot = world.getTrueRobotPose()
    d = 0.1
    poseFrom = (poseRobot[0] - d, poseRobot[1] - d, poseRobot[2] - m.pi / 2)
    poseTo = (poseRobot[0] + d, poseRobot[1] + d, poseRobot[2] + m.pi / 2)
    posEstimator = pf.ParticleFilterPoseEstimator(poseFrom, poseTo, 200)
    motionCircle = createMotionCircle(robot, -180, 0.1)

    alpha_list = robot.getSensorDirections()
    distant_map = world.getDistanceGrid()

    plotUtilities.plotPoseParticles(posEstimator.getParticles())  # Plot zu Beginn

    pos_true = []
    particles = set()
    for idx, motion in enumerate(motionCircle):
        robot.move(motion)
        sigma_motion = robot.getSigmaMotion()
        posEstimator.integrateMovement(motion, sigma_motion)
        particles = drawParticles(world, posEstimator.getParticles(), particles)
        pos_true.append(world.getTrueRobotPose())

        if idx % 10 == 0:
            plotUtilities.plotPoseParticles(posEstimator.getParticles())  # alle 10 Schritte die Partikel anzeigen
            posEstimator.integrateMeasurement(robot.sense(), alpha_list, distant_map)

    plotUtilities.plotPositions(pos_true, 'r')
    plotUtilities.plotPoseParticles(posEstimator.getParticles())  # Sensormessung
    plotUtilities.plotShow()


# Aufgabe 4b
def test_measurement(robot: Robot, world: World):
    """
    Erzeugen Sie eine Partikelmenge und integrieren Sie genau eine Sensormessung.
    Stellen Sie die Partikelmenge davor und danach dar.
    Überprüfen Sie, dass die Streuung der Partikel-menge abnimmt und, dass
    die geschätzte Pose getPose() näher an der tatsächlichen liegt.
    """
    poseRobot = world.getTrueRobotPose()
    d = 0.1
    poseFrom = (poseRobot[0] - d, poseRobot[1] - d, poseRobot[2] - m.pi / 2)
    poseTo = (poseRobot[0] + d, poseRobot[1] + d, poseRobot[2] + m.pi / 2)
    posEstimator = pf.ParticleFilterPoseEstimator(poseFrom, poseTo, 200)
    posEstimator.integrateMeasurement(robot.sense(), robot.getSensorDirections(), world.getDistanceGrid())
    plotUtilities.plotPoseParticles(posEstimator.getParticles())  # Sensormessung
    plotUtilities.plotShow()


# Aufgabe 4c
def test_halfCircle(robot: Robot, world: World):
    poseRobot = world.getTrueRobotPose()
    d = 0.1
    poseFrom = (poseRobot[0] - d, poseRobot[1] - d, poseRobot[2] - m.pi / 2)
    poseTo = (poseRobot[0] + d, poseRobot[1] + d, poseRobot[2] + m.pi / 2)
    posEstimator = pf.ParticleFilterPoseEstimator(poseFrom, poseTo, 200)
    motionCircle = createMotionCircle(robot, -180, 0.1)

    plotUtilities.plotPoseParticles(posEstimator.getParticles())  # Sensormessung

    alpha_list = robot.getSensorDirections()
    distant_map = world.getDistanceGrid()

    pos_true = []
    particles = set()
    for idx, motion in enumerate(motionCircle):
        robot.move(motion)
        sigma_motion = robot.getSigmaMotion()
        posEstimator.integrateMovement(motion, sigma_motion)
        particles = drawParticles(world, posEstimator.getParticles(), particles)
        pos_true.append(world.getTrueRobotPose())

        if idx % 10 == 0:
            plotUtilities.plotPoseParticles(posEstimator.getParticles())
            posEstimator.integrateMeasurement(robot.sense(), alpha_list, distant_map)

    plotUtilities.plotPositions(pos_true, 'r')
    plotUtilities.plotPoseParticles(posEstimator.getParticles())  # Sensormessung
    plotUtilities.plotShow()

    print(f"True robot pose: {world.getTrueRobotPose()}")
    print(f"Estimated robot pose: {posEstimator.getPose()}")


# Aufgabe 4d
def test_dynamicObstacle(robot: Robot, world: World):
    world.addDynObstacleLine(5, 10, 7, 12)
    world.addDynObstacleLine(8, 10, 13, 5)
    plotUtilities.plotPositions(((5, 10), (7, 12)), 'y')
    plotUtilities.plotPositions(((8, 10), (13, 5)), 'y')
    test_halfCircle(robot, world)


if __name__ == '__main__':
    # der Test läuft automatisiert durch. Es muss nur immer auf die Fenster geklickt werden
    myWorld = simpleWorld.buildWorld()
    myRobot = Robot.Robot()
    myWorld.setRobot(myRobot, (3, 4, m.pi / 2))
    auf1(myRobot)
    myWorld.close()

    myWorld = simpleWorld.buildWorld()
    auf2(myWorld)
    myWorld.close()

    tests = [(test_movement, (3, 4, m.pi / 2)), (test_measurement, (3, 4, m.pi / 2)),
             (test_halfCircle, (3, 4, m.pi / 2)), (test_dynamicObstacle, (3, 4, m.pi / 2))]
    for test in tests:
        print(f"Running {test_movement.__name__}")
        myWorld = simpleWorld.buildWorld()
        myRobot = Robot.Robot()
        myWorld.setRobot(myRobot, test[1])
        test[0](myRobot, myWorld)
        myWorld.close(False)
