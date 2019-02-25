import sys
import numpy as np
import cv2
from matplotlib import pyplot as plt


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

def print_landmarks(landmarks, message=""):
    print(message)
    for i in range(len(landmarks)):
        print(landmarks[i])

print(liveness_engine)

config_fe = fe.createSettingsProvider("data/faceengine.conf")
config_le = fe.createSettingsProvider("data/livenessengine.conf")
config_path = config_fe.getDefaultPath()
config_path = config_le.getDefaultPath()

liveness_engine.setSettingsProvider(config_fe)
path = liveness_engine.getDataDirectory()
print("Path to data directory is: {0}".format(path))
liveness_engine.setDataDirectory(path)

# print(fe.LA_INFRARED)
# print(fe.LSDKError.Ok)
# print(fe.LSDKError.NotInitialized)
# print(fe.LSDKError.NotReady)
# print(fe.LSDKError.PreconditionFailed)
# print(fe.LSDKError.Internal)

vidcap = cv2.VideoCapture("/home/mar/tasks/realsense_demos/video/100_FAS_video_IK/v111.IK_2018-10.avi")
n = 0
success = False
process = True
liveness = liveness_engine.createLiveness(fe.LA_INFRARED)
print("Look straight into the camera")
while process:
    try:
        ret, ir_frame = vidcap.read()
        if not ret:
            print("Image is empty")
            process = False
            break
        image = fe.Image()
        image.setData(ir_frame, fe.FormatType.R8G8B8)
        # image.save("/home/mar/tasks/liveness-bindings/dump/" + str(n) + ".jpg")
        result, liveness_success = liveness.update(image)
        print(result.what)
        print(liveness_success)
        if result.isOk:
            success = liveness_success
            process = False
            break
        result_det, detection = liveness.getDetection()
        result_warp, warp = liveness.getWarp()
        if result_warp and warp.getWidth() > 0:
            warp_cv = warp.getData()
            # to save files
            cv2.imwrite("/home/mar/tasks/liveness-bindings/dump/" + str(n) + ".jpg", warp_cv)
        n += 1
        print(n)
    except Exception as ex:
        print(str(ex))
        exit(-1)

if success:
    print("Liveness successfull")
else:
    print("Liveness unsuccessfull")



