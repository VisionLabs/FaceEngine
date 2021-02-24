import sys
# import numpy as np
# from matplotlib import pyplot as plt

from example_detector_warper import detector_one_example, warper_example, unwarp_gaze
from example_license import make_activation


def help():
    print("python example.py <path to dir with FaceEngine*.so> <path to image>")


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


def liveness_flying_faces_example(_face):
    flying_faces_estimator = faceEngine.createLivenessFlyingFacesEstimator()
    err, flying_faces_estimation = flying_faces_estimator.estimate(_face.img, _face.detection)
    if err.isOk:
        print(flying_faces_estimation)
    else:
        print("Failed liveness flying faces estimation. Reason: {0}".format(err.what))
        exit(1)


def liveness_flying_faces_batch_example(_images, _detections):
    flying_faces_estimator = faceEngine.createLivenessFlyingFacesEstimator()
    flying_faces_err, flying_faces_estimations = flying_faces_estimator.estimate(_images, _detections)
    if flying_faces_err.isOk:
        for i, _estimation in enumerate(flying_faces_estimations):
            print("{0}) {1}".format(i, _estimation))
    else:
        print("Failed liveness flying faces estimation. Reason: {0}".format(flying_faces_err.what))
        exit(1)


def liveness_fpr_example(_face):
    fpr_estimator = faceEngine.createLivenessFPREstimator()
    error, fpr_estimation = fpr_estimator.estimate(_face.img, _face.detection, _face.landmarks5_opt.value(), False)
    if error.isOk:
        print(fpr_estimation)
    else:
        print("Failed liveness FPR estimation. Reason: {0}".format(err.what))
        exit(1)


def quality_blackWhite_example(image):
    qualityEstimator = faceEngine.createQualityEstimator()
    blackWhiteEstimator = faceEngine.createBlackWhiteEstimator()

    err_subj_quality, subj_quality_result = qualityEstimator.estimate_subjective_quality(image)
    err_quality, quality_result = qualityEstimator.estimate_quality(image)
    err_blackWhite, blackWhite_result = blackWhiteEstimator.estimate(image)

    if err_subj_quality.isOk:
        print(subj_quality_result)
    else:
        print("Failed subjective quality estimation . Reason: {0}".format(err_subj_quality.what))
        exit(1)
    if err_quality.isOk:
        print(quality_result)
    else:
        print("Failed quality estimation. Reason: {0}".format(err_quality.what))
        exit(1)
    if err_blackWhite.isOk:
        print("BlackWhiteEstimation: ", blackWhite_result)
    else:
        print("Failed BlackWhiteEstimation estimation. Reason: {0}".format(err_blackWhite.what))
        exit(1)



def attribute_example(_warp):
    attribute_estimator = faceEngine.createAttributeEstimator()
    attribute_request = fe.AttributeRequest(
        fe.AttributeRequest.estimateAge |
        fe.AttributeRequest.estimateGender |
        fe.AttributeRequest.estimateEthnicity
    )
    err_attribute, result = attribute_estimator.estimate(_warp, attribute_request)
    if err_attribute.isOk:
        print(result)
        return err_attribute, result
    else:
        print("Failed attribute estimation. Reason: {0}".format(err_attribute.what))
        exit(1)


def attribute_batch_example(_warps):
    attribute_estimator = faceEngine.createAttributeEstimator()
    attribute_request = fe.AttributeRequest(
        fe.AttributeRequest.estimateAge |
        fe.AttributeRequest.estimateGender |
        fe.AttributeRequest.estimateEthnicity
    )
    err_attribute, each_warp_result, aggregate_result = attribute_estimator.estimate(_warps, attribute_request)
    if err_attribute.isOk:
        return err_attribute, each_warp_result, aggregate_result
    else:
        print("Failed batch attribute estimation. Reason: {0}".format(err_attribute.what))
        exit(1)


