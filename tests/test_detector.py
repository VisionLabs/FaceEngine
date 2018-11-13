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
import FaceEngine as fe

testDataPath = "testData"

# erase two first arguments for unittest argument parsing
del(sys.argv[1])
del(sys.argv[1])

faceEngine = fe.createFaceEngine("data",
                                   "data/faceengine.conf")
expectedDetectionMTCNN = fe.Detection()
expectedRedetectionMTCNN = fe.Detection()
expectedDetectionMiniMTCNN = fe.Detection()
expectedDetectionS3FD = fe.Detection()

expectedDetectionMTCNN.rect.x = 288
expectedDetectionMTCNN.rect.y = 93
expectedDetectionMTCNN.rect.width = 148
expectedDetectionMTCNN.rect.height = 184
expectedDetectionMTCNN.score = 0.99999

expectedRedetectionMTCNN.rect.x = 289
expectedRedetectionMTCNN.rect.y = 74
expectedRedetectionMTCNN.rect.width = 153
expectedRedetectionMTCNN.rect.height = 198
expectedRedetectionMTCNN.score = 0.99999

expectedDetectionMiniMTCNN.rect.x = 297
expectedDetectionMiniMTCNN.rect.y = 97
expectedDetectionMiniMTCNN.rect.width = 152
expectedDetectionMiniMTCNN.rect.height = 184
expectedDetectionMiniMTCNN.score = 0.99999

expectedDetectionS3FD.rect.x = 297
expectedDetectionS3FD.rect.y = 81
expectedDetectionS3FD.rect.width = 141
expectedDetectionS3FD.rect.height = 208
expectedDetectionS3FD.score = 0.99954

# helper
def invoke_vector_coords(line):
    line = line.strip().split()
    x, y = float(line[0]), float(line[1])
    return fe.Vector2f(x, y)


