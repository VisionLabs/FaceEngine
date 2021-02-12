import sys


def help():
    print("python example_human_detector.py <path to dir with FaceEngine*.so> <path to image>")


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
res = make_activation(faceEngine)
if res.isError:
    print("Failed to activate license! Reason: {0}".format(res.what))
    exit(-1)


def human_detect_example(image1, image2):
    detector = faceEngine.createHumanDetector()
    result, human_batch = detector.detect(
        [image1, image2],
        [image1.getRect(), image2.getRect()],
        10,
        fe.HumanDetectionType(fe.HDT_BOX))

    if not result.isOk:
        print("human_detect_example - failed to detect! Reason: {0}".format(result.what))
        return

    for i in range(human_batch.getSize()):
        detections = human_batch.getDetections(i)
        print("human_detect_example - next image results:")
        if len(detections) == 0:
            print("human_detect_example - no human on the image!")
        for j in range(len(detections)):
            print(detections[j])

def human_landmarks_detect_example(image1, image2):
    detector = faceEngine.createHumanDetector()
    result, human_batch = detector.detect(
        [image1, image2],
        [image1.getRect(), image2.getRect()],
        10,
        fe.HumanDetectionType(fe.HDT_BOX | fe.HDT_POINTS))
    if not result.isOk:
        print("human_landmarks_detect_example - failed to detect! Reason: {0}".format(result.what))
        return

    for i in range(human_batch.getSize()):
        detections = human_batch.getDetections(i)
        landmarks17 = human_batch.getLandmarks17(i)
        print("human_landmarks_detect_example - next image results:")
        if len(detections) == 0:
            print("human_landmarks_detect_example - no human on the image!")
        for j in range(len(detections)):
            print(detections[j])
            lm17 = landmarks17[j]
            for k in range(len(lm17)):
                print("\tPoint ", k, ":")
                score = lm17[k].score
                point = lm17[k].point
                visible = lm17[k].visible
                print("\t\tx:", point.x)
                print("\t\ty:", point.y)
                print("\t\tscore:", score)
                print("\t\tvisible:", visible)

def human_landmarks_redetect_one_example(image1):
    detector = faceEngine.createHumanDetector()

    # Make detection on the first image
    result, human_batch = detector.detect(
        [image1],
        [image1.getRect()],
        10,
        fe.HumanDetectionType(fe.HDT_BOX))
    if not result.isOk:
        print("human_landmarks_redetect_one_example - failed to detect! Reason: {0}".format(result.what))
        return

    detections = human_batch.getDetections(0)
    print("human_landmarks_redetect_one_example - next image results:")
    if len(detections) == 0:
        print("human_landmarks_redetect_one_example - no human on the image!")
        return

    for j in range(len(detections)):
        redetectOneResult, human = detector.redetectOne(image1, detections[j], fe.HumanDetectionType(fe.HDT_BOX | fe.HDT_POINTS))
        if not redetectOneResult.isOk:
            print("human_landmarks_redetect_one_example - failed to redetectOne! Reason: {0}".format(result.what))
            return
        if not human.isValid():
            print("human_landmarks_redetect_one_example - something goes wrong! Human structure is invalid after redetectOne!")
            return

        if not human.landmarks17_opt.isValid():
            print("human_landmarks_redetect_one_example - something goes wrong! HumanLandmarks17 optional is invalid after redetectOne!")
            return

        print(format(human))
        landmarks17 = human.landmarks17_opt.value()
        for k in range(len(landmarks17)):
            print("\tPoint ", k, ":")
            score = landmarks17[k].score
            point = landmarks17[k].point
            visible = landmarks17[k].visible
            print("\t\tx:", point.x)
            print("\t\ty:", point.y)
            print("\t\tscore:", score)
            print("\t\tvisible:", visible)

def human_redetect_one_example(image1, image2):
    detector = faceEngine.createHumanDetector()

    # Make detection on the first image
    result, human_batch = detector.detect(
        [image1],
        [image1.getRect()],
        1,
        fe.HumanDetectionType(fe.HDT_BOX))
    if not result.isOk:
        print("human_redetect_example - failed to detect! Reason: {0}".format(result.what))
        return

    # Get the first human from the results on the first image
    detection = human_batch.getDetections(0)[0]
    if not detection.isValid():
        print("human_redetect_example - something goes wrong! Human structure is invalid after detect!")
        return

    print("human_redetect_example - detect result:\n{0}".format(detection))
    # Make a redetect
    result, human = detector.redetectOne(image2, detection, fe.HumanDetectionType(fe.HDT_BOX))
    if not result.isOk:
        print("human_redetect_example - failed to redetectOne! Reason: {0}".format(result.what))
        return
    if not human.isValid():
        print("human_redetect_example - something goes wrong! Human structure is invalid after redetectOne!")
        return
    print("human_redetect_example - redetectOne result:\n{0}".format(human))


def set_logging(value):
    config = fe.createSettingsProvider("data/faceengine.conf")
    config_path = config.getDefaultPath()
    print("Config settings: DefaultPath {0}".format(config_path))
    config.setValue("system", "verboseLogging", value)
    faceEngine.setSettingsProvider(config)
    val = config.getValue("system", "verboseLogging")
    print("Config settings: \"system\", \"verboseLogging\" = {0}".format(val.asInt()))
    return config


if __name__ == "__main__":
    res = make_activation(faceEngine)
    if res.isError:
        print("Failed to activate license! Reason: {0}".format(res.what))
        exit(-1)
    image_path = sys.argv[2]
    image = fe.Image()
    err_image_loaded = image.load(image_path)
    if not image.isValid():
        print("Image error = ", err_image_loaded)
        exit(-1)
    human_detect_example(image, image)
    human_redetect_one_example(image, image)
    human_landmarks_detect_example(image, image)
    human_landmarks_redetect_one_example(image)
