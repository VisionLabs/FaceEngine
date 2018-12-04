import sys
# import numpy as np
# from matplotlib import pyplot as plt

from example_detector_warper import detector_one_example, warper_example

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
    # only for numpy and matplotlib
    # img_np = image.getData()
    # plt.imshow(img_np)
    # plt.show()
    # print(img_np.shape)
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

    attributeRequest = fe.AttributeRequest(
            fe.AttributeRequest.estimateAge | 
            fe.AttributeRequest.estimateGender | 
            fe.AttributeRequest.estimateEthnicity
        )
    err_attribute, attribute_result = attributeEstimator.estimate(image, attributeRequest)
    err_quality, quality_result = qualityEstimator.estimate(image)
    err_ethnicity, ethnicity_result = ethnicityEstimator.estimate(image)
    err_blackWhite, blackWhite_result = blackWhiteEstimator.estimate(image)
    err_smile, smile_result = smileEstimator.estimate(image)
    if err_attribute.isOk:
        print(attribute_result)
    else:
        print("Failed attribute estimation. Reason: {0}".format(err.what))
        exit(1)
    if err_quality.isOk:
        print(quality_result)
    else:
        print("Failed quality estimation. Reason: {0}".format(err.what))
        exit(1)
    if err_ethnicity.isOk:
        print(ethnicity_result)
        print("Ethnicity score: {0}".format(ethnicity_result.getEthnicityScore(fe.Ethnicity.Caucasian)))
        print("Predominant: {0}".format(ethnicity_result.getPredominantEthnicity()))
    else:
        print("Failed ethnicity estimation. Reason: {0}".format(err.what))
        exit(1)
    if err_blackWhite.isOk:
        print("BlackWhiteEstimation: ", blackWhite_result)
    else:
        print("Failed BlackWhiteEstimation estimation. Reason: {0}".format(err.what))
        exit(1)
    if err_smile.isOk:
        print(smile_result)
    else:
        print("Failed smile estimation. Reason: {0}".format(err.what))
        exit(1)


def headPose_example(_landmarks68):
    headPoseEstimator = faceEngine.createHeadPoseEstimator()
    err, headPoseEstimation = headPoseEstimator.estimate(_landmarks68)
    if err.isOk:
        print(headPoseEstimation)
    return err, headPoseEstimation


def depth_example(depth_image_path):
    depthEstimator = faceEngine.createDepthEstimator()
    # loadImage is used only for depth test
    depth_image = fe.loadImage(depth_image_path)
    err_depth, result = depthEstimator.estimate(depth_image)
    if err.isOk:
        print("Depth estimator value: ", result)
    else:
        print("Failed depth estimation. Reason: {0}".format(err.what))
        exit(1)


def ir_example(ir_image_path):
    iREstimator = faceEngine.createIREstimator()
    ir_image = fe.Image()
    err = ir_image.load(ir_image_path)
    if not ir_image.isValid():
        print("ir image was not found {0}".format(err))
        exit(1)
    err, ir_result = iREstimator.estimate(ir_image)
    if err.isOk:
        print("Ir estimator value: ", ir_result)
    else:
        print("Failed ir estimation. Reason: {0}".format(err.what))
        exit(1)


def faceFlow_example():
    faceFlowEstimator = faceEngine.createFaceFlowEstimator()
    face_flow_image = fe.Image()
    err = face_flow_image.load("testData/small.ppm")
    if not face_flow_image.isValid():
        print("image was not found {0}".format(err.what))
        exit(1)
    sequence = []
    for i in range(10):
        temp_image = fe.Image()
        temp_image.load("testData/" + str(i) + "big.ppm")
        sequence.append(temp_image)
    err, faceFlowResult = faceFlowEstimator.estimate(face_flow_image, sequence)
    if err.isOk:
        print("FaceFlowResult: {0}".format(faceFlowResult))