def medical_mask_warped_example(_warp):
    estimator = faceEngine.createMedicalMaskEstimator()
    err, estimation = estimator.estimate(_warp)
    if err.isOk:
        print("\nmedical_mask_warped_example:\n")
        print("Result: {}".format(estimation.result))
        print("Scores: \n\tmask: {}\n\tnoMask: {}\n\toccludedFace: {}\n".format(
            estimation.maskScore,
            estimation.noMaskScore,
            estimation.occludedFaceScore
        ))
    else:
        print("Failed medical mask estimation. Reason: {0}".format(err.what))
        exit(1)

def medical_mask_extended_warped_example(_warp):
    estimator = faceEngine.createMedicalMaskEstimator()
    err, estimation = estimator.estimate_extended(_warp)
    if err.isOk:
        print("\nmedical_mask_extended_warped_example:\n")
        print("Result: {}".format(estimation.result))
        print("Scores: \n\tmask: {}\n\tnoMask: {}\n\tmaskNotInPlace: {}\n\toccludedFace: {}\n".format(
            estimation.maskScore,
            estimation.noMaskScore,
            estimation.maskNotInPlace,
            estimation.occludedFaceScore
        ))
    else:
        print("Failed medical mask estimation. Reason: {0}".format(err.what))
        exit(1)


def medical_mask_cropped_example(_image, _detection):
    estimator = faceEngine.createMedicalMaskEstimator()
    err, estimation = estimator.estimate(_image, _detection)
    if err.isOk:
        print("\nmedical_mask_cropped_example:\n")
        print("Result: {}".format(estimation.result))
        print("Scores: \n\tmask: {}\n\tnoMask: {}\n\toccludedFace: {}\n".format(
            estimation.maskScore,
            estimation.noMaskScore,
            estimation.occludedFaceScore
        ))
    else:
        print("Failed medical mask estimation. Reason: {0}".format(err.what))
        exit(1)

def medical_mask_extended_сropped_example(_image, _detection):
    estimator = faceEngine.createMedicalMaskEstimator()
    err, estimation = estimator.estimate_extended(_image, _detection)
    if err.isOk:
        print("\nmedical_mask_extended_cropped_example:\n")
        print("Result: {}".format(estimation.result))
        print("Scores: \n\tmask: {}\n\tnoMask: {}\n\tmaskNotInPlace: {}\n\toccludedFace: {}\n".format(
            estimation.maskScore,
            estimation.noMaskScore,
            estimation.maskNotInPlace,
            estimation.occludedFaceScore
        ))
    else:
        print("Failed medical mask estimation. Reason: {0}".format(err.what))
        exit(1)

def medical_mask_warped_batch_example(_warps):
    estimator = faceEngine.createMedicalMaskEstimator()

    err, estimations = estimator.estimate(_warps)
    print("\nmedical_mask_warped_batch_example: ")
    if err.isOk:
        for i, est in enumerate(estimations):
            print("number: ", i)
            print("Result: {}".format(est.result))
            print("Scores: \n\tmask: {}\n\tnoMask: {}\n\toccludedFace: {}\n".format(
                est.maskScore,
                est.noMaskScore,
                est.occludedFaceScore
            ))
    else:
        print("Failed medical mask estimation. Reason: {0}".format(err.what))
        exit(1)


def medical_mask_cropped_batch_example(_images, _detections):
    estimator = faceEngine.createMedicalMaskEstimator()
    print("\nmedical_mask_cropped_batch_example: ")
    err, estimations = estimator.estimate(_images, _detections)
    if err.isOk:
        for i, est in enumerate(estimations):
            print("number: ", i)
            print("Result: {}".format(est.result))
            print("Scores: \n\tmask: {}\n\tnoMask: {}\n\toccludedFace: {}\n".format(
                est.maskScore,
                est.noMaskScore,
                est.occludedFaceScore
            ))
    else:
        print("Failed medical mask estimation. Reason: {0}".format(err.what))
        exit(1)

