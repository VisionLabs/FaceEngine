import sys
import unittest
import argparse
import sys
import os
import logging
import struct
from license_helper import make_activation, ActivationLicenseError

# if FaceEngine is not installed within the system, add the directory with FaceEngine*.so to system paths
parser = argparse.ArgumentParser()
parser.add_argument("-b", "--bind-path", type=str,
                    help="path to dir with FaceEngine*.so file - binding of luna-sdk")

args = parser.parse_args()

if len(sys.argv) == 1 or not args.bind_path or not os.path.isdir(args.bind_path):
    parser.print_help(sys.stderr)
    sys.exit(1)

path_to_binding = args.bind_path
print("Directory {0} with python bindings of FaceEngine was included".format(path_to_binding))
print(sys.argv)

sys.path.append(path_to_binding)

# if FaceEngine is installed only this string of code is required for module importing
import FaceEngine as f

# erase two first arguments for unittest argument parsing
del (sys.argv[1])
del (sys.argv[1])


def invokeFileLineToVector2f(line):
    line = line.strip().split()
    x, y = float(line[0]), float(line[1])
    return f.Vector2f(x, y)


def invokeFileLineToList(line):
    line = line.strip().split()
    x, y = float(line[0]), float(line[1])
    return [x, y]


# detector test and example
def detect(_image_det, _faceEngine):
    detector = _faceEngine.createDetector(f.FACE_DET_V1)
    detector_result = detector.detectOne(_image_det,
                                         _image_det.getRect(),
                                         f.DetectionType(f.dtAll))
    # for i, item in enumerate(detector_result, 1):
    # print(i, item)
    return detector_result


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
    faceEngine = None
    warper = None

    @classmethod
    def setUp(cls):
        cls.faceEngine = f.createFaceEngine("data")
        if not make_activation(cls.faceEngine):
            raise ActivationLicenseError("License is not activated!")
        cls.warper = cls.faceEngine.createWarper()

    def testAttributeEstimator(self):
        attributeEstimator = self.faceEngine.createAttributeEstimator()
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
        for i in (aggregate_result, attribute_result):
            with self.subTest(i=i):
                for result in list_result:
                    self.assertResult(i, result)

    def assertResult(self, result1, result):
        self.assertEqual(result1.gender_opt.value(), result.gender_opt.value())
        self.assertEqual(result1.genderScore_opt.value(), result.genderScore_opt.value())
        self.assertEqual(result1.ethnicity_opt.value().caucasian, result.ethnicity_opt.value().caucasian)
        self.assertEqual(result1.age_opt.value(), result.age_opt.value())

    def testQualityEstimator(self):
        qualityEstimator = self.faceEngine.createQualityEstimator()
        image = f.Image()
        image.load("testData/photo_2017-03-30_14-47-43_p.ppm")
        self.assertTrue(image.isValid())
        err, subj_quality_result = qualityEstimator.estimate(image)
        self.assertTrue(err.isOk)
        self.assertTrue(subj_quality_result.isGood())
        err1, subj_quality_result1 = qualityEstimator.estimate_subjective_quality(image)
        self.assertTrue(err1.isOk)
        self.assertTrue(subj_quality_result1.isGood())
        #compare estimate_subjective_quality vs estimate 
        floatprecision = 0.0001
        self.assertAlmostEqual(subj_quality_result.blur, subj_quality_result1.blur, floatprecision)
        self.assertAlmostEqual(subj_quality_result.light, subj_quality_result1.light, floatprecision)
        self.assertAlmostEqual(subj_quality_result.darkness, subj_quality_result1.darkness, floatprecision)
        self.assertAlmostEqual(subj_quality_result.illumination, subj_quality_result1.illumination, floatprecision)
        self.assertAlmostEqual(subj_quality_result.specularity, subj_quality_result1.specularity, floatprecision)
        #test estimate_quality
        qualityRef = f.Quality()
        qualityRef.light = 0.96277028322
        qualityRef.dark  = 0.974349558353
        qualityRef.gray  = 0.979210078716
        qualityRef.blur  = 0.961572766304
        refPrecision = 0.001
        err, quality = qualityEstimator.estimate_quality(image)
        self.assertAlmostEqual(quality.light, qualityRef.light, refPrecision)
        self.assertAlmostEqual(quality.dark, qualityRef.dark, refPrecision)
        self.assertAlmostEqual(quality.gray, qualityRef.gray, refPrecision)
        self.assertAlmostEqual(quality.blur, qualityRef.blur, refPrecision)

    def testOverlapEstimator(self):
        image = f.Image()
        err = image.load("testData/overlap_image1.jpg")
        self.assertTrue(err.isOk)
        overlap_estimator = self.faceEngine.createOverlapEstimator()
        detection = f.DetectionFloat()
        detection.score = 0.999
        detection.rect = f.RectFloat(0, 0, 240, 240)
        err, overlap_estimation = overlap_estimator.estimate(image, detection)
        self.assertTrue(err.isOk)
        self.assertAlmostEqual(0.996921, overlap_estimation.overlapValue, delta=0.1)
        self.assertTrue(overlap_estimation.overlapped)

    def testGlassesEstimator(self):
        params = {f.GlassesEstimation.NoGlasses: "testData/warp_noglasses.jpg", f.GlassesEstimation.EyeGlasses: "testData/warp_eyeglasses.jpg", f.GlassesEstimation.SunGlasses: "testData/warp_sunglasses.jpg"}
        for glasses_state, image_path in params.items():
            with self.subTest(type=glasses_state):
                warp = f.Image()
                err = warp.load(image_path)
                self.assertTrue(err.isOk)
                glassesEstimator = self.faceEngine.createGlassesEstimator()
                err, glasses_estimation = glassesEstimator.estimate(warp)
                self.assertTrue(err.isOk)
                self.assertEqual(glasses_estimation, glasses_state)

    def testHeadPoseEstimator(self):
        for i in ('landmarks', 'image'):
            with self.subTest(i=i):
                config = f.createSettingsProvider("data/faceengine.conf")
                if i == 'landmarks':
                    config.setValue("HeadPoseEstimator::Settings", "useEstimationByLandmarks", f.SettingsProviderValue(1))
                else:
                    config.setValue("HeadPoseEstimator::Settings", "useEstimationByImage", f.SettingsProviderValue(1))
                self.faceEngine.setSettingsProvider(config)
                image = f.Image()
                image.load("testData/photo_2017-03-30_14-47-43_p.ppm")
                face_err, face = detect(image, self.faceEngine)
                self.assertTrue(face_err.isOk)
                (detection, landmarks5, landmarks68) = face.detection, \
                                                       face.landmarks5_opt.value(), \
                                                       face.landmarks68_opt.value()
                headPoseEstimator = self.faceEngine.createHeadPoseEstimator()
                if i == 'landmarks':
                    err, headPoseEstimation = headPoseEstimator.estimate(landmarks68)
                else:
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

    def testBlackWhiteEstimator(self):
        blackWhiteEstimator = self.faceEngine.createBlackWhiteEstimator()
        params = {"white": ["testData/warp1.ppm", False],
                  "black": ["testData/2702096048_1.jpg", True]}
        for color, value in params.items():
            path_to_img, result = value
            with self.subTest(type=color):
                image = f.Image()
                image.load(path_to_img)
                self.assertTrue(image.isValid())
                err, isBlack = blackWhiteEstimator.estimate(image)
                self.assertTrue(err.isOk)
                self.assertEqual(isBlack, result)

    def testDepthEstimator(self):
        depthEstimator = self.faceEngine.createDepthEstimator()
        reference = f.DepthEstimation(0.9397, True)

        depthImage = f.Image()
        depthImage.load("testData/warpeddepth9397.png")
        err, depth_result = depthEstimator.estimate(depthImage)
        self.assertTrue(err.isOk)
        self.assertEqual(depth_result.isReal, reference.isReal)
        self.assertAlmostEqual(depth_result.score, reference.score, delta=0.001)

        depthImage = f.Image()
        depthImage.load("testData/bad_depth_warp.png")
        err, depth_result = depthEstimator.estimate(depthImage)
        self.assertTrue(err.isError)
        self.assertEqual(err.error, f.FSDKError.InvalidInput)

    def testIREstimatorUniversal(self):
        config = f.createSettingsProvider("data/faceengine.conf")
        config.setValue("LivenessIREstimator::Settings", "name", f.SettingsProviderValue("universal"))
        self.faceEngine.setSettingsProvider(config)
        iREstimator = self.faceEngine.createIREstimator()

        irImage = f.Image()
        irImage.load("testData/ir_Universal_real.jpg")
        self.assertTrue(irImage.isValid())
        err, irRestult = iREstimator.estimate(irImage)
        self.assertTrue(err.isOk)
        # print("irResult = ", irRestult)
        self.assertTrue(irRestult.isReal)
        self.assertAlmostEqual(irRestult.score, 0.9999, delta=0.01)

        irImage.load("testData/ir_Universal_fake.jpg")
        self.assertTrue(irImage.isValid())
        err, irRestult = iREstimator.estimate(irImage)
        self.assertTrue(err.isOk)
        # print("irResult = ", irRestult)
        self.assertFalse(irRestult.isReal)
        self.assertAlmostEqual(irRestult.score, 0.2499, delta=0.01)

    def testIREstimatorAmbarella(self):
        config = f.createSettingsProvider("data/faceengine.conf")

        config.setValue("LivenessIREstimator::Settings", "name", f.SettingsProviderValue("ambarella"))
        self.faceEngine.setSettingsProvider(config)
        iREstimator = self.faceEngine.createIREstimator()

        irImage = f.Image()
        irImage.load("testData/ir_Ambarella_real.jpg")
        self.assertTrue(irImage.isValid())
        err, irRestult = iREstimator.estimate(irImage)
        self.assertTrue(err.isOk)
        self.assertTrue(irRestult.isReal)
        self.assertAlmostEqual(irRestult.score, 0.8933, delta=0.01)

        irImage.load("testData/ir_Ambarella_fake.jpg")
        self.assertTrue(irImage.isValid())
        err, irRestult = iREstimator.estimate(irImage)
        self.assertTrue(err.isOk)
        self.assertFalse(irRestult.isReal)
        self.assertAlmostEqual(irRestult.score, 0.6871, delta=0.01)

    def testSmileEstimator(self):
        smileEstimator = self.faceEngine.createSmileEstimator()
        mouth_params = {"testData/overlap.ppm": [0.0, 0.0, 1.0], "testData/smile.ppm": [0.0, 1.0, 0.0], "testData/mouth.ppm": [1.0, 0.0, 0.0]}
        for image_path, score in mouth_params.items():
            mouth, smile, occlusion = score
            with self.subTest(image=image_path):
                image = f.Image()
                image.load(image_path)
                self.assertTrue(image.isValid())

                err_temp, face = detect(image, self.faceEngine)
                self.assertTrue(err_temp.isOk)
                (detection, landmarks5, landmarks68) = face.detection, face.landmarks5_opt.value(), face.landmarks68_opt.value()
                transformation = self.warper.createTransformation(detection, landmarks5)
                err1, warped_image = self.warper.warp(image, transformation)
                self.assertTrue(err1.isOk)
                self.assertTrue(warped_image.isValid())
                err_result, smile_result = smileEstimator.estimate(warped_image)
                self.assertTrue(err_result.isOk)
                self.assertAlmostEqual(smile_result.mouth, mouth, delta=0.01)
                self.assertAlmostEqual(smile_result.smile, smile, delta=0.01)
                self.assertAlmostEqual(smile_result.occlusion, occlusion, delta=0.01)

    def testFaceFlowEstimator(self):
        faceFlowEstimator = self.faceEngine.createFaceFlowEstimator()
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

    def testLivenessFlyingFlowEstimator(self):
        flying_faces_estimator = self.faceEngine.createLivenessFlyingFacesEstimator()
        image = f.Image()
        image.load("testData/0_Parade_Parade_0_12.jpg")
        face = f.Face()
        face.detection.rect = f.RectFloat(728, 131, 44, 63)
        face.detection.score = 0.999
        face.img = image
        faces = [face, face]
        err, flying_faces_estimation = flying_faces_estimator.estimate(face)
        errs, flying_faces_estimations = flying_faces_estimator.estimate(faces)
        self.assertTrue(err.isOk)
        self.assertTrue(errs.isOk)
        self.assertAlmostEqual(flying_faces_estimation.score, 0.9981, delta=0.001)
        self.assertAlmostEqual(flying_faces_estimations[0].score, 0.9981, delta=0.001)
        self.assertAlmostEqual(flying_faces_estimations[1].score, 0.9981, delta=0.001)
        self.assertTrue(flying_faces_estimation.isReal)

    def testLivenessRGBMEstimator(self):
        estimator = self.faceEngine.createLivenessRGBMEstimator()
        background = f.Image()
        load_err = background.load("testData/rgbm_liveness_background_real.png")
        self.assertTrue(load_err.isOk)
        frame = f.Image()
        load_err = frame.load("testData/rgbm_liveness_frame_real.png")
        self.assertTrue(load_err.isOk)
        detection = f.DetectionFloat()
        detection.rect = f.RectFloat(315, 206, 175, 221)
        detection.score = 0.999
        estimation_err, estimation = estimator.estimate(frame, detection, background)
        self.assertTrue(estimation_err.isOk)
        self.assertTrue(estimation.isReal)
        self.assertAlmostEqual(estimation.score, 0.8281, delta=0.001)

    def testEyeEstimator(self):

        landmarks = (5, 68)
        for landmarksCount in landmarks:
            with self.subTest(landmarks=landmarks):
                self.eyesTest(landmarksCount, f.State.Open, f.State.Open)

    def eyesTest(self, landmarksCount, refLeftState, refRightState):
        eyeEstimator = self.faceEngine.createEyeEstimator()
        reference = f.EyesEstimation()
        landmarksCountStr = str(landmarksCount)
        lmPath = "testData/eyes/image_" + landmarksCountStr + "_wlm.txt"
        imagePath = "testData/eyes/image_WARP.png"
        irisLeft = "testData/eyes/image_" + landmarksCountStr + "_0_iris.pts"
        irisRight = "testData/eyes/image_" + landmarksCountStr + "_1_iris.pts"
        eyelidLeft = "testData/eyes/image_" + landmarksCountStr + "_0_eyelid.pts"
        eyelidRight = "testData/eyes/image_" + landmarksCountStr + "_1_eyelid.pts"

        if landmarksCount == 68:
            landmarks_eyes = f.Landmarks68()
        else:
            landmarks_eyes = f.Landmarks5()
        warp = f.Image()
        warp.load(imagePath)

        with open(lmPath) as lmfile:
            for i, line in enumerate(lmfile):
                landmarks_eyes[i] = invokeFileLineToVector2f(line)
        cropper = f.EyeCropper()
        if landmarksCount == 68:
            eyeRectsByLandmarks = cropper.cropByLandmarks68(warp, landmarks_eyes)
        else:
            eyeRectsByLandmarks = cropper.cropByLandmarks5(warp, landmarks_eyes)
        err_eyes, eyesEstimation = eyeEstimator.estimate(warp, eyeRectsByLandmarks)
        self.assertTrue(err_eyes.isOk)
        with open(irisLeft, 'r') as irisLeftFile, open(irisRight, 'r') as irisRightFile, \
                open(eyelidLeft, 'r') as eyelidLeftFile, open(eyelidRight, 'r') as eyelidRightFile:
            # read iris
            for i, line in enumerate(irisLeftFile):
                reference.leftEye.iris[i] = invokeFileLineToVector2f(line)
                # print("irisLeftEye", reference.leftEye.iris[i], eyesEstimation.leftEye.iris[i])

            for i, line in enumerate(irisRightFile):
                reference.rightEye.iris[i] = invokeFileLineToVector2f(line)
                # print("irisRightEye", reference.rightEye.iris[i], eyesEstimation.rightEye.iris[i])

            # read eyelid
            for i, line in enumerate(eyelidLeftFile):
                reference.leftEye.eyelid[i] = invokeFileLineToVector2f(line)
                # print("eyelidLeftEye", reference.leftEye.eyelid[i], eyesEstimation.leftEye.eyelid[i])

            for i, line in enumerate(eyelidRightFile):
                reference.rightEye.eyelid[i] = invokeFileLineToVector2f(line)
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

    def testEmotionsEstimator(self):
        reference1 = f.EmotionsEstimation()
        reference2 = f.EmotionsEstimation()
        reference3 = f.EmotionsEstimation()
        reference4 = f.EmotionsEstimation()
        reference5 = f.EmotionsEstimation()
        reference6 = f.EmotionsEstimation()
        reference7 = f.EmotionsEstimation()

        reference1.anger = 0.999
        reference2.disgust = 0.999
        reference3.fear = 0.884
        reference3.surprise = 0.115
        reference4.happiness = 0.999
        reference5.sadness = 0.999
        reference6.surprise = 0.999
        reference7.neutral = 0.999

        emotions = {f.Emotions.Anger: ["testData/emotions2.ppm", reference1],
                    f.Emotions.Disgust: ["testData/00205_9501_p.ppm", reference2],
                    f.Emotions.Fear: ["testData/strah.jpg", reference3],
                    f.Emotions.Happiness: ["testData/emotions1.ppm", reference4],
                    f.Emotions.Sadness: ["testData/ethnicity1_white.jpg", reference5],
                    f.Emotions.Surprise: ["testData/udivlenie.jpg", reference6],
                    f.Emotions.Neutral: ["testData/attrib1.jpg", reference7]}

        for predominant, value in emotions.items():
            path_to_img, reference = value
            with self.subTest(type=predominant):
                acceptable_diff = 0.1
                image = f.Image()
                image.load(path_to_img)
                self.assertTrue(image.isValid())
                emotionsEstimator = self.faceEngine.createEmotionsEstimator()
                err_emotions, emotionsEstimation = emotionsEstimator.estimate(image)
                self.assertTrue(err_emotions.isOk)
                self.assertAlmostEqual(emotionsEstimation.anger, reference.anger, delta=acceptable_diff)
                self.assertAlmostEqual(emotionsEstimation.disgust, reference.disgust, delta=acceptable_diff)
                self.assertAlmostEqual(emotionsEstimation.fear, reference.fear, delta=acceptable_diff)
                self.assertAlmostEqual(emotionsEstimation.happiness, reference.happiness, delta=acceptable_diff)
                self.assertAlmostEqual(emotionsEstimation.sadness, reference.sadness, delta=acceptable_diff)
                self.assertAlmostEqual(emotionsEstimation.surprise, reference.surprise, delta=acceptable_diff)
                self.assertAlmostEqual(emotionsEstimation.neutral, reference.neutral, delta=acceptable_diff)
                # print("Emotions {0}".format(emotionsEstimation))
                # print("Best: {0}".format(emotionsEstimation.getPredominantEmotion()))
                self.assertEqual(emotionsEstimation.getPredominantEmotion(), predominant)

    def testGazeEstimatorRMIRGB(self):
        gaze_estimator_rgb = self.faceEngine.createGazeEstimator()
        image_path = "00205_9501_p.ppm"
        warp = f.Image()
        image_name = image_path[:-4]
        err_load = warp.load("testData/" + image_path)
        self.assertTrue(err_load.isOk)
        lm5_path = "testData/gaze/" + image_name + "_landmarks5.pts"
        lm5_rotated_path = "testData/gaze/" + image_name + "_rotatedlandmarks5.pts"
        actual_path = "testData/gaze/" + image_name + "_actual.txt"
        actual_path_unwarped = "testData/gaze/" + image_name + "_actual_unwarped.txt"
        landmarks5 = f.Landmarks5()
        rotated_landmarks5 = f.Landmarks5()

        with open(lm5_path) as lm5file:
            for i, line in enumerate(lm5file):
                landmarks5[i] = invokeFileLineToVector2f(line)

        with open(lm5_rotated_path) as lm_rotated5_file:
            for i, line in enumerate(lm_rotated5_file):
                rotated_landmarks5[i] = invokeFileLineToVector2f(line)

        actual = []
        with open(actual_path) as actual_file:
            for line in actual_file:
                actual = invokeFileLineToList(line)

        actual_unwarped = []
        with open(actual_path_unwarped) as actual_file_unwarped:
            for line in actual_file_unwarped:
                actual_unwarped = invokeFileLineToList(line)

        err, eye_angles = gaze_estimator_rgb.estimate(warp, rotated_landmarks5)
        self.assertTrue(err.isOk)
        self.assertAlmostEqual(eye_angles.yaw, actual[0], delta=0.1)
        self.assertAlmostEqual(eye_angles.pitch, actual[1], delta=0.1)
        detection = f.DetectionFloat()
        rect = warp.getRect()
        detection.rect = f.RectFloat(rect.x, rect.y, rect.width, rect.height)
        transformation = self.warper.createTransformation(detection, landmarks5)
        err_unwarp, eye_angles_umwarped = self.warper.unwarp(eye_angles, transformation)
        self.assertTrue(err_unwarp.isOk)
        self.assertAlmostEqual(eye_angles_umwarped.yaw, actual_unwarped[0], delta=0.1)
        self.assertAlmostEqual(eye_angles_umwarped.pitch, actual_unwarped[1], delta=0.1)

    def testAGSEstimator(self):
        config = f.createSettingsProvider("data/faceengine.conf")
        config.setValue("system", "betaMode", f.SettingsProviderValue(1))
        config.setValue("system", "verboseLogging", f.SettingsProviderValue(5))
        self.faceEngine.setSettingsProvider(config)
        estimator = self.faceEngine.createAGSEstimator()
        image = f.Image()
        image.load("testData/photo_2017-03-30_14-47-43_p.ppm")

        reference = f.DetectionFloat()
        refAGS = 0.976
        reference.rect.x = 54.0
        reference.rect.y = 58.0
        reference.rect.width = 135.0
        reference.rect.height = 178.0
        reference.score = 0.999916
        r = estimator.estimate(image, reference)
        self.assertFalse(r[0].isError)
        self.assertAlmostEqual(refAGS, r[1], delta=0.01)
        config.setValue("system", "verboseLogging", f.SettingsProviderValue(0))
        self.faceEngine.setSettingsProvider(config)

    def testMouthEstimator(self):
        estimator = self.faceEngine.createMouthEstimator()
        warper = self.faceEngine.createWarper()
        image = f.Image()
        params = {"testData/overlap.ppm": [False, False, True], "testData/mouth.ppm": [False, False, False],
                  "testData/emotions1.ppm": [False, True, False],
                  "testData/child_image1.jpg": [True, False, False], "testData/warp_obama.jpg": [True, True, False]}
        for path_to_img, value in params.items():
            isOpened, isSmiling, isOccluded = value
            with self.subTest(image=path_to_img):
                err = image.load(path_to_img)
                self.assertTrue(err.isOk)
                detStatus, face = detect(image, self.faceEngine)
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

    def testMedicalMaskEstimator(self):
        estimator = self.faceEngine.createMedicalMaskEstimator()
        warper = self.faceEngine.createWarper()
        image = f.Image()
        params = {"testData/mask.png": [True, False, False, False], "testData/nomask.png": [False, False, False, True]}
        for path_to_img, valueList in params.items():
            with self.subTest(image=path_to_img):
                err = image.load(path_to_img)
                self.assertTrue(err.isOk)
                detection = f.DetectionFloat(image.getRect(), 1.0)
                status1, output1 = estimator.estimate(image)
                status2, output2 = estimator.estimate(image, detection)
                status3, output3 = estimator.estimate([image, image], [detection, detection])
                status4, output4 = estimator.estimate([image, image])

                def assertMedicalMask(status, output, value):
                    self.assertTrue(status.isOk)
                    self.assertEqual(value[0], output.isMaskInPlace)
                    self.assertEqual(value[1], output.isMaskNotInPlace)
                    self.assertEqual(value[2], output.isNoMask)
                    self.assertEqual(value[3], output.isOccludedFace)
                    
                assertMedicalMask(status1, output1, valueList)
                assertMedicalMask(status2, output2, valueList)
                assertMedicalMask(status3, output3[0], valueList)
                assertMedicalMask(status4, output4[0], valueList)
                assertMedicalMask(status3, output3[1], valueList)
                assertMedicalMask(status4, output4[1], valueList)

    def testIrEyeEstimator(self):
        imagePath = "testData/eyes/IrWarp.png"
        warp = f.Image()
        warp.load(imagePath, f.FormatType.IR_X8X8X8)

        def read_vector_coords(line):
            tokens = line.strip().split()
            x, y = float(tokens[0]), float(tokens[1])
            return f.Vector2f(x, y)

        def read_states(line):
            tokens = line.strip().split()
            return f.State(int(tokens[0])), f.State(int(tokens[1]))

        params = {5: "testData/eyes/IrEyeRef_5.txt", 68: "testData/eyes/IrEyeRef_68.txt"}

        for lm, refPath in params.items():
            with self.subTest(landmarks=lm):
                if lm == 68:
                    landmarks = f.Landmarks68()
                else:
                    landmarks = f.Landmarks5()

                reference = f.EyesEstimation()

                with open(refPath) as refFile:
                    # read landmarks68 or landmarks5
                    for i in range(len(landmarks)):
                        landmarks[i] = read_vector_coords(refFile.readline())
                    # read eyes state
                    states = read_states(refFile.readline())
                    reference.leftEye.state = states[0]
                    reference.rightEye.state = states[1]
                    # read left iris landmarks
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
                    if lm == 68:
                        eyeRectsByLandmarks = cropper.cropByLandmarks68(warp, landmarks)
                    else:
                        eyeRectsByLandmarks = cropper.cropByLandmarks5(warp, landmarks)

                    # create estimator
                    eyeEstimator = self.faceEngine.createEyeEstimator(f.RecognitionMode.RM_INFRA_RED)

                    # estimation
                    errEyes, eyesEstimation = eyeEstimator.estimate(warp, eyeRectsByLandmarks)
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

    def testSimpleOptionalType(self):
        value = 5.0
        x = f.Optionalfloat(value)
        self.assertTrue(x.isValid())
        y = f.Optionalfloat()
        self.assertFalse(y.isValid())
        y.set(value)
        self.assertTrue(y.isValid())
        self.assertEqual(x.value(), value)
        self.assertEqual(y.value(), value)
        eth_value = f.EthnicityEstimation()
        eth_value.africanAmerican = 0.3
        eth_value.indian = 0.8
        eth1 = f.OptionalEthnicityEstimation(eth_value)
        self.assertTrue(eth1.isValid())
        eth2 = f.OptionalEthnicityEstimation()
        self.assertFalse(eth2.isValid())
        eth2.set(eth_value)
        self.assertTrue(eth2.isValid())
        self.assertTrue(eth1.value().indian != 0.0)
        self.assertTrue(eth1.value().africanAmerican != 0.0)
        self.assertEqual(eth1.value().getPredominantEthnicity(), eth_value.getPredominantEthnicity())
        self.assertEqual(eth2.value().getPredominantEthnicity(), eth_value.getPredominantEthnicity())

    def testInvalidDetectionAreas(self):
        image = f.Image()
        err = image.load("testData/overlap_image1.jpg")
        self.assertTrue(err.isOk)
        areas = {"big_area": f.DetectionFloat(f.RectFloat(0, 0, 10 ** 9, 10 ** 9), 0.999916),
                 "bad_area": f.DetectionFloat(f.RectFloat(120, 599, 15, 10), 0.999916)}
        estimator_types = ('RGBM', 'AGS', 'HeadPose', 'Overlap')
        for type in estimator_types:
            for area, detection in areas.items():
                with self.subTest(type=area):
                    if type == 'RGBM':
                        self.RGBMEstimateBadArea(detection, area)
                    elif type == 'AGS':
                        self.AGSEstimatorBadArea(image, detection, area)
                    elif type == 'HeadPose':
                        self.headPoseBadArea(image, detection, area)
                    elif type == 'Overlap':
                        self.OverlapEstimatorBadArea(image, detection, area)

    def RGBMEstimateBadArea(self, detection, area):
        estimator = self.faceEngine.createLivenessRGBMEstimator()
        background = f.Image()
        load_err = background.load("testData/rgbm_liveness_background_real.png")
        self.assertTrue(load_err.isOk)
        frame = f.Image()
        load_err = frame.load("testData/rgbm_liveness_frame_real.png")
        self.assertTrue(load_err.isOk)
        self.estimateResult(estimator, frame, detection, area, background)

    def AGSEstimatorBadArea(self, image, detection, area):
        config = f.createSettingsProvider("data/faceengine.conf")
        config.setValue("system", "betaMode", f.SettingsProviderValue(1))
        config.setValue("system", "verboseLogging", f.SettingsProviderValue(5))
        self.faceEngine.setSettingsProvider(config)
        estimator = self.faceEngine.createAGSEstimator()
        self.estimateResult(estimator, image, detection, area)
        config.setValue("system", "verboseLogging", f.SettingsProviderValue(0))
        self.faceEngine.setSettingsProvider(config)

    def OverlapEstimatorBadArea(self, image, detection, area):
        estimator = self.faceEngine.createOverlapEstimator()
        self.estimateResult(estimator, image, detection, area)

    def headPoseBadArea(self, image, detection, area):
        config = f.createSettingsProvider("data/faceengine.conf")
        config.setValue("HeadPoseEstimator::Settings", "useEstimationByImage", f.SettingsProviderValue(1))
        self.faceEngine.setSettingsProvider(config)
        estimator = self.faceEngine.createHeadPoseEstimator()
        self.estimateResult(estimator, image, detection, area)

    def estimateResult(self, estimator, image, detection, area, *background):
        err, estimation = estimator.estimate(image, detection, *background)
        self.assertEqual(err.error, f.FSDKError.InvalidRect) if area == "bad_area" else self.assertTrue(err.isOk)


if __name__ == '__main__':
    unittest.main()