def eye_example(_warp_image, _transformed_landmarks5):
    eyeEstimator = faceEngine.createEyeEstimator()
    err, eyesEstimation = eyeEstimator.estimate(_warp_image, _transformed_landmarks5)
    if err.isOk:
        print("left eye: ", eyesEstimation.leftEye.state)
        print("right eye: ", eyesEstimation.rightEye.state)
    else:
        print("Failed eye estimation. Reason: {0}".format(err.what))
        exit(1)
    # print_landmarks(eyesEstimation.leftEye.eyelid, "eyesEstimation.leftEye.eyelid")
    # print_landmarks(eyesEstimation.rightEye.eyelid, "eyesEstimation.rightEye.eyelid")
    return err, eyesEstimation


def emotions_example(warp_image):
    emotionsEstimator = faceEngine.createEmotionsEstimator()
    err, emotions_result = emotionsEstimator.estimate(warp_image)
    if err.isOk:
        print(emotions_result)
    else:
        print("Failed emotions estimation. Reason: {0}".format(err.what))
        exit(1)


def gaze_example(_headPoseEstimation, _eyesEstimation):
    gazeEstimator = faceEngine.createGazeEstimator()
    err, gaze_result = gazeEstimator.estimate(_headPoseEstimation, _eyesEstimation)
    if err.isOk:
        print(gaze_result)
    else:
        print("Failed gaze estimation. Reason: {0}".format(err.what))
        exit(1)


def ags_example(_faceEngine, _image, _detection):
    config = fe.createSettingsProvider("data/faceengine.conf")
    # to get ags estimation we need to switch on betamode
    # switch on betaMode
    config.setValue("system", "betaMode", fe.SettingsProviderValue(1))
    # switch on logs to see that betamode is enabled
    config.setValue("system", "verboseLogging", fe.SettingsProviderValue(5))
    _faceEngine.setSettingsProvider(config)
    # create ags estimator only after betamode is enabled
    agsEstimator = faceEngine.createAGSEstimator()
    err, ags_result = agsEstimator.estimate(_image, _detection)
    if err.isOk:
        print("AGS estimator value: {0}".format(ags_result))
    else:
        print("Failed AGS estimation. Reason: {0}".format(err.what))
        exit(1)
    # switch off betaMode if we do not need it yet
    config.setValue("system", "betaMode", fe.SettingsProviderValue(0))
    # switch off logs
    config.setValue("system", "verboseLogging", fe.SettingsProviderValue(0))
    _faceEngine.setSettingsProvider(config)

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
    try:
        # take the simplest example and first detection, see example_detector_warper.py
        err, face = detector_one_example(image)
        if err.isError:
            print("Detector: faces not found.")
            exit(-1)
        (detection, landmarks5, landmarks68) = \
            face.detection, \
            face.landmarks5_opt.value(), \
            face.landmarks68_opt.value()
        # print_landmarks(landmarks5, "landmarks5: ")
        (warp_image, transformed_landmarks5, transformed_landmarks68) = \
            warper_example(image, detection, landmarks5, landmarks68)
        attribute_quality_ethnicity_blackWhite_smile_example(warp_image)
        # examples with hardcoded paths to images
        depth_example("testData/warp.depth")
        ir_example("testData/irWarp.ppm")
        faceFlow_example()

        emotions_example(warp_image)
        err_headPose, headPoseEstimation = headPose_example(landmarks68)
        err_eyes, eyesEstimation = eye_example(warp_image, transformed_landmarks5)
        if err_headPose.isOk and err_eyes.isOk:
            gaze_example(headPoseEstimation, eyesEstimation)
        elif err_headPose.isError:
            print("Failed head pose estimation. Reason: {0}".format(err_headPose.what))
            exit(1)
        elif err_eyes.isOk:
            print("Failed eyes estimation. Reason: {0}".format(err_eyes.what))
            exit(1)
        ags_example(faceEngine, image, detection)
    except Exception as ex:
        print(type(ex).__name__, ex)
        exit(-1)
    finally:
        # do something here
        pass
