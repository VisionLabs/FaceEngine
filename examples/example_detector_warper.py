import sys


def help():
    print("python example_detector_warper.py <path to dir with FaceEngine*.so> <path to image>")


if len(sys.argv) != 3:
    help()
    exit(1)

# if FaceEngine is not installed pass the path to dir with FaceEngine*.so and add it to system paths
sys.path.append(sys.argv[1])
# if FaceEngine is installed only
import FaceEngine as fe
from example_license import make_activation
# correct paths or put directory "data" with example_detector_warper.py
faceEngine = fe.createFaceEngine("data")
if not make_activation(faceEngine):
    print("failed to activate license!")
    exit(-1)


def detector_batch_example(_image_det, _max_detections, _detector_type=fe.FACE_DET_V1):
    detector = faceEngine.createDetector(_detector_type)
    image_list = [_image_det,
                  _image_det,
                  _image_det]
    rect_list = [_image_det.getRect(),
                 _image_det.getRect(),
                 _image_det.getRect()]
    return detector.detect(image_list,
                           rect_list,
                           _max_detections,
                           fe.DetectionType(fe.dt5Landmarks | fe.dt68Landmarks))


def detector_redetect_example(_image_det, _max_detections, _next_image, _detector_type=fe.FACE_DET_V3, _config=None):
    detector = faceEngine.createDetector(_detector_type)
    err, face_list = detector.detect(
        [_image_det, _image_det],
        [_image_det.getRect(), _image_det.getRect()],
        _max_detections,
        fe.DetectionType(fe.dt5Landmarks | fe.dt68Landmarks))
    # take first image and all its faces, rewrite img
    for face in face_list[0]:
        face.img = _next_image
    # DetectionType must be the same as in detect. Take only first type
    redetect_result = detector.redetect(face_list[0], fe.DetectionType(fe.dt5Landmarks | fe.dt68Landmarks))
    return redetect_result


def detector_redetect_one_example(_image_det, _next_image, _detector_type=fe.FACE_DET_V3):
    detector = faceEngine.createDetector(_detector_type)
    err, face = detector.detectOne(_image_det, _image_det.getRect(), fe.DetectionType(fe.dt5Landmarks))
    # DetectionType must be the same as in detect
    if face.isValid():
        face.img = _next_image
        redetect_result = detector.redetectOne(face, fe.dt5Landmarks)
        return redetect_result
    else:
        return None


def simple_redetect_example(image1, image2, _detector_type=fe.FACE_DET_V3):
    detector = faceEngine.createDetector(_detector_type)
    # Make detection on the first image
    det_result, face = detector.detectOne(image1, image1.getRect(), fe.DetectionType(fe.dt5Landmarks))
    if det_result.isError or not face.isValid:
        print("simple_redetect_example - failed to detect! Reason: {0}".format(det_result.what))
        return

    # Redetect by face
    redetect_result, redetected_face = detector.redetectOne(face, fe.dt5Landmarks)
    if redetect_result.isError:
        print("simple_redetect_example - failed to redetect! Reason: {0}".format(redetect_result.what))
        return
    if not redetected_face.isValid():
        print("simple_redetect_example - something goes wrong! Face structure is invalid after redetect!")
        return
    print("\nsimple_redetect_example - first result: {0}".format(face.detection))

    # And one more case - redetect based on the image and rect
    redetect_result, redetected_face = detector.redetectOne(image2, face.detection.rect, fe.dt5Landmarks)
    if redetect_result.isError:
        print("simple_redetect_example - failed to redetect! Reason: {0}".format(redetect_result.what))
        return
    if not redetected_face.isValid():
        print("simple_redetect_example - something goes wrong! Face structure is invalid after redetect!")
        return
    print("\nsimple_redetect_example - second result: {0}".format(redetected_face.detection))



def detector_one_example(_image_det, _detector_type=fe.FACE_DET_V1):
    detector = faceEngine.createDetector(_detector_type)
    err, detector_result = detector.detectOne(_image_det,
                                           _image_det.getRect(),
                                           fe.DetectionType(fe.dt5Landmarks | fe.dt68Landmarks))
    print(detector_result.detection)
    return err, detector_result