def mouth_example(_warp):
    mouthEstimator = faceEngine.createMouthEstimator()
    err, mouthEstimation = mouthEstimator.estimate(_warp)
    if err.isOk:
        print(mouthEstimation)
    else:
        print("Failed to estimate mouth state! Reason: {}".format(err.what()))
    return


def depth_example(depth_image_path):
    depthEstimator = faceEngine.createDepthEstimator()
    # loadImage is used only for depth test
    depth_image = fe.loadImage(depth_image_path)
    err_depth, result = depthEstimator.estimate(depth_image)
    if err_depth.isOk:
        print("Depth estimator value: ", result)
    else:
        print("Failed depth estimation. Reason: {0}".format(err_depth.what))
        exit(1)


def ir_example(ir_image_path):
    iREstimator = faceEngine.createIREstimator()
    ir_image = fe.Image()
    err = ir_image.load(ir_image_path)
    if not ir_image.isValid():
        print("IR image load error: {0}".format(err.what))
        exit(1)
    err, ir_result = iREstimator.estimate(ir_image)
    if err.isOk:
        print("IR estimator value: ", ir_result)
    else:
        print("Failed ir estimation. Reason: {0}".format(err.what))
        exit(1)


def faceFlow_example():
    faceFlowEstimator = faceEngine.createFaceFlowEstimator()
    face_flow_image = fe.Image()
    err = face_flow_image.load("images/small.ppm")
    if not face_flow_image.isValid():
        print("image was not found {0}".format(err.what))
        exit(1)
    sequence = []
    for i in range(10):
        temp_image = fe.Image()
        temp_image.load("images/" + str(i) + "big.ppm")
        sequence.append(temp_image)
    err, faceFlowResult = faceFlowEstimator.estimate(face_flow_image, sequence)
    if err.isOk:
        print("FaceFlowResult: {0}".format(faceFlowResult))
    else:
        print("Failed faceFlowEstimation. Reason: {0}".format(err.what))


def eye_example(_warp_image, _transformed_landmarks5):
    eyeEstimator = faceEngine.createEyeEstimator()
    cropper = fe.EyeCropper()
    eyeRectsByLandmarks5 = cropper.cropByLandmarks5(_warp_image, _transformed_landmarks5)
    err_eyes, eyesEstimation = eyeEstimator.estimate(_warp_image, eyeRectsByLandmarks5)
    if err.isOk:
        print("left eye: ", eyesEstimation.leftEye.state)
        print("right eye: ", eyesEstimation.rightEye.state)
    else:
        print("Failed eye estimation. Reason: {0}".format(err.what))
        exit(1)
    # print_landmarks(eyesEstimation.leftEye.eyelid, "eyesEstimation.leftEye.eyelid")
    # print_landmarks(eyesEstimation.rightEye.eyelid, "eyesEstimation.rightEye.eyelid")
    return err, eyesEstimation


# to get eye estimation in a space of origin coordinates
def unwarp_eye_estimation(warper, transformation, eye_estimation_warped):
    err_code, eye_estimation_unwarped = warper.unwarp(eye_estimation_warped, transformation)
    return err_code, eye_estimation_unwarped


def draw_eye_points(image_cv, detection, eye_list1, eye_list2, color):
    import cv2
    detx = detection.getRect().x
    dety = detection.getRect().y
    for left, right in zip(eye_list1, eye_list2):
        x1 = int(detx + left.x)
        y1 = int(dety + left.y)
        x2 = int(detx + right.x)
        y2 = int(dety + right.y)
        cv2.circle(image_cv, (x1, y1), 1, color, 2)
        cv2.circle(image_cv, (x2, y2), 1, color, 2)

    return image_cv


