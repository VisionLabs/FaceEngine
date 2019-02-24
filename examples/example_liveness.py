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

print(fe.CLA_DEPTH)
print(fe.CLA_COUNT)

print(fe.LSDKError.Ok)
print(fe.LSDKError.NotInitialized)
print(fe.LSDKError.NotReady)
print(fe.LSDKError.PreconditionFailed)
print(fe.LSDKError.Internal)
angles = fe.Angles()
angles.left = 10
angles.pitch = 20
angles.roll = 30
print("angles {0}, {1} {2}".format(angles.left, angles.pitch, angles.roll))
scores = fe.Scores()
scores.smile = 0.3
scores.mouth = 0.3
scores.eyebrow = 0.4
print("scores {0}, {1} {2}".format(scores.smile, scores.mouth, scores.eyebrow))
eye_states = fe.EyeStates()
eye_states.left = 0
eye_states.right = 1
print("eye_states {0}, {1} ".format(eye_states.left, eye_states.right))

liveness = liveness_engine.createLiveness(fe.LA_PITCH_DOWN)
complex_liveness = liveness_engine.createComplexLiveness(fe.CLA_DEPTH)
image = fe.Image()
image_path = sys.argv[2]
err = image.load(image_path)
if err.isError:
    exit(-1)
err, success = liveness.update(image)
print(err.what, success)
err_complex, success_complex = complex_liveness.update(image, image)
print(err_complex.what, success)

liveness.reset()
got_det, detection = liveness.getDetection()
got_warp, warp = liveness.getWarp()
got, landmarks68 = liveness.getLandmarks68()
got, landmarks5 = liveness.getLandmarks5()
got, irisLandmarks = liveness.getIrisLandmarks()
got, angles = liveness.getAngles()
got, scores = liveness.getScores()
got, eye_states = liveness.getEyestates()

complex_liveness.reset()
got, detection = complex_liveness.getDetection()
got, warp = complex_liveness.getWarp()
got, landmarks68 = complex_liveness.getLandmarks68()
got, landmarks5 = complex_liveness.getLandmarks5()
got, irisLandmarks = complex_liveness.getIrisLandmarks()
got, angles = complex_liveness.getAngles()
got, scores = complex_liveness.getScores()
got, eye_states = complex_liveness.getEyestates()