import unittest
import argparse
import sys
import os

# if FaceEngine is NOT installed within the system, add the directory with FaceEngine*.so to system paths
parser = argparse.ArgumentParser()
parser.add_argument("-b", "--bind-path", type=str,
                    help="path to directory with FaceEngine*.so file - binding of luna-sdk")

args = parser.parse_args()
path_to_binding = args.bind_path
print(path_to_binding)
if not os.path.isdir(path_to_binding):
    print("Directory with FaceEngine*.so was not found.")
    exit(1)


print("Directory {0} with python bindings of FaceEngine was included".format(path_to_binding))
print(sys.argv)

sys.path.append(path_to_binding)

# if FaceEngine is installed within the system only this string of code is required for module importing
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

    def compare_detection_lists(self, _expDetection, detect_list, _imagesCount, _expLandmarks68=None):
        for j in range(_imagesCount):
            _faces = detect_list[j]
            self.assertEqual(1, len(_faces))
            _face = _faces[0]
            self.assertAlmostEqual(_expDetection.score, _face.detection.score, delta=0.0001)
            self.assertAlmostEqual(_expDetection.rect.x, _face.detection.rect.x, delta=2)
            self.assertAlmostEqual(_expDetection.rect.y, _face.detection.rect.y, delta=2)
            self.assertAlmostEqual(_expDetection.rect.width, _face.detection.rect.width, delta=2)
            self.assertAlmostEqual(_expDetection.rect.height, _face.detection.rect.height, delta=3)
            if _expLandmarks68:
                self.assertTrue(_face.landmarks68_opt.isValid())
                for k in range(len(_expLandmarks68)):
                    self.assertAlmostEqual(
                        _expLandmarks68[k].x,
                        _face.detection.rect.x + _face.landmarks68_opt.value()[j].x, 4)
                    self.assertAlmostEqual(
                        _expLandmarks68[k].y,
                        _face.detection.rect.y + _face.landmarks68_opt.value()[j].y, 4)

    def compare_detections(self, face1, face2):
        self.assertEqual(face1.detection.score, face2.detection.score)
        self.assertEqual(face1.detection.rect.x, face2.detection.rect.x)
        self.assertEqual(face1.detection.rect.y, face2.detection.rect.y)
        self.assertEqual(face1.detection.rect.width, face2.detection.rect.width)
        self.assertEqual(face1.detection.rect.height, face2.detection.rect.height)
        if face1.landmarks68_opt.isValid() and face2.landmarks68_opt.isValid():
            for k in range(len(face2.landmarks68_opt.value())):
                self.assertEqual(face1.landmarks68_opt.value()[k].x,
                                 face2.landmarks68_opt.value()[k].x)
                self.assertEqual(face1.landmarks68_opt.value()[k].y,
                                 face2.landmarks68_opt.value()[k].y)

        if face1.landmarks5_opt.isValid() and face2.landmarks5_opt.isValid():
            for k in range(len(face2.landmarks5_opt.value())):
                self.assertEqual(face1.landmarks5_opt.value()[k].x,
                                 face2.landmarks5_opt.value()[k].x)
                self.assertEqual(face1.landmarks5_opt.value()[k].y,
                                 face2.landmarks5_opt.value()[k].y)

    def detectorTest(self, _detectorType, _expectedDetection):
        configPath = os.path.join("data", "faceengine.conf")
        config = fe.createSettingsProvider(configPath)
        # config.setValue("system", "verboseLogging", fe.SettingsProviderValue(5))
        config.setValue("system", "betaMode", fe.SettingsProviderValue(1))
        faceEngine.setSettingsProvider(config)
        detector = faceEngine.createDetector(_detectorType)
        lnetExpected = fe.Landmarks68()
        ptsfilename = os.path.join(testDataPath, "image1_lnet2_precise.txt")
        image = fe.Image()
        err_image = image.load(os.path.join(testDataPath, "image1.ppm"))
        self.assertTrue(err_image.isOk)
        res_one, faceOne = detector.detectOne(
            image,
            image.getRect(),
        fe.DetectionType(fe.dt5Landmarks|fe.dt68Landmarks))
        self.assertTrue(res_one.isOk)

        with open(ptsfilename) as file:
            for i, line in enumerate(file):
                lnetExpected[i] = invoke_vector_coords(line)
            count = 10
            imagesCount = 10
            images = []
            rectangles = []
            for i in range(count):
                image = fe.Image()
                err = image.load(os.path.join(testDataPath, "image1.ppm"))
                images.append(image)
                self.assertTrue(image.isValid())
                rectangles.append(images[i].getRect())

            # without landmarks
            res, detect_list = detector.detect(images, rectangles, count, fe.dtBBox)
            self.assertEqual(len(detect_list), imagesCount)
            self.assertFalse(res.isError)
            self.compare_detection_lists(_expectedDetection, detect_list, imagesCount)
            self.compare_detections(faceOne, detect_list[0][0])

            # without lnet
            res, detect_list = detector.detect(
                images,
                rectangles,
                count,
                fe.DetectionType(fe.dtBBox|fe.dt5Landmarks))
            self.assertEqual(len(detect_list), imagesCount)
            self.assertFalse(res.isError)
            self.compare_detection_lists(
                _expectedDetection,
                detect_list,
                imagesCount)
            self.compare_detections(faceOne, detect_list[0][0])

            # with lnet
            res, detect_list = detector.detect(
                images,
                rectangles,
                count,
                fe.DetectionType(fe.dtBBox|fe.dt68Landmarks))
            self.compare_detection_lists(_expectedDetection, detect_list, imagesCount)
            self.compare_detections(faceOne, detect_list[0][0])

    def test_Detector(self):
        self.detectorTest(fe.ODT_DEFAULT, expectedDetectionMTCNN)
        self.detectorTest(fe.ODT_MTCNN, expectedDetectionMTCNN)
        self.detectorTest(fe.ODT_MTCNN_MINI, expectedDetectionMiniMTCNN)
        self.detectorTest(fe.ODT_S3FD, expectedDetectionS3FD)


if __name__ == '__main__':
    unittest.main()
