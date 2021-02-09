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

if len(sys.argv) == 1 or not args.bind_path or not os.path.isdir(args.bind_path):
    parser.print_help(sys.stderr)
    sys.exit(1)

path_to_binding = args.bind_path
print("Directory {0} with python bindings of FaceEngine was included".format(path_to_binding))
print(sys.argv)

sys.path.append(path_to_binding)

# if FaceEngine is installed within the system only this string of code is required for module importing
import FaceEngine as fe

testDataPath = "testData"

# erase two first arguments for unittest argument parsing
del (sys.argv[1])
del (sys.argv[1])


# helper
def invokeVectorCoords(line):
    line = line.strip().split()
    x, y = float(line[0]), float(line[1])
    return fe.Vector2f(x, y)


class TestFaceEngineDetector(unittest.TestCase):
    faceEngine = None

    @classmethod
    def setUpClass(cls):
        cls.faceEngine = fe.createFaceEngine("data")
        if not make_activation(cls.faceEngine):
            raise ActivationLicenseError("License is not activated!")
        cls.image = fe.Image()
        err = cls.image.load(os.path.join(testDataPath, "image1.ppm"))
        cls.assertTrue(cls, err.isOk)
        cls.assertTrue(cls, cls.image.isValid())
        cls.image_for_redetect = cls.image
        cls.detection_for_redetect = fe.Detection(fe.Rect(289, 94, 147, 185), 0.999999)
        cls.detections_batch_for_redetect = [[cls.detection_for_redetect]] * 10
        cls.images_batch_for_detect = [cls.image] * 10
        cls.rectangles = [cls.image.getRect()] * 10

    def validationTest(self, ins, isOk, i_failed_list, *args):
        val_err, val_result = ins.validate(*args)
        self.assertEqual(val_err.isOk, isOk)
        self.assertTrue(type(args[0]) is list)
        self.assertTrue(len(args[0]) > 0)
        input_list = args[0]
        for i in range(len(input_list)):
            if i in i_failed_list:
                self.assertTrue(val_result[i].isError)
            else:
                self.assertTrue(val_result[i].isOk)

    def setUp(self):
        self.configPath = os.path.join("data", "faceengine.conf")
        self.config = fe.createSettingsProvider(self.configPath)
        self.config.setValue("FaceDetV3::Settings", "minFaceSize", 20)
        self.configV3 = fe.createSettingsProvider(self.configPath)
        self.configV3.setValue("system", "defaultDetectorType", "FaceDetV3")

    def assertFacesWithEtalons(self, face_expected, face):
        self.assertDetections(face_expected.detection, face.detection, delta=3, scoreDelta=0.001)
        if face.landmarks68_opt.isValid():
            absoluteLandmarks = self.addDetectionCoordinatesToLandmarks(face, face.landmarks68_opt)
            self.assertFaceLandmarks(face_expected.landmarks68_opt.value(), absoluteLandmarks.value(), delta=4)

    def assertFaceValid(self, face, landmarks5Valid=True, landmarks68Valid=True):
        self.assertTrue(face.isValid())
        self.assertTrue(face.landmarks5_opt.isValid() == landmarks5Valid)
        self.assertTrue(face.landmarks68_opt.isValid() == landmarks68Valid)

    def assertErrors(self, errorsList, valid):
        for error in errorsList:
            self.assertEqual(error.isOk, valid)

    def assertFaceLandmarks(self, face1_landmarks, face2_landmarks, delta=0.01):
        self.assertEqual(len(face1_landmarks), len(face2_landmarks))
        for k in range(len(face2_landmarks)):
            self.assertAlmostEqual(face1_landmarks[k].x, face2_landmarks[k].x, delta=delta)
            self.assertAlmostEqual(face1_landmarks[k].y, face2_landmarks[k].y, delta=delta)

    def assertDetections(self, detection1, detection2, delta=0.01, scoreDelta=0.01):
        rect1 = detection1.getRect()
        rect2 = detection2.getRect()
        self.assertAlmostEqual(rect1.x, rect2.x, delta=delta)
        self.assertAlmostEqual(rect1.y, rect2.y, delta=delta)
        self.assertAlmostEqual(rect1.width, rect2.width, delta=delta)
        self.assertAlmostEqual(rect1.height, rect2.height, delta=delta)
        self.assertAlmostEqual(detection1.getScore(), detection2.getScore(), delta=scoreDelta)

    def addDetectionCoordinatesToLandmarks(self, face, landmarks):
        rect = face.detection.getRect()
        absoluteLandmarks = fe.Landmarks68()
        for k in range(len(landmarks.value())):
            absoluteLandmarks[k] = fe.Vector2f(rect.x + landmarks.value()[k].x,
                                               rect.y + landmarks.value()[k].y)
        return fe.OptionalLandmarks68(absoluteLandmarks)

    def testDetectorBatchDifferentImages(self):
        images = []
        rectangles = []
        image_list = ["image1.ppm", "image_720.jpg", "mouth.ppm"]
        for value in image_list:
            image = fe.Image()
            err = image.load(os.path.join(testDataPath, value))
            self.assertTrue(err.isOk)
            images.append(image)
            rectangles.append(image.getRect())

        detectorTypes = {fe.FACE_DET_V1, fe.FACE_DET_V2, fe.FACE_DET_V3}
        for detectorType in detectorTypes:
            with self.subTest(detectorType=detectorType):
                self.faceEngine.setSettingsProvider(self.config)
                detector = self.faceEngine.createDetector(detectorType)
                refFaces = []
                for img in images:
                    res, face = detector.detectOne(img, img.getRect(),
                                                   fe.DetectionType(fe.DT_BBOX | fe.DT_LANDMARKS5 | fe.DT_LANDMARKS68))
                    self.assertTrue(res.isOk)
                    self.assertFaceValid(face)
                    refFaces.append(face)
                res3, facesBatch = detector.detect(images, rectangles, 3,
                                                  fe.DetectionType(fe.DT_BBOX | fe.DT_LANDMARKS68))
                self.assertTrue(res3.isOk)
                for i in range(0, facesBatch.getSize()):
                    detections = facesBatch.getDetections(i)
                    landmarks68 = facesBatch.getLandmarks68(i)
                    for j in range(0, len(detections)):
                        self.assertTrue(detections[j].isValid())
                        self.assertDetections(refFaces[i].detection, detections[j])
                        self.assertFaceLandmarks(refFaces[i].landmarks68_opt.value(), landmarks68[j])
                        self.validationTest(detector, True, [], [images[0], images[0]], [rectangles[0], rectangles[0]], 2)
                self.validationTest(detector, False, [1], [images[0], fe.Image()], [rectangles[0], rectangles[0]], 2)
                self.validationTest(detector, False, [0], [images[0], images[0]], [fe.Rect(), rectangles[0]], 2)
                self.validationTest(detector, False, [], [images[0], images[0]], [rectangles[0], rectangles[0]], 0)

    def testDetectorBBoxLandmarks68(self):
        ref_values = {fe.FACE_DET_V1: fe.Detection(fe.RectFloat(288.0, 93.0, 148.0, 184.0), 0.9999),
                      fe.FACE_DET_V2: fe.Detection(fe.RectFloat(297.0, 97.0, 152.0, 184.0), 0.9999),
                      fe.FACE_DET_V3: fe.Detection(fe.RectFloat(297.0, 90.0, 144.0, 193.0), 0.9999)}
        lnetExpected = fe.Landmarks68()

        for detectorType, detection in ref_values.items():
            with self.subTest(detectorType=detectorType):
                self.faceEngine.setSettingsProvider(self.config)
                detector = self.faceEngine.createDetector(detectorType)
                res_one, face = detector.detectOne(self.image, self.image.getRect(),
                                                   fe.DetectionType(fe.DT_BBOX | fe.DT_LANDMARKS5 | fe.DT_LANDMARKS68))
                self.assertTrue(res_one.isOk)
                self.assertFaceValid(face)

                def getImagePrefix(detectorType):
                    if detectorType == fe.FACE_DET_V1:
                        return "fdet1"
                    if detectorType == fe.FACE_DET_V2:
                        return "fdet2"
                    if detectorType == fe.FACE_DET_V3:
                        return "fdet3"

                det = getImagePrefix(detectorType)
                prec = "precise"
                ptsfilename = os.path.join(testDataPath, "lnet/image1_lnet2_" + det + "_" + prec + ".txt")
                with open(ptsfilename) as file:
                    for i, line in enumerate(file):
                        lnetExpected[i] = invokeVectorCoords(line)
                face_expected = fe.Face(self.image)
                face_expected.detection = detection
                face_expected.landmarks68_opt.set(lnetExpected)
                self.assertFacesWithEtalons(face_expected, face)

    def testDetectorBatchValidDifferentDetectionType(self):
        detectorTypes = {fe.FACE_DET_V1, fe.FACE_DET_V2, fe.FACE_DET_V3}
        for detectorType in detectorTypes:
            with self.subTest(detectorType=detectorType):
                self.faceEngine.setSettingsProvider(self.config)
                detector = self.faceEngine.createDetector(detectorType)
                cases = [
                    {"refDetectionType": fe.DetectionType(fe.DT_BBOX | fe.DT_LANDMARKS5),
                     "compareDetectionType": fe.DT_BBOX,
                     "landmarks5Valid": False, "landmarks68Valid": False
                     },
                    {"refDetectionType": fe.DetectionType(fe.DT_BBOX | fe.DT_LANDMARKS5),
                     "compareDetectionType": fe.DetectionType(fe.DT_BBOX | fe.DT_LANDMARKS5),
                     "landmarks5Valid": True, "landmarks68Valid": False
                     },
                    {"refDetectionType": fe.DetectionType(fe.DT_BBOX | fe.DT_LANDMARKS5 | fe.DT_LANDMARKS68),
                     "compareDetectionType": fe.DetectionType(fe.DT_BBOX | fe.DT_LANDMARKS68),
                     "landmarks5Valid": False, "landmarks68Valid": True
                     },
                    {"refDetectionType": fe.DetectionType(fe.DT_ALL),
                     "compareDetectionType": fe.DetectionType(fe.DT_BBOX | fe.DT_LANDMARKS68),
                     "landmarks5Valid": False, "landmarks68Valid": True
                     },
                ]
                for i, case in enumerate(cases):
                    with self.subTest(caseIndex=i):
                        resOne, refFace = detector.detectOne(self.image, self.image.getRect(), case['refDetectionType'])
                        self.assertTrue(resOne.isOk)
                        res, facesBatch = detector.detect(self.images_batch_for_detect, self.rectangles, 10,
                                                         case["compareDetectionType"])
                        self.assertTrue(res.isOk)
                        self.assertEqual(facesBatch.getSize(), len(self.images_batch_for_detect))
                        for i in range(0, facesBatch.getSize()):
                            detections = facesBatch.getDetections(i)
                            self.assertTrue(detections[0].isValid())
                            self.assertDetections(refFace.detection, detections[0])

    def testHumanDetector(self):
        humanDetector = self.faceEngine.createHumanDetector()
        image = fe.Image()
        err_image = image.load(os.path.join(testDataPath, "0_Parade_marchingband_1_620.ppm"))
        self.assertTrue(err_image.isOk)
        err_human_detector, humanBatch = humanDetector.detect(
            [image],
            [image.getRect()],
            10,
            fe.HumanDetectionType(fe.HDT_BOX))
        self.assertTrue(err_human_detector.isOk)
        self.assertEqual(humanBatch.getSize(), 1)

        detection = humanBatch.getDetections(0)[0]
        rect = detection.getRect()
        self.assertEqual(394, rect.x)
        self.assertEqual(227, rect.y)
        self.assertEqual(107, rect.width)
        self.assertEqual(270, rect.height)

    def testRedetectDifferentImages(self):
        images = []
        ROIs = []
        image_list = ["image1.ppm", "image_720.jpg", "mouth.ppm"]
        for value in image_list:
            image = fe.Image()
            err = image.load(os.path.join(testDataPath, value))
            self.assertTrue(err.isOk)
            images.append(image)
            ROIs.append(image.getRect())

        detectorTypes = {fe.FACE_DET_V1, fe.FACE_DET_V2, fe.FACE_DET_V3}
        for detectorType in detectorTypes:
            with self.subTest(detectorType=detectorType):
                self.faceEngine.setSettingsProvider(self.config)
                detector = self.faceEngine.createDetector(detectorType)
                faces = []
                redetections = []
                for img in images:
                    res, face = detector.detectOne(img, img.getRect(),
                                                   fe.DetectionType(fe.DT_BBOX | fe.DT_LANDMARKS5 | fe.DT_LANDMARKS68))
                    self.assertTrue(res.isOk)
                    self.assertFaceValid(face)
                    faces.append(face)
                for face in faces:
                    err_red, redection = detector.redetectOne(face.img, face.detection, fe.DetectionType(fe.DT_ALL))
                    self.assertTrue(err_red.isOk)
                    redetections.append(redection)

                res, facesBatch = detector.detect(
                    images,
                    ROIs,
                    10,
                    fe.DetectionType(fe.DT_BBOX | fe.DT_LANDMARKS68)
                )
                self.assertTrue(res.isOk)

                res1, redetectFacesBatch = detector.redetect(
                    images,
                    facesBatch,
                    fe.DetectionType(fe.DT_BBOX | fe.DT_LANDMARKS68)
                )
                self.assertTrue(res1.isOk)
                self.assertEqual(redetectFacesBatch.getSize(), len(images))

                for i in range(redetectFacesBatch.getSize()):
                    detections = redetectFacesBatch.getDetections(i)
                    landmarks68 = redetectFacesBatch.getLandmarks68(i)
                    for j in range(len(detections)):
                        self.assertDetections(redetections[i].detection, detections[j])
                        self.assertFaceLandmarks(redetections[i].landmarks68_opt.value(), landmarks68[j])

                val_err, val_result = detector.validate(
                    [face.img, face.img, fe.Image()],
                    [[fe.Detection()],
                     [face.detection, fe.Detection()],
                     [face.detection, face.detection, fe.Detection()]])

                self.assertTrue(val_err.isError)
                errors0 = val_result[0]
                self.assertEqual(len(errors0), 1)
                errors1 = val_result[1]
                self.assertEqual(len(errors1), 2)
                errors2 = val_result[2]
                self.assertEqual(len(errors2), 3)

    def testRedetectOne(self):
        ref_values = {fe.FACE_DET_V1: fe.Detection(fe.RectFloat(290.0, 75.0, 150.0, 197.0), 0.99999),
                      fe.FACE_DET_V3: fe.Detection(fe.RectFloat(292.0, 92.0, 151.0, 193.0), 0.9983)}
        for detectorType, detection in ref_values.items():
            with self.subTest(detectorType=detectorType):
                self.faceEngine.setSettingsProvider(self.config)
                detector = self.faceEngine.createDetector(detectorType)
                err, face = detector.detectOne(self.image, self.image.getRect(), fe.DetectionType(fe.DT_ALL))
                self.assertTrue(err.isOk)
                self.assertFaceValid(face)
                # redetection
                err_redetect1, face_redection1 = detector.redetectOne(face.img, face.detection, fe.DetectionType(fe.DT_ALL))
                self.assertFaceValid(face_redection1)
                self.assertDetections(face_redection1.detection, detection, delta=3, scoreDelta=0.001)

    def testRedetectBatchBbox(self):
        detectorTypes = {fe.FACE_DET_V1, fe.FACE_DET_V3}
        for detectorType in detectorTypes:
            with self.subTest(detectorType=detectorType):
                self.faceEngine.setSettingsProvider(self.config)
                detector = self.faceEngine.createDetector(detectorType)

                res_one, refFace = detector.redetectOne(
                    self.image_for_redetect,
                    self.detection_for_redetect,
                    fe.DetectionType(fe.DT_BBOX | fe.DT_LANDMARKS5))
                self.assertTrue(res_one.isOk)

                res, facesBatch = detector.redetect(
                    self.images_batch_for_detect,
                    self.detections_batch_for_redetect,
                    fe.DetectionType(fe.DT_BBOX))
                self.assertTrue(res.isOk)
                self.assertEqual(len(self.images_batch_for_detect), facesBatch.getSize())

                for i in range(0, facesBatch.getSize()):
                    detections = facesBatch.getDetections(i)
                    for j in range(0, len(detections)):
                        self.assertDetections(refFace.detection, detections[j])

    def testRedetectBatchLandmarks5(self):
        detectorTypes = {fe.FACE_DET_V1, fe.FACE_DET_V3}
        for detectorType in detectorTypes:
            with self.subTest(detectorType=detectorType):
                self.faceEngine.setSettingsProvider(self.config)
                detector = self.faceEngine.createDetector(detectorType)

                res_one, refFace = detector.redetectOne(
                    self.image_for_redetect,
                    self.detection_for_redetect,
                    fe.DetectionType(fe.DT_BBOX | fe.DT_LANDMARKS5))
                self.assertTrue(res_one.isOk)

                res, facesBatch = detector.redetect(
                    self.images_batch_for_detect,
                    self.detections_batch_for_redetect,
                    fe.DetectionType(fe.DT_BBOX | fe.DT_LANDMARKS5))
                self.assertTrue(res.isOk)
                self.assertEqual(len(self.images_batch_for_detect), facesBatch.getSize())

                for i in range(0, facesBatch.getSize()):
                    detections = facesBatch.getDetections(i)
                    landmarks5 = facesBatch.getLandmarks5(i)
                    for j in range(0, len(detections)):
                        self.assertDetections(refFace.detection, detections[j])
                        self.assertFaceLandmarks(landmarks5[j], refFace.landmarks5_opt.value())

    def testRedetectBatchLandmarks68(self):
        detectorTypes = {fe.FACE_DET_V1, fe.FACE_DET_V3}
        for detectorType in detectorTypes:
            with self.subTest(detectorType=detectorType):
                self.faceEngine.setSettingsProvider(self.config)
                detector = self.faceEngine.createDetector(detectorType)

                res_one, refFace = detector.redetectOne(
                    self.image_for_redetect,
                    self.detection_for_redetect,
                    fe.DetectionType(fe.DT_LANDMARKS5 | fe.DT_LANDMARKS68))
                self.assertTrue(res_one.isOk)
                res, facesBatch = detector.redetect(
                    self.images_batch_for_detect,
                    self.detections_batch_for_redetect,
                    fe.DetectionType(fe.DT_BBOX | fe.DT_LANDMARKS68))
                self.assertTrue(res.isOk)
                self.assertEqual(len(self.images_batch_for_detect), facesBatch.getSize())

                for i in range(0, facesBatch.getSize()):
                    detections = facesBatch.getDetections(i)
                    landmarks68 = facesBatch.getLandmarks68(i)
                    for j in range(0, len(detections)):
                        self.assertDetections(refFace.detection, detections[j])
                        self.assertFaceLandmarks(refFace.landmarks68_opt.value(), landmarks68[j])

    def testRedetectBatchAll(self):
        detectorTypes = {fe.FACE_DET_V1, fe.FACE_DET_V3}
        for detectorType in detectorTypes:
            with self.subTest(detectorType=detectorType):
                self.faceEngine.setSettingsProvider(self.config)
                detector = self.faceEngine.createDetector(detectorType)

                res_one, refFace = detector.redetectOne(
                    self.image_for_redetect,
                    self.detection_for_redetect,
                    fe.DetectionType(fe.DT_ALL)
                )
                self.assertTrue(res_one.isOk)
                res, faceBatch = detector.redetect(
                    self.images_batch_for_detect,
                    self.detections_batch_for_redetect,
                    fe.DetectionType(fe.DT_BBOX | fe.DT_LANDMARKS5 | fe.DT_LANDMARKS68)
                )
                self.assertTrue(res.isOk)

                self.assertEqual(len(self.images_batch_for_detect), faceBatch.getSize())
                for i in range (0, faceBatch.getSize()):
                    detections = faceBatch.getDetections(i)
                    landmarks5 = faceBatch.getLandmarks5(i)
                    landmarks68 = faceBatch.getLandmarks68(i)
                    for j in range (0, len(detections)):
                        detection = detections[j]
                        self.assertTrue(detection.isValid())
                        self.assertDetections(refFace.detection, detection)
                        self.assertFaceLandmarks(refFace.landmarks5_opt.value(), landmarks5[j])
                        self.assertFaceLandmarks(refFace.landmarks68_opt.value(), landmarks68[j])

    def testFace(self):
        face = fe.Face()
        self.assertFalse(face.isValid())
        face = fe.Face(self.image)
        self.assertFalse(face.isValid())
        face_initiated_by_int = fe.Face(self.image, fe.Detection(self.image.getRect(), 0.97))
        face_initiated_by_float = fe.Face(self.image, fe.Detection(self.image.getRect(), 0.97))
        self.assertFaceValid(face_initiated_by_int, landmarks5Valid=False, landmarks68Valid=False)
        self.assertFaceValid(face_initiated_by_float, landmarks68Valid=False, landmarks5Valid=False)
        self.assertDetections(face_initiated_by_int.detection, face_initiated_by_float.detection)

    def testRedetectByBadBbox(self, _detectorType=fe.FACE_DET_V3):
        detector = self.faceEngine.createDetector(_detectorType)
        image = fe.Image()
        err_image = image.load(os.path.join(testDataPath, "detect_1.jpeg"))
        self.assertTrue(err_image.isOk)
        detection = fe.Detection(fe.RectFloat(120, 599, 15, 10), 0.999999)
        err, redet = detector.redetectOne(image, detection, fe.DetectionType(fe.DT_BBOX))
        self.assertEqual(err.error, fe.FSDKError.InvalidRect)

    def testRedetectByBigBbox(self, _detectorType=fe.FACE_DET_V3):
        detector = self.faceEngine.createDetector(_detectorType)
        image = fe.Image()
        err_image = image.load(os.path.join(testDataPath, "image_720.jpg"))
        self.assertTrue(err_image.isOk)
        detection = fe.Detection(fe.RectFloat(0, 0, 10 ** 9, 10 ** 9), 0.999999)
        err, redet = detector.redetectOne(image, detection, fe.DetectionType(fe.DT_BBOX))
        self.assertFaceValid(redet, landmarks68Valid=False, landmarks5Valid=False)

    def testDetectByBigArea(self, _detectorType=fe.FACE_DET_V3):
        detector = self.faceEngine.createDetector(_detectorType)
        image = fe.Image()
        err_image = image.load(os.path.join(testDataPath, "attrib1.jpg"))
        self.assertTrue(err_image.isOk)
        rect = fe.Rect(100, 100, image.getWidth(), image.getHeight())
        err, face = detector.detectOne(image, rect, fe.DetectionType(fe.DT_BBOX | fe.DT_LANDMARKS5))
        self.assertEqual(err.error, fe.FSDKError.InvalidRect)

    def testRedetectResetLandmarks(self):
        self.faceEngine.setSettingsProvider(self.config)
        detector = self.faceEngine.createDetector()
        err, face = detector.detectOne(self.image, self.image.getRect(), fe.DetectionType(fe.DT_ALL))
        self.assertTrue(err.isOk)
        self.assertFaceValid(face, landmarks68Valid=True, landmarks5Valid=True)
        res_one, face2 = detector.redetectOne(face.img, face.detection, fe.DetectionType(fe.DT_BBOX))
        self.assertFaceValid(face2, landmarks68Valid=False, landmarks5Valid=False)

if __name__ == '__main__':
    unittest.main()
