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
    # Create the LivenessOneShotRGB object
    liveness_estimator = faceEngine.createLivenessOneShotRGBEstimator()

    bestShotQualityEstimator = faceEngine.createBestShotQualityEstimator()

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
        print("Warning: On image found more than one face. And this breaking "
                "`Liveness OneShotRGB estimator` requirements. Results may be incorrect."
                " Only the first detection will be handled. Found {0} faces!".format(face_batch.getSize()))

    detection = face_batch.getDetections(0)[0]
    landmarks5 = face_batch.getLandmarks5(0)[0]

    if not detection.isValid():
        print("example_liveness_one_shot - Detection error!")
        exit(-1)

    detectionRect = detection.getRect()
    print("example_liveness_one_shot - detect result:\n{0}".format(detection))

    # Minimum detection size in pixels.
    minDetSize = 200;

    # Step back from the borders.
    borderDistance = 5;

    if min(detectionRect.width, detectionRect.height) < minDetSize:
        print("Bounding Box width and/or height is less than `minDetSize` - ", minDetSize)
        exit(-1)

    if (detectionRect.x + detectionRect.width) > (image.getWidth() - borderDistance) or (detectionRect.x < borderDistance) :
        print("Bounding Box width is out of border distance - ", borderDistance)
        exit(-1)

    if (detectionRect.y + detectionRect.height) > (image.getHeight() - borderDistance) or (detectionRect.y < borderDistance):
        print("Bounding Box height is out of border distance - ", borderDistance);
        exit(-1)

    err, bestShotQualityEstimation = bestShotQualityEstimator.estimate(image, detection, fe.BestShotQualityRequest.estimateHeadPose)
    if err.isError:
        print("Best shot quality estimation failed! Reason: {0}".format(err.what))
        exit(-1)

    headPoseEstimation = bestShotQualityEstimation.headpose_opt.value()

    print("Head pose estimation:", headPoseEstimation)

    principalAxes = 25

    if abs(headPoseEstimation.yaw) > principalAxes or abs(headPoseEstimation.pitch) > principalAxes or abs(headPoseEstimation.roll) > principalAxes:
        print("Can't estimate LivenessOneShotRGBEstimation. Yaw, pith or roll absolute value is larger than expected value: ", principalAxes)
        print(headPoseEstimation)
        exit(-1)

    # Make liveness estimation
    err, estimation = liveness_estimator.estimate(image, detection, landmarks5, 0)
    if err.isOk:
        print(estimation)
    else:
        print("Failed LivenessOneShotRGB estimation!. Reason: {0}".format(err.what))

