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
print("sys.path = ", sys.path)

# if FaceEngine is installed only this string of code is required for module importing
import FaceEngine as f

# erase two first arguments for unittest argument parsing
del(sys.argv[1])
del(sys.argv[1])

faceEnginePtr = f.createPyFaceEnginePtr("../data",
                                        "../data/faceengine.conf")


# detector test and example
def detect(image_det, max_detections):
    detector = faceEnginePtr.createDetector(f.ODT_MTCNN)
    detector_result = detector.detect(image_det, image_det.getRect(), max_detections)
    # for i, item in enumerate(detector_result, 1):
#     print(i, item)
    return detector_result

# warper test and example
max_detections = 3
image_det = f.Image()
err = image_det.load("../testData/00205_9501_p.ppm")
print(image_det.getHeight(), image_det.getWidth(), image_det.isValid())
print("Image error = ", err)

(detection, landmarks5, landmarks68) = detect(image_det, 3)[0]

warper = faceEnginePtr.createWarper()
transformation = warper.createTransformation(detection, landmarks5)
print(transformation)
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
    print("irisLandmarks = ", len(irisLandmarks))
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
        image.load("../testData/00205_9501_p.ppm")
        self.assertTrue(image.isValid())
        attribute_result = attributeEstimator.estimate(image)
        self.assertEqual(attribute_result.gender, 0.0)
        self.assertAlmostEqual(attribute_result.glasses, 0.011, delta=0.01)
        self.assertEqual(attribute_result.age, 55.0)

    def test_QualityEstimator(self):
        qualityEstimator = faceEnginePtr.createQualityEstimator()
        image = f.Image()
        image.load("../testData/photo_2017-03-30_14-47-43_p.ppm")
        self.assertTrue(image.isValid())
        quality_result = qualityEstimator.estimate(image)
        self.assertAlmostEqual(quality_result.getQuality(), 0.95, delta=0.15)

    def test_EthnicityEstimator(self):
        logging.info("EthnicityEstimator")
        ethnicityEstimator = faceEnginePtr.createEthnicityEstimator()
        image = f.Image()
        image.load("../testData/warp1.ppm")
        self.assertTrue(image.isValid())
        ethnicity_result = ethnicityEstimator.estimate(image)
        self.assertAlmostEqual(ethnicity_result.africanAmerican, 0.0, delta=0.1)
        self.assertAlmostEqual(ethnicity_result.indian, 0.0, delta=0.1)
        self.assertAlmostEqual(ethnicity_result.asian, 0.0, delta=0.1)
        self.assertAlmostEqual(ethnicity_result.caucasian, 1.0, delta=0.1)

    def test_HeadPoseEstimatorLandmarks(self):
        config = f.createSettingsProviderPtr("../data/faceengine.conf")
        config.setValue("HeadPoseEstimator::Settings","useEstimationByImage", f.SettingsProviderValue(0))
        config.setValue("HeadPoseEstimator::Settings","useEstimationByLandmarks", f.SettingsProviderValue(1))
        faceEnginePtr.setSettingsProvider(config)
        image = f.Image()
        image.load("../testData/photo_2017-03-30_14-47-43_p.ppm")
        det = faceEnginePtr.createDetector(f.ODT_MTCNN)
        det.detect(image, image.getRect(), 1)
        (detection, landmarks5, landmarks68) = detect(image, 3)[0]
        headPoseEstimator = faceEnginePtr.createHeadPoseEstimator()
        headPoseEstimation = headPoseEstimator.estimate(landmarks68)
        print(headPoseEstimation)
        expected = f.HeadPoseEstimation()
        expected.roll = 0.3891
        expected.yaw = -2.3956
        expected.pitch = 10.5922
        self.assertAlmostEqual(headPoseEstimation.roll, expected.roll, delta=3.0)
        self.assertAlmostEqual(headPoseEstimation.yaw, expected.yaw, delta=3.0)
        self.assertAlmostEqual(headPoseEstimation.pitch, expected.pitch, delta=3.0)
        print("Actual values headPoseEstimation by landmarks: {0}".format(headPoseEstimation))
        self.assertEqual(f.FrontalFaceType.FrontalFace1, expected.getFrontalFaceType())

    def test_HeadPoseEstimatorImage(self):
        config = f.createSettingsProviderPtr("../data/faceengine.conf")
        config.setValue("HeadPoseEstimator::Settings","useEstimationByImage", f.SettingsProviderValue(1))
        config.setValue("HeadPoseEstimator::Settings","useEstimationByLandmarks", f.SettingsProviderValue(0))
        faceEnginePtr.setSettingsProvider(config)
        image = f.Image()
        image.load("../testData/photo_2017-03-30_14-47-43_p.ppm")
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
        print("Actual values headPoseEstimation by image: {0}".format(headPoseEstimation))
        self.assertEqual(f.FrontalFaceType.FrontalFace1, expected.getFrontalFaceType())

    def test_BlackWhiteEstimator(self):
        blackWhiteEstimator = faceEnginePtr.createBlackWhiteEstimator()
        image = f.Image()
        image.load("../testData/warp1.ppm")
        self.assertTrue(image.isValid())
        isBlack = blackWhiteEstimator.estimate(image)
        print("blackWhiteEstimator = {0}".format(isBlack))
        self.assertFalse(isBlack)

    def test_DepthEstimator(self):
        depthEstimator = faceEnginePtr.createDepthEstimator()
        # depth
        # loadImage - only for depth image downloading
        depthImage = f.loadImage("../testData/warp.depth")
        depth_result = depthEstimator.estimate(depthImage)
        print("Depth estimation result = {0}".format(depth_result))
        self.assertAlmostEqual(depth_result.value, 1.0, delta=0.01)

    def test_IREstimator(self):
        iREstimator = faceEnginePtr.createIREstimator()
        irImage = f.Image()
        irImage.load("../testData/irWarp.ppm")
        self.assertTrue(irImage.isValid())
        irRestult = iREstimator.estimate(irImage)
        print("irResult = ", irRestult)
        self.assertFalse(irRestult.isError)
        self.assertAlmostEqual(irRestult.value, 1.0, delta=0.01)

    def test_SmileEstimator(self):
        warper = faceEnginePtr.createWarper()
        smileEstimator = faceEnginePtr.createSmileEstimator()
        smileImage = f.Image()
        overlapImage = f.Image()
        mouthImage = f.Image()
        overlapImage.load("../testData/overlap.ppm")
        self.assertTrue(overlapImage.isValid())
        smileImage.load("../testData/smile.ppm")
        self.assertTrue(smileImage.isValid())
        mouthImage.load("../testData/mouth.ppm")
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
        faceFlowImage.load("../testData/small.ppm")
        sequence = []
        for i in range(10):
            tempImage = f.Image()
            tempImage.load("../testData/" + str(i) + "big.ppm")
            sequence.append(tempImage)
        faceFlowScore = faceFlowEstimator.estimate(faceFlowImage, sequence, len(sequence))
        self.assertAlmostEqual(faceFlowScore, 0.9967, delta=0.01)

    def test_EyeEstimator(self):
        eyeEstimator = faceEnginePtr.createEyeEstimator()
        eyesEstimation = eyeEstimator.estimate(warpedImage, transformedLandmarks5)
        print(eyesEstimation.leftEye.state, eyesEstimation.leftEye.iris, eyesEstimation.leftEye.eyelid)
        print(eyesEstimation.rightEye.state, eyesEstimation.rightEye.iris, eyesEstimation.rightEye.eyelid)

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
            print("Emotions {0}".format(emotionsEstimation))
            print("Best: {0}".format(emotionsEstimation.getPredominantEmotion()))
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
        emotions_test("../testData/emotions1.ppm", reference1, f.Emotions.Happiness)
        emotions_test("../testData/emotions2.ppm", reference2, f.Emotions.Anger)

    def test_GazeEstimator(self):

        with open("../testData/gaze.bin", "rb") as file:
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
            print("GazeEstimation actual = {0}".format(actual))