def save_origin_image_with_landmarks(detection, eyes_unwarped, path):
    # get numpy aray
    image_cv = image.getData()
    image_cv = draw_eye_points(image_cv, detection, eyes_unwarped.leftEye.eyelid, eyes_unwarped.rightEye.eyelid, (0, 255, 0))
    image_cv = draw_eye_points(image_cv, detection, eyes_unwarped.leftEye.iris, eyes_unwarped.rightEye.iris, (255, 0, 0))
    # convert image to opencv
    image_saved = fe.Image()
    err_set_data = image_saved.setData(image_cv, fe.FormatType.R8G8B8)
    if err_set_data.isError:
        print("Image was not converted, reason: ", err_set_data.what)
        return
    err_save = image_saved.save(path)
    if err_save.isError:
        print("Image was not converted, reason: ", err_save.what)
        return
    else:
        print("Saved image: ", path)


def emotions_example(warp_image):
    emotionsEstimator = faceEngine.createEmotionsEstimator()
    err, emotions_result = emotionsEstimator.estimate(warp_image)
    if err.isOk:
        print(emotions_result)
    else:
        print("Failed emotions estimation. Reason: {0}".format(err.what))
        exit(1)


def gaze_example_infrared(_warp_image, _transformed_landmarks5):
    gaze_estimator = faceEngine.createGazeEstimator(fe.SensorType.NIR)
    err, gaze_result = gaze_estimator.estimate(_warp_image, transformed_landmarks5)
    if err.isOk:
        return err, gaze_result
    else:
        print("Failed gaze estimation. Reason: {0}".format(err.what))
        exit(1)


def gaze_example_rgb(_warp_image, _transformed_landmarks5):
    gaze_estimator_rgb = faceEngine.createGazeEstimator()
    err, gaze_result = gaze_estimator_rgb.estimate(_warp_image, _transformed_landmarks5)
    point = gaze_estimator_rgb.getFaceCenter(_transformed_landmarks5)
    print("Point for calculating of gaze projection: ", point)
    if err.isOk:
        return err, gaze_result
    else:
        print("Failed ir gaze estimation. Reason: {0}".format(err.what))
        exit(1)


def glasses_example(_faceEngine, _warp):
    glasses_estimator = _faceEngine.createGlassesEstimator()
    err, glasses_estimation = glasses_estimator.estimate(_warp)
    if err.isOk:
        print(glasses_estimation)
    else:
        print("Failed glasses estimation. Reason: {0}".format(err.what))
        exit(1)


def overlap_example(_face_engine, _image, _detection):
    overlap_estimator = _face_engine.createOverlapEstimator()
    err_overlap, overlap_estimation = overlap_estimator.estimate(_image, _detection)
    if err_overlap.isOk:
        print(overlap_estimation)
    else:
        print("Failed overlap estimation. Reason: {0}".format(err.what))
        exit(1)

def orientation_example(_face_engine, _image):
    estimator = _face_engine.createOrientationEstimator()

    # basic example with one image
    err, orientation = estimator.estimate(_image)
    if err.isOk:
        print("Orientation result: {0}".format(orientation))
    else:
        print("Failed orientation estimation. Reason: {0}".format(err.what))
        exit(1)

    # batch example
    err, result_list = estimator.estimate([_image, _image, _image])
    if err.isOk:
        print("Batched orientation results: {0}".format(result_list))
    else:
        print("Failed orientation estimation. Reason: {0}".format(err.what))
        exit(1)


def best_shot_quality_estimator_example(_face_engine, _image, _detection):
    estimator = _face_engine.createBestShotQualityEstimator()
    error, estimation = estimator.estimate(_image, _detection, fe.BestShotQualityRequest.estimateAll)
    if error.isOk:
        print(estimation)
    else:
        print("Failed best shot quality estimation. Reason : ", error.what)
        exit(1)


