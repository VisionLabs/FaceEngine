import sys
# import numpy as np
from example_detector_warper import detector_example, warper_example

def help():
    print("python example.py <path to FaceEngine*.so> <path to image>")


if len(sys.argv) != 3:
    help()
    exit(1)

# pass path dir with FaceEngine*.so and add it to system directory
sys.path.append(sys.argv[1])
import FaceEngine as fe

# image loading example
def image_load(image_path):
    print("image path: {0}".format(image_path))
    image = fe.Image()
    # print(fe.FormatType.R8)
    err = image.load(image_path)
    # image.load(image_path, fe.Format(fe.FormatType.R8G8B8))
    if not image.isValid():
        print("Image error = ", err)
    return image


def attribute_quality_ethnicity_blackWhite_smile_example(image):
    attributeEstimator = faceEngine.createAttributeEstimator()
    qualityEstimator = faceEngine.createQualityEstimator()
    ethnicityEstimator = faceEngine.createEthnicityEstimator()
    blackWhiteEstimator = faceEngine.createBlackWhiteEstimator()
    smileEstimator = faceEngine.createSmileEstimator()
    attribute_result = attributeEstimator.estimate(image)
    quality_result = qualityEstimator.estimate(image)
    ethnicity_result = ethnicityEstimator.estimate(image)
    blackWhite_result = blackWhiteEstimator.estimate(image)
    smile_result = smileEstimator.estimate(image)
    print(attribute_result)
    print(quality_result)
    print(ethnicity_result)
    print("Ethnicity score: {0}".format(ethnicity_result.getEthnicityScore(fe.Ethnicity.Caucasian)))
    print("Predominant: {0}".format(ethnicity_result.getPredominantEthnicity()))
    print("BlackWhiteEstimation: ", blackWhite_result)
    print(smile_result)


def headPose_example(_landmarks68):
    headPoseEstimator = faceEngine.createHeadPoseEstimator()
    headPoseEstimation = headPoseEstimator.estimate(_landmarks68)
    print(headPoseEstimation)
    return headPoseEstimation


def depth_example(depth_image_path):
    depthEstimator = faceEngine.createDepthEstimator()
    # loadImage is used only for depth test
    depth_image = fe.loadImage(depth_image_path)
    print("Depth estimator value: ", depthEstimator.estimate(depth_image).value)


def ir_example(ir_image_path):
    iREstimator = faceEngine.createIREstimator()
    ir_image = fe.Image()
    err = ir_image.load(ir_image_path)
    if not ir_image.isValid():
        print("ir image was not found {0}".format(err))
        exit(1)
    ir_result = iREstimator.estimate(ir_image)
    print("Ir estimator value: ", ir_result.value)


def faceFlow_example():
    faceFlowEstimator = faceEngine.createFaceFlowEstimator()
    face_flow_image = fe.Image()
    face_flow_image.load("testData/small.ppm")
    if not face_flow_image.isValid():
        print("image was not found {0}".format(err))
        exit(1)
    sequence = []
    for i in range(10):
        temp_image = fe.Image()
        temp_image.load("testData/" + str(i) + "big.ppm")
        sequence.append(temp_image)
    faceFlowResult = faceFlowEstimator.estimate(face_flow_image, sequence)
    print("FaceFlowResult: {0}".format(faceFlowResult))


def eye_example(_warp_image, _transformed_landmarks5):
    eyeEstimator = faceEngine.createEyeEstimator()
    eyesEstimation = eyeEstimator.estimate(_warp_image, _transformed_landmarks5)
    print("left eye: ", eyesEstimation.leftEye.state)
    print("right eye: ", eyesEstimation.rightEye.state)
    # print_landmarks(eyesEstimation.leftEye.eyelid, "eyesEstimation.leftEye.eyelid")
    # print_landmarks(eyesEstimation.rightEye.eyelid, "eyesEstimation.rightEye.eyelid")
    return eyesEstimation


def emotions_example(warp_image):
    emotionsEstimator = faceEngine.createEmotionsEstimator()
    emotions_result = emotionsEstimator.estimate(warp_image)
    print(emotions_result)


def gaze_example(_headPoseEstimation, _eyesEstimation):
    gazeEstimator = faceEngine.createGazeEstimator()
    gaze_result = gazeEstimator.estimate(_headPoseEstimation, _eyesEstimation)
    print(gaze_result)


def are_equal(desc1, desc2):
    assert(len(desc1) == len(desc2))
    for i, _ in enumerate(desc1):
        if desc1[i] != desc2[i]:
            return False
    return True





def print_landmarks(landmarks, message=""):
    print(message)
    for i in range(len(landmarks)):
        print(landmarks[i])


if __name__ == "__main__":
    # correct path or put directory "data" with example.py
    faceEngine = fe.createFaceEngine("data",
                                     "data/faceengine.conf")
    image_path = sys.argv[2]
    image = image_load(image_path)
    # only with numpy
    # data_np = image.getData()
    # print(data_np.shape)
    attribute_quality_ethnicity_blackWhite_smile_example(image)
    # unpack detector result - list of tuples
    (detection, landmarks5, landmarks68) = detector_example(image, 1)[0]
    # print_landmarks(landmarks5, "landmarks5: ")
    # print_landmarks(landmarks68, "landmarks68: ")
    (warp_image, transformed_landmarks5, transformed_landmarks68) = \
        warper_example(image, detection, landmarks5, landmarks68)

    # examples with hardcoded paths to images
    depth_example("testData/warp.depth")
    ir_example("testData/irWarp.ppm")
    faceFlow_example()

    emotions_example(warp_image)
    headPoseEstimation = headPose_example(landmarks68)
    eyesEstimation = eye_example(warp_image, transformed_landmarks5)
    gaze_example(headPoseEstimation, eyesEstimation)
