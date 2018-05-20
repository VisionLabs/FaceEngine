import sys


def help():
    print("python example.py <path to FaceEngine*.so> <path to image>")


if len(sys.argv) != 3:
    help()
    exit(1)

# pass path dir with FaceEngine*.so and add it to system directory
sys.path.append(sys.argv[1])
import FaceEngine as fe

# correct paths to data or put directories data and images with example.py
faceEngine = fe.createFaceEngine("data",
                                      "data/faceengine.conf")

# image loading example
def image_load(image_path):
    print("\nImage example")
    print("image path: {0}".format(image_path))
    image = fe.Image()
    print(fe.FormatType.R8)
    print(fe.Format())
    err = image.load(image_path)
    # image.load(image_path, fe.Format(fe.FormatType.R8G8B8))
    print("Image error = ", err)
    print("image was downloaded {0}".format(err.isOk))
    print("image width {0}".format(image.getWidth()))
    print("image height {0}".format(image.getHeight()))
    print("image is valid {0}".format(image.isValid()))
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
    print("Attribute estimation result {0}".format(attribute_result))
    print("Quality estimation result {0}".format(quality_result))
    print("Ethnicity estimation result {0}".format(ethnicity_result))
    print("getEthnicityScore() {0}".format(ethnicity_result.getEthnicityScore(fe.Ethnicity.Caucasian)))
    print("getPredominantEthnicity() {0}".format(ethnicity_result.getPredominantEthnicity()))
    print("BlackWhite estimation result {0}".format(blackWhite_result))
    print("Smile estimation result {0}".format(smile_result))


def detector_example(image_path):
    print("\nDetector example")
    detector = faceEngine.createDetector(fe.ODT_MTCNN)
    max_detections = 3
    image_det = fe.Image()
    err = image_det.load(image_path)
    if not image_det.isValid():
        print(err)
    print("Image for detection: ", image_path, image_det.getHeight(), image_det.getWidth(), image_det.isValid())
    detector_result = detector.detect(image_det, image_det.getRect(), max_detections)
    print("detector result = ", detector_result)
    test = detector_result[0][1]
    print("Landmarks test {0}".format(test[0]))
    print("Landmarks test {0}".format(test[0]))
    print("Detections: ")
    for i, item in enumerate(detector_result, 1):
        print(i, item)
    return detector_result


def warper_example(image_det, detection, landmarks5, landmarks68):
    print("\nWarper example: ")
    warper = faceEngine.createWarper()
    transformation = warper.createTransformation(detection,
                                                 landmarks5)
    print("transformation = ", transformation)
    warper_result = warper.warp(image_det, transformation)
    warp_image = warper_result
    print(warp_image)
    print(warp_image.getWidth(), warp_image.getHeight(), warp_image.isValid())
    ethnicityEstimator = faceEngine.createEthnicityEstimator()
    estimation = ethnicityEstimator.estimate(warp_image)
    print("Ethnicity estimator on warped Image {0}".format(estimation))
    transformed_landmarks5 = warper.warp(landmarks5, transformation)
    print("\nTransformed landmarks5: ")
    for i in range(len(transformed_landmarks5)):
        print(transformed_landmarks5[i])
    transformed_landmarks68 = warper.warp(landmarks68, transformation)
    print("warperResult with Landmarks5 = ", transformed_landmarks5)
    print("warperResult with Landmarks68 = ", transformed_landmarks68)
    return warp_image, transformed_landmarks5, transformed_landmarks68


def headPose_example(_landmarks68):
    headPoseEstimator = faceEngine.createHeadPoseEstimator()
    headPoseEstimation = headPoseEstimator.estimate(_landmarks68)
    print("HeadPose result {0}".format(headPoseEstimation))
    return headPoseEstimation


def depth_example(depth_image_path):
    depthEstimator = faceEngine.createDepthEstimator()
    # loadImage is used only for depth test
    depth_image = fe.loadImage(depth_image_path)
    print("Depth estimation result = {0}".format(depthEstimator.estimate(depth_image)))


def ir_example(ir_image_path):
    iREstimator = faceEngine.createIREstimator()
    ir_image = fe.Image()
    err = ir_image.load(ir_image_path)
    if not ir_image.isValid():
        print("ir image was not found {0}".format(err))
        exit(1)
    ir_result = iREstimator.estimate(ir_image)
    print("IR estimation result = {0}".format(ir_result))


def faceFlow_example():
    faceFlowEstimator = faceEngine.createFaceFlowEstimator()
    face_flow_image = fe.Image()
    face_flow_image.load("images/small.ppm")
    if not face_flow_image.isValid():
        print("ir image was not found {0}".format(err))
        exit(1)
    sequence = []
    for i in range(10):
        temp_image = fe.Image()
        temp_image.load("images/" + str(i) + "big.ppm")
        sequence.append(temp_image)
    faceFlowResult = faceFlowEstimator.estimate(face_flow_image, sequence, len(sequence))
    print("FaceFlowResult {0}".format(faceFlowResult))


def eye_example(_warp_image, _transformed_landmarks5):
    eyeEstimator = faceEngine.createEyeEstimator()
    eyesEstimation = eyeEstimator.estimate(_warp_image, _transformed_landmarks5)
    print("\nEyes estimation result: ")
    print(eyesEstimation.leftEye.state, eyesEstimation.leftEye.iris, eyesEstimation.leftEye.eyelid)
    print(eyesEstimation.rightEye.state, eyesEstimation.rightEye.iris, eyesEstimation.rightEye.eyelid)
    print_landmarks(eyesEstimation.leftEye.eyelid, "eyesEstimation.leftEye.eyelid")
    print_landmarks(eyesEstimation.rightEye.eyelid, "eyesEstimation.rightEye.eyelid")
    return eyesEstimation