def warper_example(image_det, _detection, _landmarks5, _landmarks68):
    warper = faceEngine.createWarper()
    _transformation = warper.createTransformation(_detection, _landmarks5)
    print(_transformation)
    warp_result = warper.warp(image_det, _transformation)
    if warp_result[0].isError:
        print("Failed image warping.")
        return None
    _warp_image = warp_result[1]
    err_transformed_landmarks5, _transformed_landmarks5 = warper.warp(_landmarks5, _transformation)
    if err_transformed_landmarks5.isError:
        print("Failed extraction of transformed landmarsks5.")
        return None
    err_transformed_landmarks68, _transformed_landmarks68 = warper.warp(_landmarks68, _transformation)
    if err_transformed_landmarks68.isError:
        print("Failed extraction of transformed landmarsks68.")
        return None
    return (_warp_image, _transformed_landmarks5, _transformed_landmarks68, _transformation)


def unwarp_gaze(eye_angles, _transformation):
    warper = faceEngine.createWarper()
    unwarp_error, unwarp_result = warper.unwarp(eye_angles, _transformation)
    if unwarp_error.isError:
        print("Failed gaze unwarping.")
        return None
    return unwarp_result


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
    if not make_activation(faceEngine):
        print("failed to activate license!")
        exit(-1)
    image_path = sys.argv[2]
    config = set_logging(0)
    image = fe.Image()
    err_image_loaded = image.load(image_path)
    if not image.isValid():
        print("Image error = ", err_image_loaded)
        exit(-1)
    print("\nBatch interface example: ")
    n_detections = 3
    err_batch, detect_list_batch = detector_batch_example(image, n_detections, fe.FACE_DET_V3)
    if err_batch.isError or not detect_list_batch[0]:
        print("detector_batch_example: faces are not found")
        exit(-1)
    # print all detections in list
    i_detection = 0
    i_image = 0
    for item in detect_list_batch:
        for item_item in item:
            print("image " + str(i_image) + " detection " + str(i_detection) + ": ", item_item.detection)
            i_detection += 1
            # print landmarks if you need
            # if item_item.landmarks5_opt.isValid():
            #     print_landmarks(item_item.landmarks5_opt.value(), "image № " + str(i_image) + " detection № " + str(i_detection) + ", landmarks5 = ")
            # if item_item.landmarks68_opt.isValid():
            #     print_landmarks(item_item.landmarks68_opt.value())
        i_image += 1
        i_detection = 0

    # only for example take first detection in list
    face = detect_list_batch[0][0]
    if not face.landmarks5_opt.isValid() or not face.landmarks68_opt.isValid():
        print("landmarks are not valid, please verify and pass"
              " fe.DetectionType(fe.dt5Landmarks | fe.dt68Landmarks) if need")
        exit(-1)
    (detection, landmarks5, landmarks68) = face.detection, face.landmarks5_opt.value(), face.landmarks68_opt.value()
    (warp_image, transformed_landmarks5, transformed_landmarks68, transformation) = \
        warper_example(image, detection, landmarks5, landmarks68)
    simple_redetect_example(image, image, fe.FACE_DET_V3)
    print_landmarks(landmarks5, "landmarks5: ")
    print_landmarks(transformed_landmarks5, "transformedLandmarks5: ")
    # print_landmarks_for_comparing(landmarks5, landmarks5_warp, "Comparing landmarks")

    print("\nSimple interface example: ")
    err_one, face_one = detector_one_example(image, fe.FACE_DET_V3)
    if err_one.isError:
        print("err_one: faces are not found")
        exit(-1)
    if not face_one.landmarks5_opt.isValid() or not face_one.landmarks68_opt.isValid():
        print("landmarks are not valid")
        exit(-1)
    print_landmarks(face_one.landmarks5_opt.value(), "landmarks5, detectOne: ")
    # print_landmarks(face_one.landmarks68_opt.value(), "landmarks68, detectOne: ")

