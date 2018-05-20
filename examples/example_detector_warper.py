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


def detector_example(_image_det):
    print("\nDetector example")
    detector = faceEngine.createDetector(fe.ODT_MTCNN)
    max_detections = 3
    print("Image for detection: ", _image_det.getHeight(), _image_det.getWidth(), _image_det.isValid())
    detector_result = detector.detect(_image_det, _image_det.getRect(), max_detections)
    print("detector result = ", detector_result)
    test = detector_result[0][1]
    print("Landmarks test {0}".format(test[0]))
    print("Landmarks test {0}".format(test[0]))
    print("Detections: ")
    for i, item in enumerate(detector_result, 1):
        print(i, item)
    return detector_result


def warper_example(image_det, _detection, _landmarks5, _landmarks68):
    print("\nWarper example: ")
    warper = faceEngine.createWarper()
    transformation = warper.createTransformation(_detection,
                                                 _landmarks5)
    print("transformation = ", transformation)
    warp_image = warper.warp(image_det, transformation)
    print(warp_image.getWidth(), warp_image.getHeight(), warp_image.isValid())
    ethnicityEstimator = faceEngine.createEthnicityEstimator()
    estimation = ethnicityEstimator.estimate(warp_image)
    print("Ethnicity estimator on warped Image {0}".format(estimation))
    _transformed_landmarks5 = warper.warp(_landmarks5, transformation)
    print("\nTransformed landmarks5: ")
    for i in range(len(_transformed_landmarks5)):
        print(_transformed_landmarks5[i])
    _transformed_landmarks68 = warper.warp(_landmarks68, transformation)
    print("warperResult with Landmarks5 = ", _transformed_landmarks5)
    print("warperResult with Landmarks68 = ", _transformed_landmarks68)
    return (warp_image, _transformed_landmarks5, _transformed_landmarks68)


def set_logging(value):
    config = fe.createSettingsProvider("data/faceengine.conf")
    config_path = config.getDefaultPath()
    print("Config settings: DefaultPath {0}".format(config_path))
    config.setValue("system", "verboseLogging", fe.SettingsProviderValue(value))
    faceEngine.setSettingsProvider(config)
    val = config.getValue("system", "verboseLogging")
    print("Config settings: \"system\", \"verboseLogging\" = {0}".format(val.asInt()))


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
    image_path = sys.argv[2]
    set_logging(1)
    image = fe.Image()
    err = image.load(image_path)
    if not image.isValid():
        print("Image error = ", err)
    # unpack detector result - list of tuples
    (detection, landmarks5, landmarks68) = detector_example(image)[0]

    # print_landmarks(landmarks68, "landmarks68: ")
    (warp_image, transformed_landmarks5, transformed_landmarks68) = \
        warper_example(image, detection, landmarks5, landmarks68)
    print("warp_image = ", warp_image.getWidth(), warp_image.getHeight(), warp_image.isValid())

    (_, landmarks5_warp, _) = detector_example(warp_image)[0]
    print_landmarks(landmarks5, "landmarks5: ")
    print_landmarks(transformed_landmarks5, "transformedLandmarks5: ")
    print_landmarks(landmarks5_warp, "landmarks5_warp: ")
    print_landmarks(landmarks5_warp, "transformedLandmarks5: ")
    # print_landmarks_for_comparing(transformed_landmarks5, landmarks5_warp, "Comparing landmarks")




