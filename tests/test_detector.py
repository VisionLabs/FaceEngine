import unittest
import argparse
import sys
import os
from license_helper import make_activation, ActivationLicenseError

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

expectedDetectionV1 = fe.DetectionFloat()
expectedDetectionV1 = fe.DetectionFloat()
expectedRedetectionV1 = fe.DetectionFloat()
expectedDetectionV2 = fe.DetectionFloat()
expectedDetectionV3 = fe.DetectionFloat()
expectedRedetectionV3 = fe.DetectionFloat()

expectedDetectionV1.rect.x = 288.0
expectedDetectionV1.rect.y = 93.0
expectedDetectionV1.rect.width = 148.0
expectedDetectionV1.rect.height = 184.0
expectedDetectionV1.score = 0.99999

expectedRedetectionV1.rect.x = 290.0
expectedRedetectionV1.rect.y = 75.0
expectedRedetectionV1.rect.width = 150.0
expectedRedetectionV1.rect.height = 197.0
expectedRedetectionV1.score = 0.99999

expectedDetectionV2.rect.x = 297.0
expectedDetectionV2.rect.y = 97.0
expectedDetectionV2.rect.width = 152.0
expectedDetectionV2.rect.height = 184.0
expectedDetectionV2.score = 0.99999

expectedDetectionV3.rect.x = 296.0
expectedDetectionV3.rect.y = 73.0
expectedDetectionV3.rect.width = 145.0
expectedDetectionV3.rect.height = 211.0
expectedDetectionV3.score = 0.99994

expectedRedetectionV3.rect.x = 293.0
expectedRedetectionV3.rect.y = 96.0
expectedRedetectionV3.rect.width = 150.0
expectedRedetectionV3.rect.height = 192.0
expectedRedetectionV3.score = 0.99954


# helper
def invoke_vector_coords(line):
    line = line.strip().split()
    x, y = float(line[0]), float(line[1])
    return fe.Vector2f(x, y)