def emotions_example(warp_image):
    emotionsEstimator = faceEngine.createEmotionsEstimator()
    emotions_result = emotionsEstimator.estimate(warp_image)
    print("Emotions estimation result {0}".format(emotions_result))


def gaze_example(_headPoseEstimation, _eyesEstimation):
    gazeEstimator = faceEngine.createGazeEstimator()
    print(gazeEstimator)
    gaze_result = gazeEstimator.estimate(_headPoseEstimation, _eyesEstimation)
    print("\nGaze estimation result {0}".format(gaze_result))


def are_equal(desc1, desc2):
    assert(len(desc1) == len(desc2))
    for i, _ in enumerate(desc1):
        if desc1[i] != desc2[i]:
            return False
    return True


def extractor_test_aggregation(version, use_mobile_net, cpu_type, device):
    print("Extractor_test_aggregation")
    config = fe.createSettingsProvider("data/faceengine.conf")
    configPath = config.getDefaultPath()
    print(configPath)
    config.setValue("DescriptorFactory::Settings", "model", fe.SettingsProviderValue(version))
    config.setValue("DescriptorFactory::Settings", "useMobileNet", fe.SettingsProviderValue(use_mobile_net))
    config.setValue("flower", "deviceClass", fe.SettingsProviderValue(device))
    config.setValue("system", "cpuClass", fe.SettingsProviderValue(cpu_type))
    config.setValue("system", "verboseLogging", fe.SettingsProviderValue(5))
    # config.setValue("QualityEstimator::Settings", "logGray", f.SettingsProviderValue(0.05, 3.3, 0.05, 0.012))
    faceEngine.setSettingsProvider(config)
    # try to test settingsProvider
    print("SettingProvider")
    val = config.getValue("QualityEstimator::Settings", "platt")
    print(val.asPoint2f())
    val = config.getValue("QualityEstimator::Settings", "expBlur")
    print("Try to print value in different formats: ")
    print(val.asPoint2f())
    print(val.asPoint2i())
    print(val.asSize())
    print(val.asRect())
    print(val.asBool())
    print(val.asInt())
    print(val.asFloat())
    print(val.asString())

    faceEngine.setSettingsProvider(config)
    val = config.getValue("MTCNNDetector::Settings", "scaleFactor")
    print(val.asFloat())

    warps = [fe.Image(), fe.Image()]

    warps[0].load("images/warp1.ppm")
    warps[1].load("images/warp2.ppm")
    batchSize = len(warps)
    descriptorExtractor = faceEngine.createExtractor()
    batch = faceEngine.createDescriptorBatch(batchSize)
    descriptor = faceEngine.createDescriptor()
    descriptor2 = faceEngine.createDescriptor()
    aggregation = faceEngine.createDescriptor()

    result1 = descriptorExtractor.extractFromWarpedImageBatch(warps, batch, aggregation, batchSize)
    print(result1)
    print("aggregation: ", aggregation.getModelVersion(), aggregation.getDescriptorLength())
    result2 = descriptorExtractor.extractFromWarpedImage(warps[0], descriptor)
    result3 = descriptorExtractor.extractFromWarpedImage(warps[1], descriptor2)
    # # test of error
    # print("Image is valid = {0}".format(warps[2].isValid()))
    # result4 = descriptorExtractor.extractFromWarpedImage(warps[2], descriptor2)
    # print("test of return error", result4)
    print(result2)
    print(result3)
    print(descriptor.getModelVersion() == batch.getModelVersion())
    print(batch.getMaxCount())
    print(batch.getCount())
    print(batch.getDescriptorSize())
    desc1 = descriptor.getDescriptor()
    desc2 = descriptor2.getDescriptor()
    print("Descriptor")
    desc_from_batch = batch.getDescriptorSlow(1).getDescriptor()
    for i, element in enumerate(desc1):
        print(i, ")", desc1[i], desc_from_batch[i])
    print("Descritptor comparing {0}".format(are_equal(desc1, desc_from_batch)))
    print("are_equal(desc1, desc2){0}".format(are_equal(desc1, desc2)))

def print_landmarks(landmarks, message=""):
    print(message)
    for i in range(len(landmarks)):
        print(landmarks[i])


if __name__ == "__main__":
    image_path = sys.argv[2]
    image = image_load(image_path)
    attribute_quality_ethnicity_blackWhite_smile_example(image)
    # unpack detector result - list of tuples
    (detection, landmarks5, landmarks68) = detector_example(image_path)[0]
    print_landmarks(landmarks5, "landmarks5: ")
    # print_landmarks(landmarks68, "landmarks68: ")
    (warp_image, transformed_landmarks5, transformed_landmarks68) = \
        warper_example(image, detection, landmarks5, landmarks68)
    print("transformedLandmarks5[0]", transformed_landmarks5)

    # examples with hardcoded images
    depth_example("images/warp.depth")
    ir_example("images/irWarp.ppm")
    faceFlow_example()

    emotions_example(warp_image)
    headPoseEstimation = headPose_example(landmarks68)
    eyesEstimation = eye_example(warp_image, transformed_landmarks5)
    gaze_example(headPoseEstimation, eyesEstimation)
    extractor_test_aggregation(46, True, "cpu", "cpu")
