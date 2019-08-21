import unittest
import argparse
import sys
import os
from license_helper import make_activation, ActivationLicenseError

# if FaceEngine is not installed within the system, add the directory with FaceEngine*.so to system paths
parser = argparse.ArgumentParser()
parser.add_argument("-b", "--bind-path", type=str,
                    help="path to dir with FaceEngine*.so file - binding of luna-sdk")

args = parser.parse_args()
path_to_binding = args.bind_path
print(path_to_binding)
if not os.path.isdir(path_to_binding):
    print("Directory with FaceEngine*.so was not found.")
    exit(1)


print("Directory {0} with python bindings of FaceEngine was included".format(path_to_binding))
print(sys.argv)

sys.path.append(path_to_binding)

# if FaceEngine is installed only this string of code is required for module importing
import FaceEngine as fe

# erase two first arguments for unittest argument parsing
del(sys.argv[1])
del(sys.argv[1])

faceEngine = fe.createFaceEngine("data", "data/faceengine.conf")

if not make_activation(faceEngine):
    raise ActivationLicenseError("License is not activated!")

test_data_path = "testData"
dataPath = "data"

liveness_engine = fe.createLivenessEngine(faceEngine, "data")


def print_landmarks(landmarks, message=""):
    print(message)
    for i in range(len(landmarks)):
        print(landmarks[i])

print(liveness_engine)

config = fe.createSettingsProvider("data/faceengine.conf")
config_path = config.getDefaultPath()

#implementation
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

print(fe.CLA_DEPTH)

print(fe.LSDKError.Ok)
print(fe.LSDKError.NotInitialized)
print(fe.LSDKError.NotReady)
print(fe.LSDKError.PreconditionFailed)
print(fe.LSDKError.Internal)
angles = fe.Angles()
angles.yaw = 10
angles.pitch = 20
angles.roll = 30
print("angles {0}, {1} {2}".format(angles.yaw, angles.pitch, angles.roll))
scores = fe.Scores()
scores.smile = 0.3
scores.mouth = 0.3
scores.eyebrow = 0.4
print("scores {0}, {1} {2}".format(scores.smile, scores.mouth, scores.eyebrow))
eye_states = fe.EyeStates()
eye_states.left = 0
eye_states.right = 1
print("eye_states {0}, {1} ".format(eye_states.left, eye_states.right))

liveness = liveness_engine.createLiveness(fe.LA_INFRARED)
complex_liveness = liveness_engine.createComplexLiveness(fe.CLA_DEPTH)
image = fe.Image()
image_path = test_data_path + "/image1.ppm"
err = image.load(image_path)
if err.isError:
    exit(-1)
err, success = liveness.update(image)
result_det, detection = liveness.getDetection()
if result_det:
    print(detection)
else:
    print("detection was not got")


result_warp, warp = liveness.getWarp()
print("result_warp success:", result_warp, warp.getWidth(), warp.getHeight())
result, landmarks68 = liveness.getLandmarks68()
print("landmarks68 success:", result)
result, landmarks5 = liveness.getLandmarks5()
print_landmarks(landmarks5, "landmarks5:")
print("landmarks5 success:", result)
result, irisLandmarks = liveness.getIrisLandmarks()
print("Irislandmarks success:", result)
result, angles = liveness.getAngles()
print("Angles success:", result)
result, scores = liveness.getScores()
print("Scores success:", result)
result, eye_states = liveness.getEyestates()
print("Eye_states success:", result)
print(err.what, success)
liveness.reset()

result, detection = complex_liveness.getDetection()
result, warp = complex_liveness.getWarp()
result, landmarks68 = complex_liveness.getLandmarks68()
result, landmarks5 = complex_liveness.getLandmarks5()
result, irisLandmarks = complex_liveness.getIrisLandmarks()
result, angles = complex_liveness.getAngles()
result, scores = complex_liveness.getScores()
result, eye_states = complex_liveness.getEyestates()
complex_liveness.reset()


class TestFaceEngineLiveness(unittest.TestCase):

    def simpleLivenessTest(self, type, path):
        configPath = os.path.join("data", "faceengine.conf")
        config = fe.createSettingsProvider(configPath)
        config.setValue("system", "defaultDetectorType", fe.SettingsProviderValue("FaceDetV1"))
        faceEngine.setSettingsProvider(config)
        liveness = liveness_engine.createLiveness(type)
        # image_list = fe.loadFrameSequence(test_data_path + "/smile.bin")
        image_list = fe.loadFrameSequence(path)
        result = None
        success = False
        for image in image_list:
            if not image.isValid():
                print("Image is not valid")
                continue
            result, success = liveness.update(image)
        self.assertTrue(result.isOk)
        self.assertTrue(success)

    def complexLivenessTest(self, type, color_path, depth_path):
        configPath = os.path.join("data", "faceengine.conf")
        config = fe.createSettingsProvider(configPath)
        config.setValue("system", "defaultDetectorType", fe.SettingsProviderValue("FaceDetV1"))
        faceEngine.setSettingsProvider(config)
        complex_liveness = liveness_engine.createComplexLiveness(type)
        color_image_list = fe.loadFrameSequence(color_path)
        depth_image_list = fe.loadFrameSequence(depth_path)
        result = None
        success = False
        self.assertEqual(len(color_image_list), len(depth_image_list))
        for i, _ in enumerate(color_image_list):
            if not color_image_list[i].isValid():
                print("Image is not valid")
                continue
            if not depth_image_list[i].isValid():
                print("Image is not valid")
                continue
            result, success = complex_liveness.update(color_image_list[i], depth_image_list[i])
        self.assertTrue(result.isOk)
        self.assertTrue(success)

    def test_liveness(self):
        self.simpleLivenessTest(fe.LA_INFRARED, test_data_path + "/infrared.bin")
        self.simpleLivenessTest(fe.LA_YAW_RIGHT, test_data_path + "/yawright.bin")
        self.simpleLivenessTest(fe.LA_YAW_LEFT, test_data_path + "/yawleft.bin")
        self.simpleLivenessTest(fe.LA_PITCH_UP, test_data_path + "/pitchup.bin")
        self.simpleLivenessTest(fe.LA_PITCH_DOWN, test_data_path + "/pitchdown.bin")
        self.simpleLivenessTest(fe.LA_MOUTH, test_data_path + "/mouth.bin")
        self.simpleLivenessTest(fe.LA_EYEBROW, test_data_path + "/eyebrow.bin")
        self.simpleLivenessTest(fe.LA_EYE, test_data_path + "/eye.bin")
        self.simpleLivenessTest(fe.LA_ZOOM, test_data_path + "/zoom.bin")
        self.simpleLivenessTest(fe.LA_SMILE, test_data_path + "/smile.bin")
        self.complexLivenessTest(fe.CLA_DEPTH, test_data_path + "/color.bin", test_data_path + "/depth.bin")


if __name__ == '__main__':
    unittest.main()