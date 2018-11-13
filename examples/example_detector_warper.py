import sys

def help():
    print("python example_detector_warper.py <path to FaceEngine*.so> <path to image>")

if len(sys.argv) != 3:
    help()
    exit(1)

# if FaceEngine is not installed pass the path to dir with FaceEngine*.so and add it to system paths
sys.path.append(sys.argv[1])
# if FaceEngine is installed only
import FaceEngine as fe
# correct paths or put directory "data" with example_detector_warper.py
faceEngine = fe.createFaceEngine("data", "data/faceengine.conf")


def detector_example(_image_det,
                     _max_detections,
                     _comparer_type=fe.DetectionComparerType.DCT_CENTER,
                     _detector_type=fe.ODT_MTCNN,
                     _config=None):
    if _detector_type == fe.ODT_S3FD and _config:
        _config.setValue("system", "betaMode", fe.SettingsProviderValue(1))
        faceEngine.setSettingsProvider(_config)
    detector = faceEngine.createDetector(_detector_type)
    detector.setDetectionComparer(_comparer_type)
    detector_result = detector.detect(_image_det, _image_det.getRect(), _max_detections)
    return detector_result


def detector_batch_example(_image_det, _max_detections, _detector_type=fe.ODT_MTCNN, _config=None):
    if _detector_type == fe.ODT_S3FD and _config:
        _config.setValue("system", "betaMode", fe.SettingsProviderValue(1))
        faceEngine.setSettingsProvider(_config)
    detector = faceEngine.createDetector(_detector_type)
    err, detector_result = detector.detect([_image_det,
                                           _image_det,
                                           _image_det],
                                           [_image_det.getRect(),
                                           _image_det.getRect(),
                                           _image_det.getRect()],
                                           _max_detections,
                                           fe.DetectionType(fe.dt5Landmarks | fe.dt68Landmarks))
    print(detector_result[0][0].detection)
    print(detector_result[0][0].landmarks5_opt.isValid())
    print(detector_result[0][0].landmarks68_opt.isValid())
    print(detector_result[0][0].landmarks68_opt.value()[0])
    print(detector_result[0][0].landmarks5_opt.value()[0])
    return err, detector_result


def detector_one_example(_image_det, _detector_type=fe.ODT_MTCNN, _config=None):
    if _detector_type == fe.ODT_S3FD and _config:
        _config.setValue("system", "betaMode", fe.SettingsProviderValue(1))
        faceEngine.setSettingsProvider(_config)
    detector = faceEngine.createDetector(_detector_type)
    err, detector_result = detector.detectOne(_image_det,
                                           _image_det.getRect(),
                                           fe.DetectionType(fe.dt5Landmarks | fe.dt68Landmarks))
    print(detector_result.detection)
    print(detector_result.landmarks5_opt.isValid())
    print(detector_result.landmarks68_opt.isValid())
    print(detector_result.landmarks68_opt.value()[0])
    print(detector_result.landmarks5_opt.value()[0])
    return err, detector_result


def detector_example_light(_image_det, max_detections):
    detector = faceEngine.createDetector(fe.ODT_MTCNN)
    detector_result = detector.detect_light(_image_det, _image_det.getRect(), max_detections)
    return detector_result


def detector_example_5(_image_det, max_detections):
    detector = faceEngine.createDetector(fe.ODT_MTCNN)
    detector_result = detector.detect5(_image_det, _image_det.getRect(), max_detections)
    return detector_result


def warper_example(image_det, _detection, _landmarks5, _landmarks68):
    warper = faceEngine.createWarper()
    transformation = warper.createTransformation(_detection, _landmarks5)
    print(transformation)
    warp_result = warper.warp(image_det, transformation)
    if warp_result[0].isError:
        print("Failed image warping.")
        return None
    _warp_image = warp_result[1]
    err_transformed_landmarks5, _transformed_landmarks5 = warper.warp(_landmarks5, transformation)
    if err_transformed_landmarks5.isError:
        print("Failed extraction of transformed landmarsks5.")
        return None
    err_transformed_landmarks68, _transformed_landmarks68 = warper.warp(_landmarks68, transformation)
    if err_transformed_landmarks68.isError:
        print("Failed extraction of transformed landmarsks68.")
        return None
    return (_warp_image, _transformed_landmarks5, _transformed_landmarks68)


def set_logging(value):
    config = fe.createSettingsProvider("data/faceengine.conf")
    config_path = config.getDefaultPath()
    print("Config settings: DefaultPath {0}".format(config_path))
    config.setValue("system", "verboseLogging", fe.SettingsProviderValue(value))
    faceEngine.setSettingsProvider(config)
    val = config.getValue("system", "verboseLogging")
    print("Config settings: \"system\", \"verboseLogging\" = {0}".format(val.asInt()))
    return config


def print_landmarks(landmarks, message=""):
    print(message)
    for i in range(len(landmarks)):
        print(landmarks[i])


def print_landmarks_for_comparing(landmarks1, landmarks2, message=""):
    assert(len(landmarks1) == len(landmarks2))
    print(message)
    for i, _ in enumerate(landmarks1):
        print("{0}) {1}, {2}, delta_x = {3}, delta_y = {4}".format(
            i,
            landmarks1[i],
            landmarks2[i],
            landmarks1[i].x - landmarks2[i].x,
            landmarks1[i].y - landmarks2[i].y))


if __name__ == "__main__":
    face = fe.Face()
    image_path = sys.argv[2]
    config = set_logging(1)
    image = fe.Image()
    err_detect_ligth = image.load(image_path)
    if not image.isValid():
        print("Image error = ", err_detect_ligth)
    # unpack detector result - list of tuples
    # err_detect, detect_list = detector_example(image, 1)
    err_detect, detect_list = detector_example(image, 10, fe.DetectionComparerType.DCT_CENTER)

    if err_detect.isError or len(detect_list) < 1:
        print("detect: faces are not found")
        exit(-1)
    for item in detect_list:
        print(item[0])

    # only for example take first detection in list
    (detection, landmarks5, landmarks68) = detect_list[0]
    # light version return only list of detections
    err_detect_light, detect_light_result = detector_example_light(image, 1)
    if err_detect_ligth.isError:
        print("detect_light: faces are not found")
        exit(-1)
    # detect5 version return only list of tuples with detections and landmarks5
    err_detect5, detect5_tuple = detector_example_5(image, 1)
    if err_detect5.isError or len(detect5_tuple) < 1:
        print("detect_5: faces are not found")
        exit(-1)
    (warp_image, transformed_landmarks5, transformed_landmarks68) = \
        warper_example(image, detection, landmarks5, landmarks68)
    (_, landmarks5_warp, _) = detector_example(warp_image, 1)[1][0]
    print_landmarks(landmarks5, "landmarks5: ")
    print_landmarks(landmarks68, "landmarks68: ")
    print_landmarks(transformed_landmarks5, "transformedLandmarks5: ")

    # print_landmarks_for_comparing(landmarks5, landmarks5_warp, "Comparing landmarks")
    print("MSD", detection)
    err_batch, detect_list_batch = detector_batch_example(image, 3, fe.ODT_S3FD, config)

    # err_one, detect_one = detector_one_example(image, 3)
    # print_landmarks(detect_one[0].landmarks5_opt.value(), "landmarks5 test: ")





