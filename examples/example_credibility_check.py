import sys
from math import sqrt

from example_license import make_activation


def print_help():
    print("python example.py <path to dir with FaceEngine*.so> <path to image>")


if len(sys.argv) != 3:
    print_help()
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

    # Create the warper object
    warper = faceEngine.createWarper()

    # Create detector object
    detector = faceEngine.createDetector(fe.FACE_DET_V3)

    # Create head pose estimator
    headPoseEstimator = faceEngine.createHeadPoseEstimator()

    # Create attribute estimator
    attributeEstimator = faceEngine.createAttributeEstimator()

    # Create quality estimator
    qualityEstimator = faceEngine.createQualityEstimator()

    # Create credibility check estimator
    credibilityEstimator = faceEngine.createCredibilityCheckEstimator()

    # Create overlap estimator
    overlapEstimator = faceEngine.createOverlapEstimator()

    # Max absolute yaw, pitch and roll head pose angles value for correct estimation
    principalAxes = 20

    # Minimal detection width for correct estimation
    minDetectionSize = 100

    # Minimal age for correct estimation
    minAge = 18

    # Read the input image
    image_path = sys.argv[2]
    print("image path: {0}".format(image_path))
    image = fe.Image()
    err = image.load(image_path)
    # Just exit in case of error
    if err.isError:
        print("Loading image failed! Reason: {0}".format(err))
        exit(-1)

    # Make detection
    print("Only one face will be handled.")
    err, face = detector.detectOne(
        image,
        image.getRect(),
        fe.DetectionType(fe.dt5Landmarks | fe.dtBBox)
    )

    if err.isError:
        print("Detection failed! Reason: {0}".format(err))
        exit(-1)

    if not face.isValid():
        print("Detection failed! Face is invalid")
        exit(-1)

    print("Face rect: ", face.detection.rect)

    detection = face.detection
    landmarks5 = face.landmarks5_opt.value()

    err, head_pose = headPoseEstimator.estimate(image, detection)
    if err.isError:
        print("Head pose estimation failed! Reason: {0}".format(err))
        exit(-1)

    if (abs(head_pose.yaw) > principalAxes or
        abs(head_pose.pitch) > principalAxes or
        abs(head_pose.roll) > principalAxes):
        print("Can't guarantee correctness of CredibilityCheckEstimation.")
        print("Yaw, pith or roll absolute value is larger than expected value: ", principalAxes)
        print(head_pose)

    detectionSize = detection.rect.width
    if detectionSize < minDetectionSize:
        print("Can't guarantee correctness of CredibilityCheckEstimation.")
        print("Detection size is less than expected value: ", minDetectionSize)
        print("Actual value: ", detectionSize)

    # Estimate face overlap
    err, overlap_estimation = overlapEstimator.estimate(image, detection)
    if err.isError:
        print("Overlap estimation failed! Reason: {0}".format(err))
        exit(-1)

    if overlap_estimation.overlapped:
        print("Can't guarantee correctness of CredibilityCheckEstimation.")
        print("Face is overlapped. ", overlap_estimation)

    # Warp image
    transformation = warper.createTransformation(detection, landmarks5)
    err, warped_image = warper.warp(image, transformation)
    if err.isError:
        print("Warping failed! Reason: {0}".format(err))
        exit(-1)

    # Estimate quality
    err, quality = qualityEstimator.estimate(warped_image)
    if err.isError:
        print("Quality estimation failed! Reason: {0}".format(err))
        exit(-1)

    if quality.isBlurred or quality.isHighlighted:
        print("Can't guarantee correctness of CredibilityCheckEstimation.")
        print("Image quality is too bad: ", quality)

    # Estimate age
    err, attributes = attributeEstimator.estimate(warped_image, fe.AttributeRequest.estimateAge)
    if err.isError:
        print("Attribute estimation failed! Reason: {0}".format(err))
        exit(-1)

    if not attributes.age_opt.isValid():
        print("Can't estimate Credibility. Age estimation is invalid")
        exit(-1)

    age = attributes.age_opt.value()
    if age < minAge:
        print("Can't guarantee correctness of CredibilityCheckEstimation.")
        print("Person must be adult. Age estimation: ", age)

    # Finally, credibility estimation
    err, estimation = credibilityEstimator.estimate(warped_image)
    if err.isError:
        print("Credibility check estimation failed! Reason: {0}".format(err))
        exit(-1)

    print(estimation)
