import sys

from example_license import make_activation

def help():
    print("python example.py <path to dir with FaceEngine*.so> <path to image>")

if len(sys.argv) != 3:
    help()
    exit(1)


# pass path dir with FaceEngine*.so and add it to system directory
sys.path.append(sys.argv[1])
import FaceEngine as fe


if __name__ == "__main__":
    # Create the main root object
    faceEngine = fe.createFaceEngine("data")
    # Make an activation
    if not make_activation(faceEngine):
        print("failed to activate license!")
        exit(-1)
    # Create the detector object
    detector = faceEngine.createDetector(fe.FACE_DET_V3)
    # Create the warper object
    warper = faceEngine.createWarper()
    # Create the LivenessOneShotRGB object
    liveness_estimator = faceEngine.createLivenessOneShotRGBEstimator()

    # Read the input image
    image_path = sys.argv[2]
    print("image path: {0}".format(image_path))
    image = fe.Image()
    err = image.load(image_path)
    # Just exit in case of error
    if err.isError:
        print("Loading image failed! Reason: {0}".format(err))
        exit(-1)

    # Make detection on the image
    err, face_batch = detector.detect(
        [image],
        [image.getRect()],
        10,
        fe.DetectionType(fe.DT_BBOX | fe.DT_LANDMARKS5)
    )
    # Just exit in case of error
    if err.isError:
        print("Detection failed! Reason: {0}".format(err))
        exit(-1)

    # Only one face on the target image
    if face_batch.getSize() != 1:
        print("Exactly one face required on the image, but found {0} faces!".format(face_batch.getSize()))
        exit(-1)

    detection = face_batch.getDetections(0)[0]
    landmarks5 = face_batch.getLandmarks5(0)[0]

    bestShotQualityEstimator = faceEngine.createBestShotQualityEstimator()
    err, bestShotQualityEstimation = bestShotQualityEstimator.estimate(image, detection, fe.BestShotQualityRequest.estimateHeadPose)
    if err.isError:
        print("Best shot quality estimation failed! Reason: {0}".format(err.what))
        exit(-1)

    headPoseEstimation = bestShotQualityEstimation.headpose_opt.value()

    print("Head pose estimation:", headPoseEstimation)

    config = faceEngine.getSettingsProvider()
    principalAxes = config.getValue("LivenessOneShotRGBEstimator::Settings", "principalAxes")[0]

    if abs(headPoseEstimation.yaw) > principalAxes or abs(headPoseEstimation.pitch) > principalAxes or abs(headPoseEstimation.roll) > principalAxes:
        print("Can't estimate LivenessOneShotRGBEstimation. Yaw, pith or roll absolute value is larger than expected value: ", principalAxes)
        print(headPoseEstimation)
        exit(-1)

    # Make liveness estimation
    err, estimation = liveness_estimator.estimate(image, detection, landmarks5)
    if err.isOk:
        print(estimation)
    else:
        print("Failed LivenessOneShotRGB estimation!. Reason: {0}".format(err.what))

