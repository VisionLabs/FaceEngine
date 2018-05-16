import sys
import unittest
import argparse
import sys
import os
import glob
import logging

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

class TestFaceEngineRect(unittest.TestCase):

    def test_AttributeEstimator(self):
        attributeEstimator = faceEnginePtr.createAttributeEstimator()
        image = f.Image()
        image.load("../testData/00205_9501_p.ppm")
        self.assertTrue(image.isValid())
        attribute_result = attributeEstimator.estimate(image)

        self.assertEqual(attribute_result.gender, 0.0)
        self.assertAlmostEqual(attribute_result.glasses, 0.011, places=3)
        self.assertEqual(attribute_result.age, 55.0)

    def test_QualityEstimator(self):
        qualityEstimator = faceEnginePtr.createQualityEstimator()
        image = f.Image()
        image.load("../testData/photo_2017-03-30_14-47-43_p.ppm")
        self.assertTrue(image.isValid())
        quality_result = qualityEstimator.estimate(image)

        self.assertAlmostEqual(quality_result.getQuality(), 0.95, places=1)
        # self.assertAlmostEqual(quality_result.dark, 0.975, places=2)
        # self.assertAlmostEqual(quality_result.gray, 0.979, places=2)
        # self.assertAlmostEqual(quality_result.blur, 0.955, places=2)

    def test_EthnicityEstimator(self):
        logging.info("EthnicityEstimator")
        ethnicityEstimator = faceEnginePtr.createEthnicityEstimator()
        image = f.Image()
        image.load("../testData/warp1.ppm")
        self.assertTrue(image.isValid())
        ethnicity_result = ethnicityEstimator.estimate(image)
        self.assertAlmostEqual(ethnicity_result.africanAmerican, 0.0, places=2)
        self.assertAlmostEqual(ethnicity_result.indian, 0.0, places=2)
        self.assertAlmostEqual(ethnicity_result.asian, 0.0, places=2)
        self.assertAlmostEqual(ethnicity_result.caucasian, 1.0, places=2)

    def test_HeadPoseEstimator(self):

        config = f.createSettingsProviderPtr("../data/faceengine.conf")
        config.setValue("HeadPoseEstimator::Settings","useEstimationByImage", f.SettingsProviderValue(0))
        config.setValue("HeadPoseEstimator::Settings","useEstimationByLandmarks", f.SettingsProviderValue(1))
        faceEnginePtr.setSettingsProvider(config)
        val1 = config.getValue("HeadPoseEstimator::Settings","useEstimationByImage")
        val2 = config.getValue("HeadPoseEstimator::Settings","useEstimationByLandmarks")
        print("val1 = ", val1.asInt())
        print("val2 = ", val2.asInt())

        image = f.Image()
        image.load("../testData/photo_2017-03-30_14-47-43_p.ppm")
        det = faceEnginePtr.createDetector(f.ODT_MTCNN)
        det.detect(image, image.getRect(), 1)
        (detection, landmarks5, landmarks68) = detect(image, 3)[0]
        print("headPoseDebug", landmarks68)
        headPoseEstimator = faceEnginePtr.createHeadPoseEstimator()
        headPoseEstimation = headPoseEstimator.estimate(landmarks68)
        # for i in range(len(landmarks68)):
        #     print(landmarks68[i])
        print(headPoseEstimation)

    def test_BlackWhiteEstimator(self):
        blackWhiteEstimator = faceEnginePtr.createBlackWhiteEstimator()
        image = f.Image()
        image.load("../testData/warp1.ppm")
        self.assertTrue(image.isValid())
        print(blackWhiteEstimator.estimate(image))

    def test_DepthEstimator(self):
        depthEstimator = faceEnginePtr.createDepthEstimator()
        # depth
        # loadImage - only for depth image downloading
        depthImage = f.loadImage("../testData/warp.depth")
        depth_result = depthEstimator.estimate(depthImage)
        print("Depth estimation result = {0}".format(depth_result))
        self.assertAlmostEqual(depth_result.value, 1.0, places=2)

    def test_IREstimator(self):
        iREstimator = faceEnginePtr.createIREstimator()
        irImage = f.Image()
        irImage.load("../testData/irWarp.ppm")
        self.assertTrue(irImage.isValid())
        print("iRresult = ", iREstimator.estimate(irImage))

    def test_SmileEstimator(self):
        detector = faceEnginePtr.createDetector(f.ODT_MTCNN)
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

        self.assertAlmostEqual(overlap_result.mouth, 0.0, places=3)
        self.assertAlmostEqual(overlap_result.smile, 0.0, places=3)
        self.assertAlmostEqual(overlap_result.occlusion, 1.0, places=3)

        (detection_smile, landmarks5_smile, landmarks68_smile) = detect(smileImage, 1)[0]
        transformation_smile = warper.createTransformation(detection_smile, landmarks5_smile)
        warped_smile_image = warper.warp(smileImage, transformation_smile)
        smile_result = smileEstimator.estimate(warped_smile_image)

        self.assertAlmostEqual(smile_result.mouth, 0.0, places=3)
        self.assertAlmostEqual(smile_result.smile, 1.0, places=3)
        self.assertAlmostEqual(smile_result.occlusion, 0.0, places=3)

        (detection_mouth, landmarks5_mouth, landmarks68_mouth) = detect(mouthImage, 1)[0]
        transformation_mouth = warper.createTransformation(detection_mouth, landmarks5_mouth)
        warped_mouth_image = warper.warp(mouthImage, transformation_mouth)
        mouth_result = smileEstimator.estimate(warped_mouth_image)

        self.assertAlmostEqual(mouth_result.mouth, 1.0, places=2)
        self.assertAlmostEqual(mouth_result.smile, 0.0, places=3)
        self.assertAlmostEqual(mouth_result.occlusion, 0.0, places=2)


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
        self.assertAlmostEqual(faceFlowScore, 0.9967, places=4)

    def test_EyeEstimator(self):
        eyeEstimator = faceEnginePtr.createEyeEstimator()
        eyesEstimation = eyeEstimator.estimate(warpedImage, transformedLandmarks5)
        print(eyesEstimation.leftEye.state, eyesEstimation.leftEye.iris, eyesEstimation.leftEye.eyelid)
        print(eyesEstimation.rightEye.state, eyesEstimation.rightEye.iris, eyesEstimation.rightEye.eyelid)

    def test_EmotionsEstimator(self):
        emotionsEstimator = faceEnginePtr.createEmotionsEstimator()
        image_happiness = f.Image()
        image_anger = f.Image()
        image_happiness.load("../testData/emotions1.ppm")
        image_anger.load("../testData/emotions2.ppm")
        self.assertTrue(image_happiness.isValid())
        self.assertTrue(image_anger.isValid())
        print(emotionsEstimator.estimate(image_happiness))
        print(emotionsEstimator.estimate(image_anger))

    def test_GazeEstimator(self):
        gazeEstimator = faceEnginePtr.createGazeEstimator()
        eyeEstimator = faceEnginePtr.createEyeEstimator()
        headPoseEstimator = faceEnginePtr.createHeadPoseEstimator()
        headPoseEstimation = headPoseEstimator.estimate(landmarks68)
        eyesEstimation = eyeEstimator.estimate(warpedImage, transformedLandmarks5)
        gaze_result = gazeEstimator.estimate(headPoseEstimation, eyesEstimation)
        print("Gaze result", gaze_result)


if __name__ == '__main__':
    unittest.main()

