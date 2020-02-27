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
        cls.face_for_redetect = fe.Face(cls.image, fe.Detection(fe.Rect(289, 94, 147, 185), 0.999999))
        cls.faces_batch_for_redetect = [cls.face_for_redetect] * 10
        cls.images_batch_for_detect = [cls.image] * 10
        cls.rectangles = [cls.image.getRect()] * 10

    def setUp(self):
        self.configPath = os.path.join("data", "faceengine.conf")
        self.config = fe.createSettingsProvider(self.configPath)
        self.config.setValue("FaceDetV3::Settings", "minFaceSize", 20)

    def assertFacesWithEtalons(self, face_expected, face):
        self.assertDetections(face_expected.detection, face.detection, delta=3, scoreDelta=0.001)
        if face.landmarks68_opt.isValid():
            absoluteLandmarks = self.addDetectionCoordinatesToLandmarks(face, face.landmarks68_opt)
            self.assertFaceLandmarks(face_expected.landmarks68_opt, absoluteLandmarks, delta=4)

    def assertFaceValid(self, face, landmarks5Valid=True, landmarks68Valid=True):
        self.assertTrue(face.isValid())
        self.assertTrue(face.landmarks5_opt.isValid() == landmarks5Valid)
        self.assertTrue(face.landmarks68_opt.isValid() == landmarks68Valid)

    def assertErrors(self, errorsList, valid):
        for error in errorsList:
            self.assertEqual(error.isOk, valid)

    def assertFaceLandmarks(self, face1_landmarks, face2_landmarks, delta=0):
        self.assertEqual(len(face1_landmarks.value()), len(face2_landmarks.value()))
        for k in range(len(face2_landmarks.value())):
            self.assertAlmostEqual(face1_landmarks.value()[k].x, face2_landmarks.value()[k].x, delta=delta)
            self.assertAlmostEqual(face1_landmarks.value()[k].y, face2_landmarks.value()[k].y, delta=delta)

    def assertDetections(self, detection1, detection2, delta=0.0, scoreDelta=0.0):
        self.assertAlmostEqual(detection1.rect.x, detection2.rect.x, delta=delta)
        self.assertAlmostEqual(detection1.rect.y, detection2.rect.y, delta=delta)
        self.assertAlmostEqual(detection1.rect.width, detection2.rect.width, delta=delta)
        self.assertAlmostEqual(detection1.rect.height, detection2.rect.height, delta=delta)
        self.assertAlmostEqual(detection1.score, detection2.score, delta=scoreDelta)

    def addDetectionCoordinatesToLandmarks(self, face, landmarks):
        absoluteLandmarks = fe.Landmarks68()
        for k in range(len(landmarks.value())):
            absoluteLandmarks[k] = fe.Vector2f(face.detection.rect.x + landmarks.value()[k].x,
                                               face.detection.rect.y + landmarks.value()[k].y)
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
                                                   fe.DetectionType(fe.dtBBox | fe.dt5Landmarks | fe.dt68Landmarks))
                    self.assertTrue(res.isOk)
                    self.assertFaceValid(face)
                    refFaces.append(face)
                res3, facesList = detector.detect(images, rectangles, 3,
                                                  fe.DetectionType(fe.dtBBox | fe.dt68Landmarks))
                self.assertTrue(res3.isOk)
                for i, faces in enumerate(facesList):
                    self.assertFaceValid(faces[0], landmarks5Valid=False)
                    self.assertDetections(refFaces[i].detection, faces[0].detection)
                    self.assertFaceLandmarks(refFaces[i].landmarks68_opt, faces[0].landmarks68_opt)

    def testDetectorBBoxLandmarks68(self):
        ref_values = {fe.FACE_DET_V1: fe.DetectionFloat(fe.RectFloat(288.0, 93.0, 148.0, 184.0), 0.99999),
                      fe.FACE_DET_V2: fe.DetectionFloat(fe.RectFloat(297.0, 97.0, 152.0, 184.0), 0.999986),
                      fe.FACE_DET_V3: fe.DetectionFloat(fe.RectFloat(296.0, 73.0, 145.0, 211.0), 0.999936)}
        lnetExpected = fe.Landmarks68()

        for detectorType, detection in ref_values.items():
            with self.subTest(detectorType=detectorType):
                self.faceEngine.setSettingsProvider(self.config)
                detector = self.faceEngine.createDetector(detectorType)
                res_one, face = detector.detectOne(self.image, self.image.getRect(),
                                                   fe.DetectionType(fe.dtBBox | fe.dt5Landmarks | fe.dt68Landmarks))
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
                    {"refDetectionType": fe.DetectionType(fe.dtBBox | fe.dt5Landmarks),
                     "compareDetectionType": fe.dtBBox,
                     "landmarks5Valid": False, "landmarks68Valid": False
                     },
                    {"refDetectionType": fe.DetectionType(fe.dtBBox | fe.dt5Landmarks),
                     "compareDetectionType": fe.DetectionType(fe.dtBBox | fe.dt5Landmarks),
                     "landmarks5Valid": True, "landmarks68Valid": False
                     },
                    {"refDetectionType": fe.DetectionType(fe.dtBBox | fe.dt5Landmarks | fe.dt68Landmarks),
                     "compareDetectionType": fe.DetectionType(fe.dtBBox | fe.dt68Landmarks),
                     "landmarks5Valid": False, "landmarks68Valid": True
                     },
                    {"refDetectionType": fe.DetectionType(fe.dtAll),
                     "compareDetectionType": fe.DetectionType(fe.dtBBox | fe.dt68Landmarks),
                     "landmarks5Valid": False, "landmarks68Valid": True
                     },
                ]
                for i, case in enumerate(cases):
                    with self.subTest(caseIndex=i):
                        resOne, refFace = detector.detectOne(self.image, self.image.getRect(), case['refDetectionType'])
                        self.assertTrue(resOne.isOk)
                        res, facesList = detector.detect(self.images_batch_for_detect, self.rectangles, 10,
                                                         case["compareDetectionType"])
                        self.assertTrue(res.isOk)
                        self.assertEqual(len(facesList), len(self.images_batch_for_detect))
                        for faces in facesList:
                            self.assertFaceValid(faces[0], landmarks5Valid=case['landmarks5Valid'],
                                                 landmarks68Valid=case['landmarks68Valid'])
                            self.assertDetections(refFace.detection, faces[0].detection)

    def testHumanDetector(self):
        humanDetector = self.faceEngine.createHumanDetector()
        image = fe.Image()
        err_image = image.load(os.path.join(testDataPath, "0_Parade_marchingband_1_620.ppm"))
        self.assertTrue(err_image.isOk)
        err_human_detector, list_of_list_of_detections = humanDetector.detect(
            [image],
            [image.getRect()],
            10,
            fe.HumanDetectionType(fe.DCT_BOX))
        self.assertTrue(err_human_detector.isOk)
        self.assertEqual(74, list_of_list_of_detections[0][0].detection.rect.x)
        self.assertEqual(235, list_of_list_of_detections[0][0].detection.rect.y)
        self.assertEqual(153, list_of_list_of_detections[0][0].detection.rect.width)
        self.assertEqual(298, list_of_list_of_detections[0][0].detection.rect.height)

    def testRedetectDifferentImages(self):
        images = []
        image_list = ["image1.ppm", "image_720.jpg", "mouth.ppm"]
        for value in image_list:
            image = fe.Image()
            err = image.load(os.path.join(testDataPath, value))
            self.assertTrue(err.isOk)
            images.append(image)

        detectorTypes = {fe.FACE_DET_V1, fe.FACE_DET_V2, fe.FACE_DET_V3}
        for detectorType in detectorTypes:
            with self.subTest(detectorType=detectorType):
                self.faceEngine.setSettingsProvider(self.config)
                detector = self.faceEngine.createDetector(detectorType)
                faces = []
                faces_redetect = []
                for img in images:
                    res, face = detector.detectOne(img, img.getRect(),
                                                   fe.DetectionType(fe.dtBBox | fe.dt5Landmarks | fe.dt68Landmarks))
                    self.assertTrue(res.isOk)
                    self.assertFaceValid(face)
                    faces.append(face)
                for face in faces:
                    err_red, redection = detector.redetectOne(face, fe.DetectionType(fe.dtAll))
                    self.assertTrue(err_red.isOk)
                    faces_redetect.append(redection)
                res1, facesList, errorList = detector.redetect(faces, fe.DetectionType(fe.dtBBox | fe.dt68Landmarks))
                self.assertTrue(res.isOk)
                self.assertErrors(errorList, True)

                for i, face_redetect in enumerate(faces_redetect):
                    self.assertDetections(face_redetect.detection, facesList[i].detection)
                    self.assertFaceLandmarks(face_redetect.landmarks68_opt, facesList[i].landmarks68_opt)

    def testRedetectOne(self):
        ref_values = {fe.FACE_DET_V1: fe.DetectionFloat(fe.RectFloat(290.0, 75.0, 150.0, 197.0), 0.99999),
                      fe.FACE_DET_V3: fe.DetectionFloat(fe.RectFloat(293.0, 96.0, 150.0, 192.0), 0.997161)}
        for detectorType, detection in ref_values.items():
            with self.subTest(detectorType=detectorType):
                self.faceEngine.setSettingsProvider(self.config)
                detector = self.faceEngine.createDetector(detectorType)
                err, face = detector.detectOne(self.image, self.image.getRect(), fe.DetectionType(fe.dtAll))
                self.assertTrue(err.isOk)
                self.assertFaceValid(face)
                # redetection
                err_redetect1, face_redection1 = detector.redetectOne(face, fe.DetectionType(fe.dtAll))
                self.assertFaceValid(face_redection1)
                self.assertDetections(face_redection1.detection, detection, delta=3, scoreDelta=0.001)

                err_redetect2, face_redection2 = detector.redetectOne(face.img, face.detection.rect,
                                                                      fe.DetectionType(fe.dtAll))
                self.assertFaceValid(face_redection2)
                self.assertDetections(face_redection2.detection, detection, delta=3, scoreDelta=0.001)

    def testRedetectBatchBbox(self):
        detectorTypes = {fe.FACE_DET_V1, fe.FACE_DET_V3}
        for detectorType in detectorTypes:
            with self.subTest(detectorType=detectorType):
                self.faceEngine.setSettingsProvider(self.config)
                detector = self.faceEngine.createDetector(detectorType)

                res_one, refFace = detector.redetectOne(self.face_for_redetect, fe.DetectionType(fe.dtBBox | fe.dt5Landmarks))
                self.assertTrue(res_one.isOk)
                res, faces, errorList = detector.redetect(self.faces_batch_for_redetect, fe.DetectionType(fe.dtBBox))
                self.assertTrue(res.isOk)
                self.assertErrors(errorList, True)

                for face in faces:
                    self.assertFaceValid(face, landmarks5Valid=False, landmarks68Valid=False)
                    self.assertDetections(refFace.detection, face.detection)
                self.assertEqual(len(faces), len(self.faces_batch_for_redetect))

    def testRedetectBatchLandmarks5(self):
        detectorTypes = {fe.FACE_DET_V1, fe.FACE_DET_V3}
        for detectorType in detectorTypes:
            with self.subTest(detectorType=detectorType):
                self.faceEngine.setSettingsProvider(self.config)
                detector = self.faceEngine.createDetector(detectorType)

                res_one, refFace = detector.redetectOne(self.face_for_redetect, fe.DetectionType(fe.dtBBox | fe.dt5Landmarks))
                self.assertTrue(res_one.isOk)
                res, faces, errorList = detector.redetect(self.faces_batch_for_redetect,
                                               fe.DetectionType(fe.dtBBox | fe.dt5Landmarks))
                self.assertTrue(res.isOk)
                self.assertErrors(errorList, True)

                for face in faces:
                    self.assertFaceValid(face, landmarks5Valid=True, landmarks68Valid=False)
                    self.assertDetections(refFace.detection, face.detection)
                    self.assertFaceLandmarks(face.landmarks5_opt, refFace.landmarks5_opt)
                self.assertEqual(len(faces), len(self.faces_batch_for_redetect))

    def testRedetectBatchLandmarks68(self):
        detectorTypes = {fe.FACE_DET_V1, fe.FACE_DET_V3}
        for detectorType in detectorTypes:
            with self.subTest(detectorType=detectorType):
                self.faceEngine.setSettingsProvider(self.config)
                detector = self.faceEngine.createDetector(detectorType)

                res_one, refFace = detector.redetectOne(self.face_for_redetect,
                                                        fe.DetectionType(fe.dt5Landmarks | fe.dt68Landmarks))
                self.assertTrue(res_one.isOk)
                res, faces, errorList = detector.redetect(self.faces_batch_for_redetect,
                                               fe.DetectionType(fe.dtBBox | fe.dt68Landmarks))
                self.assertTrue(res.isOk)
                self.assertErrors(errorList, True)

                for face in faces:
                    self.assertFaceValid(face, landmarks5Valid=False, landmarks68Valid=True)
                    self.assertDetections(refFace.detection, face.detection)
                    self.assertFaceLandmarks(face.landmarks68_opt, refFace.landmarks68_opt)
                self.assertEqual(len(faces), len(self.faces_batch_for_redetect))

    def testRedetectBatchAll(self):
        detectorTypes = {fe.FACE_DET_V1, fe.FACE_DET_V3}
        for detectorType in detectorTypes:
            with self.subTest(detectorType=detectorType):
                self.faceEngine.setSettingsProvider(self.config)
                detector = self.faceEngine.createDetector(detectorType)

                res_one, refFace = detector.redetectOne(self.face_for_redetect, fe.DetectionType(fe.dtAll))
                self.assertTrue(res_one.isOk)
                res, faces, errorList = detector.redetect(self.faces_batch_for_redetect,
                                               fe.DetectionType(fe.dtBBox | fe.dt5Landmarks | fe.dt68Landmarks))
                self.assertTrue(res.isOk)
                self.assertErrors(errorList, True)

                for face in faces:
                    self.assertFaceValid(face)
                    self.assertDetections(refFace.detection, face.detection)
                    self.assertFaceLandmarks(refFace.landmarks5_opt, face.landmarks5_opt)
                    self.assertFaceLandmarks(refFace.landmarks68_opt, face.landmarks68_opt)
                self.assertEqual(len(faces), len(self.faces_batch_for_redetect))

    def testFace(self):
        face = fe.Face()
        self.assertFalse(face.isValid())
        face = fe.Face(self.image)
        self.assertFalse(face.isValid())
        face_initiated_by_int = fe.Face(self.image, fe.Detection(self.image.getRect(), 0.97))
        face_initiated_by_float = fe.Face(self.image, fe.DetectionFloat(self.image.getRect(), 0.97))
        self.assertFaceValid(face_initiated_by_int, landmarks5Valid=False, landmarks68Valid=False)
        self.assertFaceValid(face_initiated_by_float, landmarks68Valid=False, landmarks5Valid=False)
        self.assertDetections(face_initiated_by_int.detection, face_initiated_by_float.detection)

    def testRedetectByBadBbox(self, _detectorType=fe.FACE_DET_V3):
        detector = self.faceEngine.createDetector(_detectorType)
        image = fe.Image()
        err_image = image.load(os.path.join(testDataPath, "detect_1.jpeg"))
        self.assertTrue(err_image.isOk)
        box = fe.RectFloat(120, 599, 15, 10)
        err, redet = detector.redetectOne(image, box, fe.DetectionType(fe.dtBBox))
        self.assertEqual(err.error, fe.FSDKError.InvalidRect)

    def testRedetectByBigBbox(self, _detectorType=fe.FACE_DET_V3):
        detector = self.faceEngine.createDetector(_detectorType)
        image = fe.Image()
        err_image = image.load(os.path.join(testDataPath, "image_720.jpg"))
        self.assertTrue(err_image.isOk)
        box = fe.RectFloat(0, 0, 10 ** 9, 10 ** 9)
        err, redet = detector.redetectOne(image, box, fe.DetectionType(fe.dtBBox))
        self.assertFaceValid(redet, landmarks68Valid=False, landmarks5Valid=False)

    def testDetectByBigArea(self, _detectorType=fe.FACE_DET_V3):
        detector = self.faceEngine.createDetector(_detectorType)
        image = fe.Image()
        err_image = image.load(os.path.join(testDataPath, "attrib1.jpg"))
        self.assertTrue(err_image.isOk)
        area = fe.Rect(100, 100, image.getWidth(), image.getHeight())
        err, face = detector.detectOne(image, area, fe.DetectionType(fe.dtBBox | fe.dt5Landmarks))
        self.assertEqual(err.error, fe.FSDKError.InvalidRect)

    def testRedetectResetLandmarks(self):
        self.faceEngine.setSettingsProvider(self.config)
        detector = self.faceEngine.createDetector()
        err, face = detector.detectOne(self.image, self.image.getRect(), fe.DetectionType(fe.dtAll))
        self.assertTrue(err.isOk)
        self.assertFaceValid(face, landmarks68Valid=True, landmarks5Valid=True)
        res_one, face2 = detector.redetectOne(face, fe.DetectionType(fe.dtBBox))
        self.assertFaceValid(face2, landmarks68Valid=False, landmarks5Valid=False)


if __name__ == '__main__':
    unittest.main()
