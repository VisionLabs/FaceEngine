import sys
import unittest
import argparse
import sys
import os
import glob
import logging
import struct

# if FaceEngine is not installed within the system, add the directory with FaceEngine*.so to system paths
parser = argparse.ArgumentParser()
parser.add_argument("-b", "--bind-path", type=str,
                    help="path to FaceEngine*.so file - binding of luna-sdk")

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
import FaceEngine as f

# erase two first arguments for unittest argument parsing
del(sys.argv[1])
del(sys.argv[1])

faceEnginePtr = f.createFaceEngine("data",
                                        "data/faceengine.conf")


# detector test and example
def detect(image_det, max_detections):
    detector = faceEnginePtr.createDetector(f.ODT_MTCNN)
    detector_result = detector.detect(image_det, image_det.getRect(), max_detections)
    # for i, item in enumerate(detector_result, 1):
#     print(i, item)
    return detector_result

# warper example
max_detections = 3
image_det = f.Image()
err = image_det.load("testData/00205_9501_p.ppm")

(detection, landmarks5, landmarks68) = detect(image_det, 3)[0]

warper = faceEnginePtr.createWarper()
transformation = warper.createTransformation(detection, landmarks5)
warpedImage = warper.warp(image_det, transformation)

transformedLandmarks5 = warper.warp(landmarks5, transformation)
transformedLandmarks68 = warper.warp(landmarks68, transformation)


def readGazeEstimation(fileReader):
    gazeEstimation = f.GazeEstimation()
    gazeEstimation.leftEye.yaw = struct.unpack('<f', fileReader.read(4))[0]
    gazeEstimation.leftEye.pitch = struct.unpack('<f', fileReader.read(4))[0]
    gazeEstimation.rightEye.yaw = struct.unpack('<f', fileReader.read(4))[0]
    gazeEstimation.rightEye.pitch = struct.unpack('<f', fileReader.read(4))[0]

    return gazeEstimation


def readHeadPoseEstimation(fileReader):
    headPoseEstimation = f.HeadPoseEstimation()
    headPoseEstimation.yaw = struct.unpack('<f', fileReader.read(4))[0]
    headPoseEstimation.pitch = struct.unpack('<f', fileReader.read(4))[0]
    headPoseEstimation.roll = struct.unpack('<f', fileReader.read(4))[0]

    return headPoseEstimation

def readLandmarks68(fileReader):
    landmarks68 = f.Landmarks68()
    for i in range(len(landmarks68)):
        landmarks68[i] = f.Vector2f(struct.unpack('<f', fileReader.read(4))[0],
                                    struct.unpack('<f', fileReader.read(4))[0])

    return landmarks68


def readLandmarks5(fileReader):
    landmarks5 = f.Landmarks5()
    for i in range(len(landmarks5)):
        landmarks5[i] = f.Vector2f(struct.unpack('<f', fileReader.read(4))[0],
                                    struct.unpack('<f', fileReader.read(4))[0])

    return landmarks5


def readIrisLandmarks(fileReader):
    irisLandmarks = f.IrisLandmarks()
    for i in range(len(irisLandmarks)):
        irisLandmarks[i] = f.Vector2f(struct.unpack('<f', fileReader.read(4))[0],
                                   struct.unpack('<f', fileReader.read(4))[0])

    return irisLandmarks


def readEyelidLandmarks(fileReader):
    eyelidLandmarks = f.EyelidLandmarks()
    for i in range(len(eyelidLandmarks)):
        eyelidLandmarks[i] = f.Vector2f(struct.unpack('<f', fileReader.read(4))[0],
                                      struct.unpack('<f', fileReader.read(4))[0])

    return eyelidLandmarks