def best_shot_quality_estimator_batch_example(_face_engine, _images, _detections):
    estimator = _face_engine.createBestShotQualityEstimator()
    error, estimations = estimator.estimate(_images, _detections, fe.BestShotQualityRequest.estimateAll)
    if error.isError:
        print("Failed best shot quality estimation. Reason : ", error.what)
        exit(1)

    for i, estimation in enumerate(estimations):
        print("number: ", i, estimation)


def print_landmarks(landmarks, message=""):
    print(message)
    for i in range(len(landmarks)):
        print(landmarks[i])


def get_info():
    print(fe.getVersionHash())
    print(fe.getVersionString())
    print(fe.getBuildInfo())


if __name__ == "__main__":
    # correct path or put directory "data" with example.py
    faceEngine = fe.createFaceEngine("data")
    get_info()
    res = make_activation(faceEngine)
    if res.isError:
        print("Failed to activate license! Reason: {0}".format(res.what))
        exit(-1)
    image_path = sys.argv[2]
    image = image_load(image_path)
    try:
        # Orientation example is the first because other examples could not work 
        # with images with not normal orientation
        orientation_example(faceEngine, image)

        # take the simplest example and first detection, see example_detector_warper.py
        err, face = detector_one_example(image)
        if err.isError or not face.isValid():
            print("Detector: faces are not found.")
            exit(-1)
        if not face.landmarks5_opt.isValid() or not face.landmarks68_opt.isValid():
            print("Detector: landmarks are not valid.")
            exit(-1)
        (detection, landmarks5, landmarks68) = \
            face.detection, \
            face.landmarks5_opt.value(), \
            face.landmarks68_opt.value()
        # print_landmarks(landmarks5, "landmarks5: ")
        (warp_image, transformed_landmarks5, transformed_landmarks68, transformation, warper) = \
            warper_example(image, detection, landmarks5, landmarks68)
        quality_blackWhite_example(warp_image)
        glasses_example(faceEngine, warp_image)
        overlap_example(faceEngine, image, detection)
        err, attribute_result = attribute_example(warp_image)
        print(attribute_result)
        # for example list consists two the same images
        err_attribute_batch, attribute_list_result, aggregate_result = attribute_batch_example([warp_image, warp_image])
        print("aggregate attribute result: ", aggregate_result)
        emotions_example(warp_image)
        mouth_example(warp_image)
        err_eyes, eyesEstimation = eye_example(warp_image, transformed_landmarks5)
        # to get eye estimation in origin coords
        err_eye_unwarped, eyes_unwarped = unwarp_eye_estimation(warper, transformation, eyesEstimation)
        # if err_eye_unwarped.isOk:
            # will be saved in working directory, be sure you have numpy and cv2
            # save_origin_image_with_landmarks(detection, eyes_unwarped, "iris_eyelid_image.png")
        err_gaze, gaze_result = gaze_example_rgb(warp_image, transformed_landmarks5)
        if err_gaze.isOk:
            gaze = unwarp_gaze(gaze_result, transformation)
            print(gaze)
        elif err_eyes.isError:
            print("Failed eyes estimation. Reason: {0}".format(err_eyes.what))
        liveness_flying_faces_example(face)
        liveness_flying_faces_batch_example([face.img, face.img], [face.detection, face.detection])
        liveness_fpr_example(face)
        medical_mask_warped_example(warp_image)
        medical_mask_extended_warped_example(warp_image)
        medical_mask_cropped_example(image, detection)
        medical_mask_extended_сropped_example(image, detection)
        medical_mask_warped_batch_example([warp_image, warp_image])
        medical_mask_cropped_batch_example([image, image], [detection, detection])
        best_shot_quality_estimator_example(faceEngine, image, detection)
        best_shot_quality_estimator_batch_example(faceEngine, [image, image], [detection, detection])
        # examples with hardcoded paths to images, special needs
        # depth_example("images/warp.depth")
        # ir_example("images/irWarp.ppm")
        # faceFlow_example()
    except Exception as ex:
        print(type(ex).__name__, ex)
        exit(-1)
    finally:
        # do something here
        pass