if __name__ == '__main__':
    unittest.main()



# // Create FaceEngine main object
# IFaceEnginePtr faceEngine = acquire(createFaceEngine(dataPath.c_str()));
# IGazeEstimatorPtr gazeEstimator = acquire(faceEngine->createGazeEstimator());
#
# auto deserializeData = [](HeadPoseEstimation& pose, EyesEstimation& eyes){
#     Landmarks68 landmarks;
# std::ifstream file (testDataPath+"/gaze.bin" , std::ios::in|std::ios::binary);
# file.read((char*)&pose.yaw, sizeof(float));
# file.read((char*)&pose.pitch, sizeof(float));
# file.read((char*)&pose.roll, sizeof(float));
#
# file.read((char*)eyes.leftEye.iris.landmarks, sizeof(EyesEstimation::EyeAttributes::IrisLandmarks));
# file.read((char*)eyes.rightEye.iris.landmarks, sizeof(EyesEstimation::EyeAttributes::IrisLandmarks));
#
# file.read((char*)&landmarks, sizeof(Landmarks68));
# file.close();
#
# // write lm68 into eyesestimation
# for(int idx = 0; idx < 6; ++idx) {
# eyes.leftEye.eyelid.landmarks[idx] = landmarks.landmarks[36 + idx];
# eyes.rightEye.eyelid.landmarks[idx] = landmarks.landmarks[42 + idx];
# }
# };
#
# HeadPoseEstimation headPoseEstimation;
# EyesEstimation eyesEstimation;
#
# deserializeData(headPoseEstimation, eyesEstimation);
#
# GazeEstimation actual;
# GazeEstimation expected;
# expected.leftEye.yaw = -9.0405864578f;
# expected.leftEye.pitch = -2.1464545992f;
# expected.rightEye.yaw = -4.9038884727f;
# expected.rightEye.pitch = -0.13287750706f;
#
# Result<FSDKError> gazeErr = gazeEstimator->estimate(headPoseEstimation, eyesEstimation, actual);
#
# ASSERT_FALSE(gazeErr.isError());
#
# std::cout << "Actual values: l.yaw " <<actual.leftEye.yaw << "| l.pitch " << actual.leftEye.pitch << "| r.yaw "  << actual.rightEye.yaw << "| r.pitch "  << actual.rightEye.pitch << std::endl;
#
# ASSERT_NEAR(expected.rightEye.pitch, actual.rightEye.pitch, 0.01);
# ASSERT_NEAR(expected.rightEye.yaw, actual.rightEye.yaw, 0.01);
# ASSERT_NEAR(expected.leftEye.pitch, actual.leftEye.pitch, 0.01);
# ASSERT_NEAR(expected.leftEye.yaw, actual.leftEye.yaw, 0.01);
# }