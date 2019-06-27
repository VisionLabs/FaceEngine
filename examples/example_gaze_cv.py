import numpy as np
import cv2
import sys
import math


def help():
    print("python example.py <path to dir with FaceEngine*.so> <path to video>")


if len(sys.argv) != 3:
    help()
    exit(1)

sys.path.append(sys.argv[1])
import FaceEngine as fe

faceEngine = fe.createFaceEngine("data",
                                 "data/faceengine.conf")


def detect(detector, image_det):
    detector_result = detector.detectOne(image_det,
                                         image_det.getRect(),
                                         fe.DetectionType(fe.dtAll))
    return detector_result


def rotate_point_3d(point, pitch, yaw, roll):
    pointRoll = [0, 0, 0]
    pointRoll[0] = point[0] * math.cos(roll) - point[1] * math.sin(roll)
    pointRoll[1] = point[0] * math.sin(roll) + point[1] * math.cos(roll)
    pointRoll[2] = point[2]

    pointYaw = [0, 0, 0]
    pointYaw[0] = pointRoll[0] * math.cos(yaw) + pointRoll[2] * math.sin(yaw)
    pointYaw[1] = pointRoll[1]
    pointYaw[2] = -pointRoll[2] * math.sin(yaw) + pointRoll[2] * math.cos(yaw)

    pointPitch = [0, 0, 0]
    pointPitch[0] = pointYaw[0]
    pointPitch[1] = pointYaw[1] * math.cos(pitch) - pointYaw[2] * math.sin(pitch)
    pointPitch[2] = pointYaw[2] * math.sin(pitch) + pointYaw[2] * math.cos(pitch)

    return pointPitch


if __name__ == "__main__":
    detector = faceEngine.createDetector(fe.FACE_DET_V3)
    warper = faceEngine.createWarper()
    gaze_estimator_rgb = faceEngine.createGazeEstimator()
    eyeEstimator = faceEngine.createEyeEstimator()
    cropper = fe.EyeCropper()
    segment = 100
    gaze = [(0, 0, 0), (0, 0, segment)]

    cap = cv2.VideoCapture(0)
    process = True
    frame = np.zeros((480, 640, 3))
    while(process):
        # Capture frame-by-frame
        cv2.imshow('frame', frame)
        ret, frame = cap.read()
        if not ret:
            print("Image is empty")
            process = False
            break
        image = fe.Image()
        image.setData(frame)
        if not image.isValid():
            continue
        err_detect, face = detect(detector, image)
        if err_detect.isError or not face.isValid():
            cv2.waitKey(1)
            print("detection not found")
            continue
        if not face.landmarks5_opt.isValid() or not face.landmarks68_opt.isValid() or not face.detection.isValid():
            continue
        (detection, landmarks5, landmarks68) = face.detection, face.landmarks5_opt.value(), face.landmarks68_opt.value()
        transformation = warper.createTransformation(detection, landmarks5)
        err_transformed_landmarks5, transformed_landmarks5 = warper.warp(landmarks5, transformation)
        err_warp, warp = warper.warp(image, transformation)
        if err_warp.isError:
            continue
        if err_transformed_landmarks5.isError:
            continue
        err, eye_angles = gaze_estimator_rgb.estimate(warp, landmarks5, transformed_landmarks5)
        if err.isError:
            continue
        # print(eye_angles)
        transformation = warper.createTransformation(detection, landmarks5)
        center_x_left = landmarks5[0].x + detection.rect.x
        center_y_left = landmarks5[0].y + detection.rect.y
        center_x_right = landmarks5[1].x + detection.rect.x
        center_y_right = landmarks5[1].y + detection.rect.y
        rotated_gaze = rotate_point_3d(gaze[1], eye_angles.pitch / 180 * math.pi, -eye_angles.yaw / 180 * math.pi, 0)
        cv2.line(
            frame,
            (int(center_x_left), int(center_y_left)),
            (int(rotated_gaze[0] + center_x_left), int(rotated_gaze[1] + center_y_left)),
            (0, 255, 0), 4)
        cv2.line(
            frame,
            (int(center_x_right), int(center_y_right)),
            (int(rotated_gaze[0] + center_x_right), int(rotated_gaze[1] + center_y_right)),
            (0, 255, 0), 4)

        for i in range(len(landmarks68)):
            landmark = landmarks68[i]
            cv2.circle(frame, (int(landmark.x + detection.rect.x),
                               int(landmark.y + detection.rect.y)), 2, (0, 0, 255), -1)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # When everything done, release the capture
    cap.release()
    cv2.destroyAllWindows()