class TestFaceEngineRect(unittest.TestCase):

    def test_AttributeEstimator(self):
        attributeEstimator = faceEnginePtr.createAttributeEstimator()
        image = f.Image()
        image.load("testData/00205_9501_p.ppm")
        self.assertTrue(image.isValid())
        attribute_result = attributeEstimator.estimate(image)
        self.assertEqual(attribute_result.gender, 0.0)
        self.assertEqual(attribute_result.glasses, 0.0)
        self.assertAlmostEqual(attribute_result.age, 60.44, delta=0.1)

    def test_QualityEstimator(self):
        qualityEstimator = faceEnginePtr.createQualityEstimator()
        image = f.Image()
        image.load("testData/photo_2017-03-30_14-47-43_p.ppm")
        self.assertTrue(image.isValid())
        quality_result = qualityEstimator.estimate(image)
        self.assertAlmostEqual(quality_result.getQuality(), 0.95, delta=0.15)

    def test_EthnicityEstimator(self):
        logging.info("EthnicityEstimator")
        ethnicityEstimator = faceEnginePtr.createEthnicityEstimator()
        image = f.Image()
        image.load("testData/warp1.ppm")
        self.assertTrue(image.isValid())
        ethnicity_result = ethnicityEstimator.estimate(image)
        self.assertAlmostEqual(ethnicity_result.africanAmerican, 0.0, delta=0.1)
        self.assertAlmostEqual(ethnicity_result.indian, 0.0, delta=0.1)
        self.assertAlmostEqual(ethnicity_result.asian, 0.0, delta=0.1)
        self.assertAlmostEqual(ethnicity_result.caucasian, 1.0, delta=0.1)

    def test_HeadPoseEstimatorLandmarks(self):
        config = f.createSettingsProvider("data/faceengine.conf")
        config.setValue("HeadPoseEstimator::Settings","useEstimationByImage", f.SettingsProviderValue(0))
        config.setValue("HeadPoseEstimator::Settings","useEstimationByLandmarks", f.SettingsProviderValue(1))
        faceEnginePtr.setSettingsProvider(config)
        image = f.Image()
        image.load("testData/photo_2017-03-30_14-47-43_p.ppm")
        det = faceEnginePtr.createDetector(f.ODT_MTCNN)
        det.detect(image, image.getRect(), 1)
        (detection, landmarks5, landmarks68) = detect(image, 3)[0]
        headPoseEstimator = faceEnginePtr.createHeadPoseEstimator()
        headPoseEstimation = headPoseEstimator.estimate(landmarks68)
        expected = f.HeadPoseEstimation()
        expected.roll = 0.3891
        expected.yaw = -2.3956
        expected.pitch = 10.5922
        self.assertAlmostEqual(headPoseEstimation.roll, expected.roll, delta=3.0)
        self.assertAlmostEqual(headPoseEstimation.yaw, expected.yaw, delta=3.0)
        self.assertAlmostEqual(headPoseEstimation.pitch, expected.pitch, delta=3.0)
        # print("Actual values headPoseEstimation by landmarks: {0}".format(headPoseEstimation))
        self.assertEqual(f.FrontalFaceType.FrontalFace1, expected.getFrontalFaceType())

    def test_HeadPoseEstimatorImage(self):
        config = f.createSettingsProvider("data/faceengine.conf")
        config.setValue("HeadPoseEstimator::Settings","useEstimationByImage", f.SettingsProviderValue(1))
        config.setValue("HeadPoseEstimator::Settings","useEstimationByLandmarks", f.SettingsProviderValue(0))
        faceEnginePtr.setSettingsProvider(config)
        image = f.Image()
        image.load("testData/photo_2017-03-30_14-47-43_p.ppm")
        det = faceEnginePtr.createDetector(f.ODT_MTCNN)
        det.detect(image, image.getRect(), 1)
        (detection, landmarks5, landmarks68) = detect(image, 3)[0]
        headPoseEstimator = faceEnginePtr.createHeadPoseEstimator()
        headPoseEstimation = headPoseEstimator.estimate(image, detection)
        expected = f.HeadPoseEstimation()
        expected.roll = 0.3891
        expected.yaw = -2.3956
        expected.pitch = 10.5922
        self.assertAlmostEqual(headPoseEstimation.roll, expected.roll, delta=3.0)
        self.assertAlmostEqual(headPoseEstimation.yaw, expected.yaw, delta=3.0)
        self.assertAlmostEqual(headPoseEstimation.pitch, expected.pitch, delta=3.0)
        # print("Actual values headPoseEstimation by image: {0}".format(headPoseEstimation))
        self.assertEqual(f.FrontalFaceType.FrontalFace1, expected.getFrontalFaceType())

    def test_BlackWhiteEstimator(self):
        blackWhiteEstimator = faceEnginePtr.createBlackWhiteEstimator()
        image = f.Image()
        image.load("testData/warp1.ppm")
        self.assertTrue(image.isValid())
        isBlack = blackWhiteEstimator.estimate(image)
        self.assertFalse(isBlack)

    def test_DepthEstimator(self):
        depthEstimator = faceEnginePtr.createDepthEstimator()
        # depth
        # loadImage - only for depth image downloading
        depthImage = f.loadImage("testData/warp.depth")
        depth_result = depthEstimator.estimate(depthImage)
        # print("Depth estimation result = {0}".format(depth_result))
        self.assertAlmostEqual(depth_result.value, 1.0, delta=0.01)

    def test_IREstimator(self):
        config = f.createSettingsProvider("data/faceengine.conf")
        
        config.setValue("LivenessIREstimator::Settings", "cooperativeMode", f.SettingsProviderValue(1))
        faceEnginePtr.setSettingsProvider(config)
        iREstimator = faceEnginePtr.createIREstimator()
        
        irImage = f.Image()
        irImage.load("testData/irWarp.ppm")
        self.assertTrue(irImage.isValid())
        irRestult = iREstimator.estimate(irImage)
        # print("irResult = ", irRestult)
        self.assertTrue(irRestult.isReal)
        self.assertAlmostEqual(irRestult.score, 1.0, delta=0.01)


        config.setValue("LivenessIREstimator::Settings", "cooperativeMode", f.SettingsProviderValue(0))
        faceEnginePtr.setSettingsProvider(config)
        iREstimator = faceEnginePtr.createIREstimator()
        
        irImage.load("testData/irWarpNonCooperative.png")
        self.assertTrue(irImage.isValid())
        irRestult = iREstimator.estimate(irImage)
        self.assertTrue(irRestult.isReal)
        self.assertAlmostEqual(irRestult.score, 0.9935, delta=0.001)

    def test_SmileEstimator(self):
        warper = faceEnginePtr.createWarper()
        smileEstimator = faceEnginePtr.createSmileEstimator()
        smileImage = f.Image()
        overlapImage = f.Image()
        mouthImage = f.Image()
        overlapImage.load("testData/overlap.ppm")
        self.assertTrue(overlapImage.isValid())
        smileImage.load("testData/smile.ppm")
        self.assertTrue(smileImage.isValid())
        mouthImage.load("testData/mouth.ppm")
        self.assertTrue(mouthImage.isValid())

        (detection_overlap, landmarks5_overlap, landmarks68_overlap) = detect(overlapImage, 1)[0]
        transformation_overlap = warper.createTransformation(detection_overlap, landmarks5_overlap)
        warped_overlap_image = warper.warp(overlapImage, transformation_overlap)
        overlap_result = smileEstimator.estimate(warped_overlap_image)

        self.assertAlmostEqual(overlap_result.mouth, 0.0, delta=0.01)
        self.assertAlmostEqual(overlap_result.smile, 0.0, delta=0.01)
        self.assertAlmostEqual(overlap_result.occlusion, 1.0, delta=0.01)

        (detection_smile, landmarks5_smile, landmarks68_smile) = detect(smileImage, 1)[0]
        transformation_smile = warper.createTransformation(detection_smile, landmarks5_smile)
        warped_smile_image = warper.warp(smileImage, transformation_smile)
        smile_result = smileEstimator.estimate(warped_smile_image)

        self.assertAlmostEqual(smile_result.mouth, 0.0, delta=0.01)
        self.assertAlmostEqual(smile_result.smile, 1.0, delta=0.01)
        self.assertAlmostEqual(smile_result.occlusion, 0.0, delta=0.01)

        (detection_mouth, landmarks5_mouth, landmarks68_mouth) = detect(mouthImage, 1)[0]
        transformation_mouth = warper.createTransformation(detection_mouth, landmarks5_mouth)
        warped_mouth_image = warper.warp(mouthImage, transformation_mouth)
        mouth_result = smileEstimator.estimate(warped_mouth_image)

        self.assertAlmostEqual(mouth_result.mouth, 1.0, delta=0.01)
        self.assertAlmostEqual(mouth_result.smile, 0.0, delta=0.01)
        self.assertAlmostEqual(mouth_result.occlusion, 0.0, delta=0.01)


    def test_FaceFlowEstimator(self):
        faceFlowEstimator = faceEnginePtr.createFaceFlowEstimator()
        faceFlowImage = f.Image()
        faceFlowImage.load("testData/small.ppm")
        sequence = []
        for i in range(10):
            tempImage = f.Image()
            tempImage.load("testData/" + str(i) + "big.ppm")
            sequence.append(tempImage)
        faceFlowScore = faceFlowEstimator.estimate(faceFlowImage, sequence)
        self.assertAlmostEqual(faceFlowScore, 0.9967, delta=0.01)

    def test_EyeEstimator(self):
        eyeEstimator = faceEnginePtr.createEyeEstimator()
        def testImage(landmarksCount, refLeftState, refRightState):

            reference = f.EyesEstimation()
            landmarksCountStr = str(landmarksCount)
            lm68Path = "testData/eyes/image_" + landmarksCountStr + "_wlm.txt"
            imagePath = "testData/eyes/image_WARP.png"
            irisLeft = "testData/eyes/image_" + landmarksCountStr + "_0_iris.pts"
            irisRight = "testData/eyes/image_" + landmarksCountStr + "_1_iris.pts"
            eyelidLeft = "testData/eyes/image_" + landmarksCountStr + "_0_eyelid.pts"
            eyelidRight = "testData/eyes/image_" + landmarksCountStr + "_1_eyelid.pts"

            landmarks68_eyes = f.Landmarks68()
            warp = f.Image()
            warp.load(imagePath)

            def invoke_vector_coords(line):
                line = line.strip().split()
                x, y = float(line[0]), float(line[1])
                return f.Vector2f(x, y)

            with open(lm68Path) as lm68file:
                for i, line in enumerate(lm68file):
                    landmarks68_eyes[i] = invoke_vector_coords(line)

            eyesEstimation = eyeEstimator.estimate(warp, landmarks68_eyes)

            with open(irisLeft, 'r') as irisLeftFile, open(irisRight, 'r') as irisRightFile, \
                    open(eyelidLeft, 'r') as eyelidLeftFile, open(eyelidRight, 'r') as eyelidRightFile:
                pass

                # read iris
                for i, line in enumerate(irisLeftFile):
                    reference.leftEye.iris[i] = invoke_vector_coords(line)
                    # print("irisLeftEye", reference.leftEye.iris[i], eyesEstimation.leftEye.iris[i])

                for i, line in enumerate(irisRightFile):
                    reference.rightEye.iris[i] = invoke_vector_coords(line)
                    # print("irisRightEye", reference.rightEye.iris[i], eyesEstimation.rightEye.iris[i])

                # read eyelid
                for i, line in enumerate(eyelidLeftFile):
                    reference.leftEye.eyelid[i] = invoke_vector_coords(line)
                    # print("eyelidLeftEye", reference.leftEye.eyelid[i], eyesEstimation.leftEye.eyelid[i])

                for i, line in enumerate(eyelidRightFile):
                    reference.rightEye.eyelid[i] = invoke_vector_coords(line)
                    # print("eyelidRightEye", reference.rightEye.eyelid[i], eyesEstimation.rightEye.eyelid[i])

            reference.leftEye.state = refLeftState
            reference.rightEye.state = refRightState

            acceptableDiff = 1.0

            # compare with reference
            self.assertEqual(eyesEstimation.leftEye.state, reference.leftEye.state)
            self.assertEqual(eyesEstimation.rightEye.state, reference.rightEye.state)

            # iris
            for i in range(len(f.IrisLandmarks())):
                self.assertAlmostEqual(eyesEstimation.leftEye.iris[i].x, reference.leftEye.iris[i].x, delta=acceptableDiff)
                self.assertAlmostEqual(eyesEstimation.leftEye.iris[i].y, reference.leftEye.iris[i].y, delta=acceptableDiff)
                self.assertAlmostEqual(eyesEstimation.rightEye.iris[i].x, reference.rightEye.iris[i].x, delta=acceptableDiff)
                self.assertAlmostEqual(eyesEstimation.rightEye.iris[i].y, reference.rightEye.iris[i].y, delta=acceptableDiff)

            # eyelid
            for i in range(len(f.EyelidLandmarks())):
                self.assertAlmostEqual(eyesEstimation.leftEye.eyelid[i].x, reference.leftEye.eyelid[i].x, delta=acceptableDiff)
                self.assertAlmostEqual(eyesEstimation.leftEye.eyelid[i].y, reference.leftEye.eyelid[i].y, delta=acceptableDiff)
                self.assertAlmostEqual(eyesEstimation.rightEye.eyelid[i].x, reference.rightEye.eyelid[i].x, delta=acceptableDiff)
                self.assertAlmostEqual(eyesEstimation.rightEye.eyelid[i].y, reference.rightEye.eyelid[i].y, delta=acceptableDiff)

        testImage(68, f.State.Open, f.State.Open)


    def test_EmotionsEstimator(self):
        def emotions_test(path_to_warp_image, reference, predominant):
            acceptable_diff = 0.1
            image = f.Image()
            image.load(path_to_warp_image)
            self.assertTrue(image.isValid())
            emotionsEstimator = faceEnginePtr.createEmotionsEstimator()
            emotionsEstimation = emotionsEstimator.estimate(image)
            self.assertAlmostEqual(emotionsEstimation.anger, reference.anger, delta=acceptable_diff)
            self.assertAlmostEqual(emotionsEstimation.disgust, reference.disgust, delta=acceptable_diff)
            self.assertAlmostEqual(emotionsEstimation.fear, reference.fear, delta=acceptable_diff)
            self.assertAlmostEqual(emotionsEstimation.happiness, reference.happiness, delta=acceptable_diff)
            self.assertAlmostEqual(emotionsEstimation.sadness, reference.sadness, delta=acceptable_diff)
            self.assertAlmostEqual(emotionsEstimation.surprise, reference.surprise, delta=acceptable_diff)
            self.assertAlmostEqual(emotionsEstimation.sadness, reference.sadness, delta=acceptable_diff)
            self.assertAlmostEqual(emotionsEstimation.neutral, reference.neutral, delta=acceptable_diff)
            # print("Emotions {0}".format(emotionsEstimation))
            # print("Best: {0}".format(emotionsEstimation.getPredominantEmotion()))
            self.assertEqual(emotionsEstimation.getPredominantEmotion(), predominant)
        reference1 = f.EmotionsEstimation()
        reference2 = f.EmotionsEstimation()
        reference1.anger = 0.0022
        reference1.disgust = 0.0000
        reference1.fear = 0.0011
        reference1.happiness = 0.9461
        reference1.sadness = 0.0223
        reference1.surprise = 0.0001
        reference1.neutral = 0.0282

        reference2.anger = 0.8487
        reference2.disgust = 0.0002
        reference2.fear = 0.0052
        reference2.happiness = 0.0507
        reference2.sadness = 0.0826
        reference2.surprise = 0.0003
        reference2.neutral = 0.0123
        emotions_test("testData/emotions1.ppm", reference1, f.Emotions.Happiness)
        emotions_test("testData/emotions2.ppm", reference2, f.Emotions.Anger)

    def test_GazeEstimator(self):
        with open("testData/gaze.bin", "rb") as file:
            eyesEstimation = f.EyesEstimation()
            headPoseEstimation = readHeadPoseEstimation(file)
            eyesEstimation.leftEye.iris = readIrisLandmarks(file)
            eyesEstimation.rightEye.iris = readIrisLandmarks(file)
            landmarks68_gaze = readLandmarks68(file)
            for i in range(6):
                eyesEstimation.leftEye.eyelid[i] = f.Vector2f(landmarks68_gaze[36 + i].x, landmarks68_gaze[36 + i].y)
                eyesEstimation.rightEye.eyelid[i] = f.Vector2f(landmarks68_gaze[42 + i].x, landmarks68_gaze[42 + i].y)
            expected = f.GazeEstimation()
            expected.leftEye.yaw = -9.0405864578
            expected.leftEye.pitch = -2.1464545992
            expected.rightEye.yaw = -4.9038884727
            expected.rightEye.pitch = -0.13287750706
            gazeEstimator = faceEnginePtr.createGazeEstimator()
            actual = gazeEstimator.estimate(headPoseEstimation, eyesEstimation)
            self.assertAlmostEqual(actual.leftEye.yaw, expected.leftEye.yaw, delta=0.01)
            self.assertAlmostEqual(actual.leftEye.pitch, expected.leftEye.pitch, delta=0.01)
            self.assertAlmostEqual(actual.rightEye.yaw, expected.rightEye.yaw, delta=0.01)
            self.assertAlmostEqual(actual.rightEye.pitch, expected.rightEye.pitch, delta=0.01)
            # print("GazeEstimation actual = {0}".format(actual))

    def test_AGSEstimator(self):
        config = f.createSettingsProvider("data/faceengine.conf")
        config.setValue("system", "betaMode", f.SettingsProviderValue(1))
        config.setValue("system", "verboseLogging", f.SettingsProviderValue(5))
        faceEnginePtr.setSettingsProvider(config)
        estimator = faceEnginePtr.createAGSEstimator()
        image = f.Image();
        image.load("testData/photo_2017-03-30_14-47-43_p.ppm");

        reference = f.Detection()
        refAGS = 0.977100
        reference.rect.x = 54
        reference.rect.y = 58
        reference.rect.width = 135
        reference.rect.height = 178
        reference.score = 0.999916

        r = estimator.estimate(image, reference);
        self.assertFalse(r[0].isError)
        self.assertAlmostEqual(refAGS, r[1], delta=0.0001)
        config.setValue("system", "verboseLogging", f.SettingsProviderValue(0))
        faceEnginePtr.setSettingsProvider(config)

if __name__ == '__main__':
    unittest.main()