class TestFaceEngineDetector(unittest.TestCase):

    faceEngine = None

    @classmethod
    def setUp(cls):
        cls.faceEngine = fe.createFaceEngine("data")
        if not make_activation(cls.faceEngine):
            raise ActivationLicenseError("License is not activated!")

    def compare_detection_lists(self, _expDetection, detect_list, _imagesCount, _expLandmarks68=None):
        for j in range(_imagesCount):
            _faces = detect_list[j]
            self.assertEqual(1, len(_faces))
            _face = _faces[0]
            self.assertAlmostEqual(_expDetection.score, _face.detection.score, delta=0.001)
            self.assertAlmostEqual(_expDetection.rect.x, _face.detection.rect.x, delta=3)
            self.assertAlmostEqual(_expDetection.rect.y, _face.detection.rect.y, delta=3)
            self.assertAlmostEqual(_expDetection.rect.width, _face.detection.rect.width, delta=3)
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

    def compare_faces(self, face1, face2):
        self.assertTrue(face1.isValid())
        self.assertTrue(face2.isValid())
        self.compare_detections(face1.detection, face2.detection)
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
        self.faceEngine.setSettingsProvider(config)
        detector = self.faceEngine.createDetector(_detectorType)
        lnetExpected = fe.Landmarks68()
        def get_image_prefix(_detectorType):
            if _detectorType == fe.FACE_DET_V1:
                return "fdet1"
            if _detectorType == fe.FACE_DET_V2:
                return "fdet2"
            if _detectorType == fe.FACE_DET_V3:
                return "fdet3"

        det = get_image_prefix(_detectorType)
        prec = "precise"
        ptsfilename = os.path.join(testDataPath, "lnet/image1_lnet2_" + det + "_" + prec + ".txt")
        image = fe.Image()
        err_image = image.load(os.path.join(testDataPath, "image1.ppm"))
        self.assertTrue(err_image.isOk)
        res_one, faceOne1 = detector.detectOne(
            image,
            image.getRect(),
            fe.DetectionType(fe.dt5Landmarks))
        self.assertTrue(res_one.isOk)
        res_one, faceOne2 = detector.detectOne(
            image,
            image.getRect(),
            fe.DetectionType(fe.dt5Landmarks|fe.dt68Landmarks))
        self.assertTrue(res_one.isOk)
        # dtAll test
        res_one, faceOne3 = detector.detectOne(
            image,
            image.getRect(),
            fe.DetectionType(fe.dtAll))
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
                self.assertTrue(err.isOk)
                images.append(image)
                self.assertTrue(image.isValid())
                rectangles.append(images[i].getRect())

            # without landmarks
            res, detect_list = detector.detect(images, rectangles, count, fe.dtBBox)
            self.assertEqual(len(detect_list), imagesCount)
            self.assertFalse(res.isError)
            self.compare_detection_lists(_expectedDetection, detect_list, imagesCount)
            self.compare_faces(faceOne1, detect_list[0][0])

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
            self.compare_faces(faceOne1, detect_list[0][0])

            # with lnet
            res, detect_list = detector.detect(
                images,
                rectangles,
                count,
                fe.DetectionType(fe.dtBBox|fe.dt68Landmarks))
            self.compare_detection_lists(_expectedDetection, detect_list, imagesCount)
            self.compare_faces(faceOne2, detect_list[0][0])
            self.compare_faces(faceOne3, detect_list[0][0])

    def humanDetectorTest(self):
        configPath = os.path.join("data", "faceengine.conf")
        humanDetector = self.faceEngine.createHumanDetector()
        image = fe.Image()
        err_image = image.load(os.path.join(testDataPath, "0_Parade_marchingband_1_620.ppm"))
        self.assertTrue(err_image.isOk)
        err_human_detector, list_of_list_of_detections = humanDetector.detect([image], [image.getRect()], 10)
        self.assertTrue(err_human_detector.isOk)
        self.assertEqual(74, list_of_list_of_detections[0][0].detection.rect.x)
        self.assertEqual(235, list_of_list_of_detections[0][0].detection.rect.y)
        self.assertEqual(153, list_of_list_of_detections[0][0].detection.rect.width)
        self.assertEqual(298, list_of_list_of_detections[0][0].detection.rect.height)

    def test_Detector(self):
        self.detectorTest(fe.FACE_DET_V1, expectedDetectionV1)
        self.detectorTest(fe.FACE_DET_V2, expectedDetectionV2)
        self.detectorTest(fe.FACE_DET_V3, expectedDetectionV3)
        self.humanDetectorTest()

    def redetectTest(self, _detectorType, refDetection):
        configPath = os.path.join("data", "faceengine.conf")
        config = fe.createSettingsProvider(configPath)
        if _detectorType == fe.FACE_DET_V3:
            config.setValue("FaceDetV3::Settings", "RedetectExpandCoef", fe.SettingsProviderValue(0.7))
        self.faceEngine.setSettingsProvider(config)
        detector = self.faceEngine.createDetector(_detectorType)
        image = fe.Image()
        err_image = image.load(os.path.join(testDataPath, "image1.ppm"))
        self.assertTrue(err_image.isOk)
        err, face = detector.detectOne(image, image.getRect(), fe.DetectionType(fe.dtBBox|fe.dt5Landmarks))
        self.assertFalse(err.isError)
        self.assertTrue(face.isValid())
        # redetection
        iteraionsNumber = 1
        for i in range(iteraionsNumber):
            err_redetect1, face_redection1 = detector.redetectOne(face, fe.DetectionType(fe.dtBBox|fe.dt5Landmarks))
            self.assertTrue(err_redetect1.isOk)
            self.assertTrue(face_redection1.isValid())
            self.assertAlmostEqual(refDetection.rect.x, face_redection1.detection.rect.x, delta=3)
            self.assertAlmostEqual(refDetection.rect.y, face_redection1.detection.rect.y, delta=3)
            self.assertAlmostEqual(refDetection.rect.width, face_redection1.detection.rect.width, delta=3)
            self.assertAlmostEqual(refDetection.rect.height, face_redection1.detection.rect.height, delta=3)
            err_redetect2, face_redection2 = detector.redetectOne(image, face.detection, fe.DetectionType(fe.dtBBox|fe.dt5Landmarks))
            self.assertTrue(err_redetect2.isOk)
            self.assertTrue(face_redection2.isValid())
            self.assertAlmostEqual(refDetection.rect.x, face_redection2.detection.rect.x, delta=3)
            self.assertAlmostEqual(refDetection.rect.y, face_redection2.detection.rect.y, delta=3)
            self.assertAlmostEqual(refDetection.rect.width, face_redection2.detection.rect.width, delta=3)
            self.assertAlmostEqual(refDetection.rect.height, face_redection2.detection.rect.height, delta=3)

    def test_RedetectOne(self):
        self.redetectTest(fe.FACE_DET_V1, expectedRedetectionV1)
        self.redetectTest(fe.FACE_DET_V3, expectedRedetectionV3)

    def test_OptionalLandmarks(self):
        landmarks5 = fe.Landmarks5()
        for i, _ in enumerate(landmarks5):
            landmarks5[i] = fe.Vector2f(float(i), float(i))
        optional_landmarks5_1 = fe.OptionalLandmarks5(landmarks5)
        self.assertTrue(optional_landmarks5_1.isValid())
        optional_landmarks5_2 = fe.OptionalLandmarks5()
        # invalid befor initialization
        self.assertFalse(optional_landmarks5_2.isValid())
        optional_landmarks5_2.set(landmarks5)
        self.assertTrue(optional_landmarks5_2.isValid())
        landmarks5_from_optional1 = optional_landmarks5_1.value()
        landmarks5_from_optional2 = optional_landmarks5_2.value()
        for i, _ in enumerate(landmarks5_from_optional1):
            self.assertTrue(landmarks5_from_optional1[i] != 0.0)
            self.assertEqual(landmarks5[i], landmarks5_from_optional1[i])
            self.assertEqual(landmarks5[i], landmarks5_from_optional2[i])

        landmarks68 = fe.Landmarks68()
        for i, _ in enumerate(landmarks68):
            landmarks68[i] = fe.Vector2f(float(i), float(i))
        optional_landmarks68_1 = fe.OptionalLandmarks68(landmarks68)
        self.assertTrue(optional_landmarks68_1.isValid())
        optional_landmarks68_2 = fe.OptionalLandmarks68()
        # invalid befor initialization
        self.assertFalse(optional_landmarks68_2.isValid())
        optional_landmarks68_2.set(landmarks68)
        self.assertTrue(optional_landmarks68_2.isValid())
        landmarks68_from_optional1 = optional_landmarks68_1.value()
        landmarks68_from_optional2 = optional_landmarks68_2.value()
        for i, _ in enumerate(landmarks68_from_optional1):
            self.assertTrue(landmarks68_from_optional1[i] != 0.0)
            self.assertEqual(landmarks68[i], landmarks68_from_optional1[i])
            self.assertEqual(landmarks68[i], landmarks68_from_optional2[i])
    def compare_detections(self, detection1, detection2):
        self.assertTrue(detection1.isValid())
        self.assertTrue(detection2.isValid())
        self.assertEqual(detection1.rect.x, detection2.rect.x)
        self.assertEqual(detection1.rect.y, detection2.rect.y)
        self.assertEqual(detection1.rect.width, detection2.rect.width)
        self.assertEqual(detection1.rect.height, detection2.rect.height)
        self.assertEqual(detection1.score, detection2.score)

    def test_BaseDetection(self):
        detection_int = fe.Detection(fe.Rect(10, 11, 12, 13), 0.97)
        detection_float = fe.DetectionFloat(fe.RectFloat(10.0, 11.0, 12.0, 13.0), 0.97)
        self.compare_detections(detection_int, detection_float)
        detection_float = fe.DetectionFloat(fe.Rect(10, 11, 12, 13), 0.97)
        self.compare_detections(detection_int, detection_float)
        detection_int = fe.Detection()
        self.assertFalse(detection_int.isValid())
        detection_int.set(detection_float)
        self.compare_detections(detection_int, detection_float)
        detection_int2 = detection_int.asInt()
        detection_float2 = detection_int.asFloat()
        self.compare_detections(detection_int2, detection_float2)
        detection_int3 = detection_float.asInt()
        detection_float3 = detection_float.asFloat()
        self.compare_detections(detection_int3, detection_float3)

    def test_Face(self):
        face = fe.Face()
        self.assertFalse(face.isValid())
        image = fe.Image()
        err_image = image.load(os.path.join(testDataPath, "image1.ppm"))
        self.assertTrue(err_image.isOk)
        self.assertTrue(image.isValid())
        face = fe.Face(image)
        self.assertFalse(face.isValid())
        face_initiated_by_int = fe.Face(image, fe.Detection(image.getRect(), 0.97))
        face_initiated_by_float = fe.Face(image, fe.DetectionFloat(image.getRect(), 0.97))
        self.compare_faces(face_initiated_by_int, face_initiated_by_float)
        self.assertTrue(face_initiated_by_float.isValid())


if __name__ == '__main__':
    unittest.main()
