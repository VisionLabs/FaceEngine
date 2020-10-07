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
if not make_activation(faceEngine):
    print("failed to activate license!")
    exit(-1)


def human_detect_example(image1, image2):
    detector = faceEngine.createHumanDetector()
    result = detector.detect(
        [image1, image2],
        [image1.getRect(), image2.getRect()],
        10,
        fe.HumanDetectionType(fe.DCT_BOX))

    if not result[0].isOk:
        print("human_detect_example - failed to detect! Reason: {0}".format(result[0].what))
        return

    for human_list in result[1]:
        print("human_detect_example - next image results:")
        if len(human_list) == 0:
            print("human_detect_example - no human on the image!")
        else:
            for human in human_list:
                print(human)


def human_landmarks_detect_example(image1, image2):
    detector = faceEngine.createHumanDetector()
    result = detector.detect(
        [image1, image2],
        [image1.getRect(), image2.getRect()],
        10,
        fe.HumanDetectionType(fe.DCT_BOX | fe.DCT_POINTS))
    if not result[0].isOk:
        print("human_landmarks_detect_example - failed to detect! Reason: {0}".format(result[0].what))
        return

    for human_list in result[1]:
        print("human_landmarks_detect_example - next image results:")
        if len(human_list) == 0:
            print("human_landmarks_detect_example - no human on the image!")
        else:
            for human in human_list:
                print(human)
                if human.landmarks17_opt.isValid() :
                    landmarks17 = human.landmarks17_opt.value()
                    for i in range(len(landmarks17)):
                        print("\tPoint ", i, ":")
                        score = landmarks17[i].score
                        point = landmarks17[i].point
                        print("\t\tx:", point.x)
                        print("\t\ty:", point.y)
                        print("\t\tscore:", score)
                else:
                    print("human_landmarks_detect_example - landmarks failed!")


def human_redetect_one_example(image1, image2):
    detector = faceEngine.createHumanDetector()

    # Make detection on the first image
    result = detector.detect(
        [image1],
        [image1.getRect()],
        1,
        fe.HumanDetectionType(fe.DCT_BOX))
    if not result[0].isOk:
        print("human_redetect_example - failed to detect! Reason: {0}".format(result.what))
        return

    # Get human list result for the first image
    human_list = result[1][0]
    if len(human_list) == 0:
        print("human_redetect_example - no any human on the image!")
        return
    # Get the first human from the results on the first image
    human = human_list[0]
    if not human.isValid():
        print("human_redetect_example - something goes wrong! Human structure is invalid after detect!")
        return

    print("human_redetect_example - detect result:\n{0}".format(human))
    # Set for the Human structure new image
    human.img = image2
    # And make a redetect
    result = detector.redetectOne(human)
    if not result[0].isOk:
        print("human_redetect_example - failed to redetectOne! Reason: {0}".format(result[0].what))
        return
    if not result[0].value:
        print("human_redetect_example - no human was found during redetectOne!")
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
    if not make_activation(faceEngine):
        print("failed to activate license!")
        exit(-1)
    image_path = sys.argv[2]
    image = fe.Image()
    err_image_loaded = image.load(image_path, fe.FormatType.R8G8B8)
    if not image.isValid():
        print("Image error = ", err_image_loaded)
        exit(-1)
    human_detect_example(image, image)
    human_redetect_one_example(image, image)
    human_landmarks_detect_example(image, image)