class TestFaceEngineDetector(unittest.TestCase):
    def detect(self, detectorType, refDetection):
        configPath = "data" + "/faceengine.conf"
        config = faceEngine.createSettingsProvider(configPath)
        config.setValue("system", "verboseLogging", 5)
        config.setValue("system", "betaMode", 1)
        faceEngine.setSettingsProvider(config)
        detector = faceEngine.createDetector(detectorType)
        landmarks68 = fe.Landmarks68()
        lnetExpected = testDataPath + "/image1_lnet2_precise.txt"


        with open(lnetExpected) as lm68file:
            for i, line in enumerate(lm68file):
                landmarks68[i] = invoke_vector_coords(line)
        image = fe.Image()
        err = image.load(testDataPath + "/image1.ppm")

        # without landmarks
        count = 10
        res, detect_list = detector.detect(image, image.getRect(), count)
        count = len(detect_list)
        for i in range(count):
            print(detect_list[i].score,
                  detect_list[i].rect.x,
                  detect_list[i].rect.y,
                  detect_list[i].rect.width,
                  detect_list[i].rect.height)
        self.assertEqual(1, count)
        self.assertAlmostEqual(refDetection.score, detect_list[0].score, delta=0.0001)
        self.assertAlmostEqual(refDetection.rect.x, detect_list[0].rect.x, delta=2)
        self.assertAlmostEqual(refDetection.rect.y, detect_list[0].rect.y, delta=2)
        self.assertAlmostEqual(refDetection.rect.width, detect_list[0].width, delta=2)
        self.assertAlmostEqual(refDetection.rect.height, detect_list[0].height, delta=2)


    def test_BatchDetector(self):
        configPath = "data" + "/faceengine.conf"
        config = fe.createSettingsProvider(configPath)
        config.setValue("system", "verboseLogging", fe.SettingsProviderValue(5))
        config.setValue("system", "betaMode", fe.SettingsProviderValue(1))
        faceEngine.setSettingsProvider(config)
        detector = faceEngine.createDetector(fe.ODT_MTCNN)
        lnetExpected = fe.Landmarks68()
        ptsfilename = testDataPath + "/image1_lnet2_precise.txt"
        with open(ptsfilename) as file:
            for i, line in enumerate(file):
                lnetExpected[i] = invoke_vector_coords(line)
            count = 10
            imagesCount = 10
            images = []
            rectangles = []
            for i in range(count):
                image = fe.Image()
                err = image.load(testDataPath + "/" + "image1.ppm")
                images.append(image)
                self.assertTrue(image.isValid())
                rectangles.append(images[i].getRect())
            # without landmarks
            res, detect_list = detector.detect(images, rectangles, count, fe.dtBBox)
            self.assertEqual(len(detect_list), imagesCount)

            for i in range(imagesCount):
                faces = detect_list[i]
                self.assertEqual(1, len(faces))
                face = faces[0]
                self.assertAlmostEqual(expectedDetectionMTCNN.score,  face.m_detection.score, delta=0.0001)
                self.assertAlmostEqual(expectedDetectionMTCNN.rect.x, face.m_detection.rect.x, delta=2)
                self.assertAlmostEqual(expectedDetectionMTCNN.rect.y, face.m_detection.rect.y, delta=2)
                self.assertAlmostEqual(expectedDetectionMTCNN.rect.width, face.m_detection.rect.width, delta=2)
                self.assertAlmostEqual(expectedDetectionMTCNN.rect.height, face.m_detection.rect.height, delta=2)


            res, detect_list = detector.detect(images,
                                               rectangles,
                                               count,
                                               fe.DetectionType(fe.dtBBox|fe.dt68Landmarks))
            self.assertFalse(res.isError())
            self.assertEqual(len(detect_list), imagesCount)


    def test_EyeEstimator(self):
        eyeEstimator = faceEngine.createEyeEstimator()
        def testImage(landmarksCount, refLeftState, refRightState):

            reference = fe.EyesEstimation()
            landmarksCountStr = str(landmarksCount)
            lm68Path = "testData/eyes/image_" + landmarksCountStr + "_wlm.txt"
            imagePath = "testData/eyes/image_WARP.png"
            irisLeft = "testData/eyes/image_" + landmarksCountStr + "_0_iris.pts"
            irisRight = "testData/eyes/image_" + landmarksCountStr + "_1_iris.pts"
            eyelidLeft = "testData/eyes/image_" + landmarksCountStr + "_0_eyelid.pts"
            eyelidRight = "testData/eyes/image_" + landmarksCountStr + "_1_eyelid.pts"

            landmarks68_eyes = fe.Landmarks68()
            warp = fe.Image()
            warp.load(imagePath)

            def invoke_vector_coords(line):
                line = line.strip().split()
                x, y = float(line[0]), float(line[1])
                return fe.Vector2f(x, y)

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
            for i in range(len(fe.IrisLandmarks())):
                self.assertAlmostEqual(eyesEstimation.leftEye.iris[i].x, reference.leftEye.iris[i].x, delta=acceptableDiff)
                self.assertAlmostEqual(eyesEstimation.leftEye.iris[i].y, reference.leftEye.iris[i].y, delta=acceptableDiff)
                self.assertAlmostEqual(eyesEstimation.rightEye.iris[i].x, reference.rightEye.iris[i].x, delta=acceptableDiff)
                self.assertAlmostEqual(eyesEstimation.rightEye.iris[i].y, reference.rightEye.iris[i].y, delta=acceptableDiff)

            # eyelid
            for i in range(len(fe.EyelidLandmarks())):
                self.assertAlmostEqual(eyesEstimation.leftEye.eyelid[i].x, reference.leftEye.eyelid[i].x, delta=acceptableDiff)
                self.assertAlmostEqual(eyesEstimation.leftEye.eyelid[i].y, reference.leftEye.eyelid[i].y, delta=acceptableDiff)
                self.assertAlmostEqual(eyesEstimation.rightEye.eyelid[i].x, reference.rightEye.eyelid[i].x, delta=acceptableDiff)
                self.assertAlmostEqual(eyesEstimation.rightEye.eyelid[i].y, reference.rightEye.eyelid[i].y, delta=acceptableDiff)

        testImage(68, fe.State.Open, fe.State.Open)



if __name__ == '__main__':
    unittest.main()