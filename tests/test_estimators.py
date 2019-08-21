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
    detector = faceEnginePtr.createDetector(f.FACE_DET_V1)
    detector_result = detector.detectOne(image_det,
                                         image_det.getRect(),
                                         f.DetectionType(f.dtAll))
    # for i, item in enumerate(detector_result, 1):
#     print(i, item)
    return detector_result

# warper example
max_detections = 3
image_det = f.Image()
err = image_det.load("testData/00205_9501_p.ppm")

err, face = detect(image_det, 1)
(detection, landmarks5, landmarks68) = face.detection, \
                                       face.landmarks5_opt.value(), \
                                       face.landmarks68_opt.value()

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

class TestFaceEngineEstimators(unittest.TestCase):

    def test_AttributeEstimator(self):
        attributeEstimator = faceEnginePtr.createAttributeEstimator()
        image = f.Image()
        image.load("testData/00205_9501_p.ppm")
        self.assertTrue(image.isValid())
        attributeRequest = f.AttributeRequest(
            f.AttributeRequest.estimateAge | 
            f.AttributeRequest.estimateGender | 
            f.AttributeRequest.estimateEthnicity
        )
        err, attribute_result = attributeEstimator.estimate(image, attributeRequest)
        self.assertTrue(err.isOk)
        self.assertEqual(attribute_result.gender_opt.value(), 0.0)
        self.assertAlmostEqual(attribute_result.genderScore_opt.value(), 0.01, delta=0.05)
        self.assertAlmostEqual(attribute_result.ethnicity_opt.value().caucasian, 1.0, delta=0.1)
        self.assertAlmostEqual(attribute_result.age_opt.value(), 60.0, delta=2.0)
        err_batch, list_result, aggregate_result = attributeEstimator.estimate([image, image], attributeRequest)
        self.assertTrue(err_batch.isOk)
        print(list_result)
        print(attribute_result)
        for result in list_result:
            self.assertEqual(attribute_result.gender_opt.value(), result.gender_opt.value())
            self.assertEqual(attribute_result.genderScore_opt.value(), result.genderScore_opt.value())
            self.assertEqual(attribute_result.ethnicity_opt.value().caucasian, result.ethnicity_opt.value().caucasian)
            self.assertEqual(attribute_result.age_opt.value(), result.age_opt.value())
            # batch with each of image
            self.assertEqual(aggregate_result.gender_opt.value(), result.gender_opt.value())
            self.assertEqual(aggregate_result.genderScore_opt.value(), result.genderScore_opt.value())
            self.assertEqual(aggregate_result.ethnicity_opt.value().caucasian, result.ethnicity_opt.value().caucasian)
            self.assertEqual(aggregate_result.age_opt.value(), result.age_opt.value())

    def test_QualityEstimator(self):
        qualityEstimator = faceEnginePtr.createQualityEstimator()
        image = f.Image()
        image.load("testData/photo_2017-03-30_14-47-43_p.ppm")
        self.assertTrue(image.isValid())
        err, quality_result = qualityEstimator.estimate(image)
        self.assertTrue(err.isOk)
        self.assertAlmostEqual(quality_result.getQuality(), 0.95, delta=0.15)

    def test_EthnicityEstimator(self):
        logging.info("EthnicityEstimator")
        ethnicityEstimator = faceEnginePtr.createEthnicityEstimator()
        image = f.Image()
        image.load("testData/warp1.ppm")
        self.assertTrue(image.isValid())
        err, ethnicity_result = ethnicityEstimator.estimate(image)
        self.assertTrue(err.isOk)
        self.assertAlmostEqual(ethnicity_result.africanAmerican, 0.0, delta=0.1)
        self.assertAlmostEqual(ethnicity_result.indian, 0.0, delta=0.1)
        self.assertAlmostEqual(ethnicity_result.asian, 0.0, delta=0.1)
        self.assertAlmostEqual(ethnicity_result.caucasian, 1.0, delta=0.1)

    def test_GlassesEstimator(self):
        warp0 = f.Image()
        warp1 = f.Image()
        warp2 = f.Image()
        err = warp0.load("testData/warp_noglasses.jpg")
        self.assertTrue(err.isOk)
        err = warp1.load("testData/warp_eyeglasses.jpg")
        self.assertTrue(err.isOk)
        err = warp2.load("testData/warp_sunglasses.jpg")
        self.assertTrue(err.isOk)
        glassesEstimator = faceEnginePtr.createGlassesEstimator()

        err, glasses_estimation = glassesEstimator.estimate(warp0)
        self.assertTrue(err.isOk)
        self.assertEqual(glasses_estimation, f.GlassesEstimation.NoGlasses)

        err, glasses_estimation = glassesEstimator.estimate(warp1)
        self.assertTrue(err.isOk)
        self.assertEqual(glasses_estimation, f.GlassesEstimation.EyeGlasses)

        err, glasses_estimation = glassesEstimator.estimate(warp2)
        self.assertTrue(err.isOk)
        self.assertEqual(glasses_estimation, f.GlassesEstimation.SunGlasses)

    def test_HeadPoseEstimatorLandmarks(self):
        config = f.createSettingsProvider("data/faceengine.conf")
        config.setValue("HeadPoseEstimator::Settings","useEstimationByImage", f.SettingsProviderValue(0))
        config.setValue("HeadPoseEstimator::Settings","useEstimationByLandmarks", f.SettingsProviderValue(1))
        faceEnginePtr.setSettingsProvider(config)
        image = f.Image()
        image.load("testData/photo_2017-03-30_14-47-43_p.ppm")
        det = faceEnginePtr.createDetector(f.FACE_DET_V1)
        err_temp, face = detect(image, 3)
        (detection, landmarks5, landmarks68) = face.detection, \
                                               face.landmarks5_opt.value(), \
                                               face.landmarks68_opt.value()
        headPoseEstimator = faceEnginePtr.createHeadPoseEstimator()
        err, headPoseEstimation = headPoseEstimator.estimate(landmarks68)
        self.assertTrue(err.isOk)
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
        det = faceEnginePtr.createDetector(f.FACE_DET_V1)
        _, face = detect(image, 3)
        (detection, landmarks5, landmarks68) = face.detection, \
                                               face.landmarks5_opt.value(), \
                                               face.landmarks68_opt.value()
        headPoseEstimator = faceEnginePtr.createHeadPoseEstimator()
        err, headPoseEstimation = headPoseEstimator.estimate(image, detection)
        self.assertTrue(err.isOk)
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
        err, isBlack = blackWhiteEstimator.estimate(image)
        self.assertTrue(err.isOk)
        self.assertFalse(isBlack)

    def test_DepthEstimator(self):
        depthEstimator = faceEnginePtr.createDepthEstimator()
        # depth
        # loadImage - only for depth image downloading saved as binary array
        depthImage = f.loadImage("testData/warp.depth")
        err, depth_result = depthEstimator.estimate(depthImage)
        self.assertTrue(err.isOk)
        # print("Depth estimation result = {0}".format(depth_result))
        self.assertAlmostEqual(depth_result, 1.0, delta=0.01)

    def test_IREstimator(self):
        config = f.createSettingsProvider("data/faceengine.conf")
        
        config.setValue("LivenessIREstimator::Settings", "cooperativeMode", f.SettingsProviderValue(1))
        faceEnginePtr.setSettingsProvider(config)
        iREstimator = faceEnginePtr.createIREstimator()
        
        irImage = f.Image()
        irImage.load("testData/irWarp.ppm")
        self.assertTrue(irImage.isValid())
        err, irRestult = iREstimator.estimate(irImage)
        self.assertTrue(err.isOk)
        # print("irResult = ", irRestult)
        self.assertTrue(irRestult.isReal)
        self.assertAlmostEqual(irRestult.score, 1.0, delta=0.01)


        config.setValue("LivenessIREstimator::Settings", "cooperativeMode", f.SettingsProviderValue(0))
        faceEnginePtr.setSettingsProvider(config)
        iREstimator = faceEnginePtr.createIREstimator()
        
        irImage.load("testData/irWarpNonCooperative.png")
        self.assertTrue(irImage.isValid())
        err, irRestult = iREstimator.estimate(irImage)
        self.assertTrue(err.isOk)
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


        err_temp, face = detect(overlapImage, 1)
        (detection_overlap,
         landmarks5_overlap,
         landmarks68_overlap) = \
            face.detection, \
            face.landmarks5_opt.value(), \
            face.landmarks68_opt.value()

        transformation_overlap = warper.createTransformation(detection_overlap, landmarks5_overlap)
        err1, warped_overlap_image = warper.warp(overlapImage, transformation_overlap)
        self.assertTrue(err1.isOk)
        self.assertTrue(warped_overlap_image.isValid())
        err_smile1, smile_result = smileEstimator.estimate(warped_overlap_image)
        self.assertTrue(err_smile1.isOk)
        self.assertAlmostEqual(smile_result.mouth, 0.0, delta=0.01)
        self.assertAlmostEqual(smile_result.smile, 0.0, delta=0.01)
        self.assertAlmostEqual(smile_result.occlusion, 1.0, delta=0.01)
        err2, face = detect(smileImage, 1)
        self.assertTrue(err2.isOk)
        # self.assertTrue(len(det_list) > 0)
        (detection_smile, landmarks5_smile, landmarks68_smile) = face.detection, \
                                                                 face.landmarks5_opt.value(), \
                                                                 face.landmarks68_opt.value()
        transformation_smile = warper.createTransformation(detection_smile, landmarks5_smile)
        err2, warped_overlap_image = warper.warp(smileImage, transformation_smile)
        self.assertTrue(err2.isOk)
        self.assertTrue(warped_overlap_image.isValid())
        err_smile2, smile_result = smileEstimator.estimate(warped_overlap_image)
        self.assertTrue(err_smile2.isOk)
        self.assertAlmostEqual(smile_result.mouth, 0.0, delta=0.01)
        self.assertAlmostEqual(smile_result.smile, 1.0, delta=0.01)
        self.assertAlmostEqual(smile_result.occlusion, 0.0, delta=0.01)

        _, face = detect(mouthImage, 1)
        (detection_mouth, landmarks5_mouth, landmarks68_mouth) = face.detection, \
                                                                 face.landmarks5_opt.value(), \
                                                                 face.landmarks68_opt.value()
        transformation_mouth = warper.createTransformation(detection_mouth, landmarks5_mouth)
        err3, warped_mouth_image = warper.warp(mouthImage, transformation_mouth)
        self.assertTrue(err3.isOk)
        self.assertTrue(warped_mouth_image.isValid())
        err, mouth_result = smileEstimator.estimate(warped_mouth_image)
        self.assertTrue(err.isOk)
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
        err, faceFlowScore = faceFlowEstimator.estimate(faceFlowImage, sequence)
        self.assertTrue(err.isOk)
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
            cropper = f.EyeCropper()
            eyeRectsByLandmarks68 = cropper.cropByLandmarks68(warp, landmarks68_eyes)
            err_eyes, eyesEstimation = eyeEstimator.estimate(warp, eyeRectsByLandmarks68)
            self.assertTrue(err_eyes.isOk)
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
            err_emotions, emotionsEstimation = emotionsEstimator.estimate(image)
            self.assertTrue(err_emotions.isOk)
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
        reference1.anger = 0.001
        reference1.disgust = 0.001
        reference1.fear = 0.001
        reference1.happiness = 0.999
        reference1.sadness = 0.001
        reference1.surprise = 0.001
        reference1.neutral = 0.001

        reference2.anger = 0.999
        reference2.disgust = 0.001
        reference2.fear = 0.001
        reference2.happiness = 0.001
        reference2.sadness = 0.001
        reference2.surprise = 0.001
        reference2.neutral = 0.001

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
            err, actual = gazeEstimator.estimate(headPoseEstimation, eyesEstimation)
            self.assertTrue(err.isOk)
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
        image = f.Image()
        image.load("testData/photo_2017-03-30_14-47-43_p.ppm")

        reference = f.DetectionFloat()
        refAGS = 0.976
        reference.rect.x = 54.0
        reference.rect.y = 58.0
        reference.rect.width = 135.0
        reference.rect.height = 178.0
        reference.score = 0.999916
        r = estimator.estimate(image, reference);
        self.assertFalse(r[0].isError)
        self.assertAlmostEqual(refAGS, r[1], delta=0.01)
        config.setValue("system", "verboseLogging", f.SettingsProviderValue(0))
        faceEnginePtr.setSettingsProvider(config)

    def test_MouthEstimator(self):
        config = f.createSettingsProvider("data/faceengine.conf")
        config.setValue("system", "betaMode", f.SettingsProviderValue(1))
        config.setValue("system", "verboseLogging", f.SettingsProviderValue(5))
        faceEnginePtr.setSettingsProvider(config)
        estimator = faceEnginePtr.createMouthEstimator()
        warper = faceEnginePtr.createWarper()
        image = f.Image()

        def mouthRunner(imageName, isOpened, isSmiling, isOccluded):
            err = image.load(imageName)
            self.assertTrue(err.isOk)
            detStatus, face = detect(image, 1)
            self.assertTrue(detStatus.isOk)
            (detection, landmarks5, landmarks68) = face.detection, \
                                               face.landmarks5_opt.value(), \
                                               face.landmarks68_opt.value()

            transformation = warper.createTransformation(detection, landmarks5)
            warpStatus, warpedImage = warper.warp(image, transformation)
            self.assertTrue(detStatus.isOk)
            
            status, output = estimator.estimate(warpedImage)
            self.assertTrue(status.isOk)
            self.assertEqual(isOpened, output.isOpened)
            self.assertEqual(isSmiling, output.isSmiling)
            self.assertEqual(isOccluded, output.isOccluded)

        mouthRunner("testData/overlap.ppm", False, False, True)
        mouthRunner("testData/mouth.ppm", False, False, False)
        mouthRunner("testData/emotions1.ppm", False, True, False)
        mouthRunner("testData/child_image1.jpg", True, False, False)
        mouthRunner("testData/warp_obama.jpg", True, True, False)

    def test_IrEyeEstimator(self):
        imagePath = "testData/eyes/IrWarp.png"
        refPath = "testData/eyes/IrEyeRef.txt"

        warp = f.Image()
        warp.load(imagePath, f.FormatType.IR_X8X8X8)

        landmarks68 = f.Landmarks68()
        landmarks5 = f.Landmarks5()

        reference = f.EyesEstimation()

        def read_vector_coords(line):
            tokens = line.strip().split()
            x, y = float(tokens[0]), float(tokens[1])
            return f.Vector2f(x, y)
        def read_states(line):
            tokens = line.strip().split()
            return f.State(int(tokens[0])), f.State(int(tokens[1]))

        with open(refPath) as refFile:
            # read landmarks68
            for i in range(len(landmarks68)):
                landmarks68[i] = read_vector_coords(refFile.readline())
            # read landmarks5
            for i in range(len(landmarks5)):
                landmarks5[i] = read_vector_coords(refFile.readline())
            # read eyes state
            states = read_states(refFile.readline())
            reference.leftEye.state = states[0]
            reference.rightEye.state = states[1]
            #read left iris landmarks
            for i in range(len(reference.leftEye.iris)):
                reference.leftEye.iris[i] = read_vector_coords(refFile.readline())
            # read right iris landmarks
            for i in range(len(reference.rightEye.iris)):
                reference.rightEye.iris[i] = read_vector_coords(refFile.readline())
            # read left eyelid landmarks
            for i in range(len(reference.leftEye.eyelid)):
                reference.leftEye.eyelid[i] = read_vector_coords(refFile.readline())
            # read right eyelid landmarks
            for i in range(len(reference.rightEye.eyelid)):
                reference.rightEye.eyelid[i] = read_vector_coords(refFile.readline())

        # crop
        cropper = f.EyeCropper()
        eyeRectsByLandmarks68 = cropper.cropByLandmarks68(warp, landmarks68)

        # create estimator
        eyeEstimator = faceEnginePtr.createEyeEstimator(f.RecognitionMode.RM_INFRA_RED)

        # estimation
        errEyes, eyesEstimation = eyeEstimator.estimate(warp, eyeRectsByLandmarks68)
        self.assertTrue(errEyes.isOk)

        # validation
        acceptableDiff = 1.0
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

if __name__ == '__main__':
    unittest.main()

