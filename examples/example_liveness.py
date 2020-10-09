import sys
import numpy as np
import cv2
# from matplotlib import pyplot as plt


def help():
    print("python example.py <path to dir with FaceEngine*.so> <path to video>")


if len(sys.argv) != 3:
    help()
    exit(1)

sys.path.append(sys.argv[1])
import FaceEngine as fe
from example_license import make_activation

def print_landmarks(landmarks, message=""):
    print(message)
    for i in range(len(landmarks)):
        print(landmarks[i])


if __name__ == "__main__":
    faceEngine = fe.createFaceEngine("data")

    res = make_activation(faceEngine)
    if res.isError:
        print("Failed to activate license! Reason: {0}".format(res.what))
        exit(1)
    liveness_engine = fe.createLivenessEngine(faceEngine, "./data")
    video_path = sys.argv[2]
    print("downloading of video: {0}".format(video_path))

    # example of config creating
    config_fe = fe.createSettingsProvider("data/faceengine.conf")
    config_le = fe.createSettingsProvider("data/livenessengine.conf")
    aggr = config_le.getValue("Liveness::Infrared", "aggregationThreshold")

    vidcap = cv2.VideoCapture(video_path)
    success = False
    process = True
    liveness = liveness_engine.createLiveness(fe.LA_INFRARED)
    n = 0
    image = fe.Image()
    while process:
        try:
            ret, ir_frame_cv = vidcap.read()
            print(ir_frame_cv.shape)
            if not ret:
                print("Image is empty")
                process = False
                break
            image = fe.Image()
            image.setData(ir_frame_cv, fe.FormatType.R8G8B8)
            # full image saving
            # image.save("path to save/dump/" + str(n) + ".jpg")
            result, liveness_success = liveness.update(image)
            print(image.getWidth(), image.getHeight(), image.getFormat())
            # image_cv = image.getData()
            # plt.imshow(image_cv)
            # plt.xticks([]), plt.yticks([])
            # plt.show()
            print(result.what)
            print(liveness_success)
            if liveness_success:
                success = liveness_success
                process = False
                break
            # result_det, detection = liveness.getDetection()
            # result_warp, warp = liveness.getWarp()
            # if result_warp and warp.getWidth() > 0:
            #     warp_cv = warp.getData()
                # saving of warped image
                # cv2.imwrite("path to save/dump/" + str(n) + ".jpg", warp_cv)
            n += 1
            print(n)
        except Exception as ex:
            print(str(ex))
            exit(-1)

    if success:
        print("Liveness successfull")
    else:
        print("Liveness unsuccessfull")



