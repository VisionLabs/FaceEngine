import cv2
import sys
import numpy as np


def help():
    print("python example.py <path to dir with FaceEngine*.so> <path to image>")


if len(sys.argv) != 3:
    help()
    exit(1)

sys.path.append(sys.argv[1])
import FaceEngine as fe

faceEngine = fe.createFaceEngine("data",
                                 "data/faceengine.conf")

liveness_engine = fe.createLivenessEngine(faceEngine, "data")
print(liveness_engine)

config = fe.createSettingsProvider("data/faceengine.conf")
config_path = config.getDefaultPath()

liveness_engine.setSettingsProvider(config)
path = liveness_engine.getDataDirectory()
print("Path to data directory is: {0}".format(path))
liveness_engine.setDataDirectory(path)

print(fe.LA_PITCH_DOWN)
print(fe.LA_PITCH_UP)
print(fe.LA_YAW_LEFT)
print(fe.LA_YAW_RIGHT)
print(fe.LA_SMILE)
print(fe.LA_MOUTH)
print(fe.LA_EYEBROW)
print(fe.LA_EYE)
print(fe.LA_ZOOM)
print(fe.LA_INFRARED)
print(fe.LA_EYEBROW)
print(fe.LA_COUNT)

liveness = liveness_engine.createLiveness(fe.LA_PITCH_DOWN)