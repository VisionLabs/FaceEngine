import numpy as np
import cv2
import sys
import math


def help():
    print("python example_gaze_cv.py <path to dir with FaceEngine*.so>")


if len(sys.argv) != 2:
    help()
    exit(1)

sys.path.append(sys.argv[1])

import FaceEngine as fe
from example_license import make_activation

# correct paths or put directory "data" with example_gaze_cv.py
faceEngine = fe.createFaceEngine("data")
res = make_activation(faceEngine)
if res.isError:
    print("Failed to activate license! Reason: {0}".format(res.what))
    exit(-1)


def detect(detector, image_det):
    detector_result = detector.detectOne(image_det,
                                         image_det.getRect(),
                                         fe.DetectionType(fe.DT_ALL))
    return detector_result


def rotate_point_3d(point, pitch, yaw, roll):
    # roll is usually 0 and we needn't recalculate it
    point_roll = point

    point_yaw = [0, 0, 0]
    point_yaw[0] = point_roll[0] * math.cos(yaw) + point_roll[2] * math.sin(yaw)
    point_yaw[1] = point_roll[1]
    point_yaw[2] = -point_roll[2] * math.sin(yaw) + point_roll[2] * math.cos(yaw)

    point_pitch = [0, 0, 0]
    point_pitch[0] = point_yaw[0]
    point_pitch[1] = point_yaw[1] * math.cos(pitch) - point_yaw[2] * math.sin(pitch)
    point_pitch[2] = point_yaw[2] * math.sin(pitch) + point_yaw[2] * math.cos(pitch)

    return point_pitch


if __name__ == "__main__":
    detector = faceEngine.createDetector(fe.FACE_DET_V3)
    warper = faceEngine.createWarper()
    gaze_estimator_rgb = faceEngine.createGazeEstimator(fe.SensorType.Visible)
    segment = 100
    gaze = [(0, 0, 0), (0, 0, segment)]

    cap = cv2.VideoCapture(0)
    process = True
    frame = np.zeros((480, 640, 3))
    while process:
        # Capture frame-by-frame
        ret, frame = cap.read()
        if not ret:
            print("Could not read frame!")
            process = False
            break
        image = fe.Image()
        image.setData(frame, fe.FormatType.R8G8B8)
        if not image.isValid():
            continue
        err_detect, face = detect(detector, image)
        if err_detect.isOk or face.isValid():
            if face.landmarks5_opt.isValid() and face.landmarks68_opt.isValid() and face.detection.isValid():
                (detection, landmarks5, landmarks68) = face.detection, face.landmarks5_opt.value(), face.landmarks68_opt.value()
                transformation = warper.createTransformation(detection, landmarks5)
                err_transformed_landmarks5, transformed_landmarks5 = warper.warp(landmarks5, transformation)
                err_warp, warp = warper.warp(image, transformation)
                if err_warp.isOk and err_transformed_landmarks5.isOk:
                    err, eye_angles = gaze_estimator_rgb.estimate(warp, transformed_landmarks5)
                    err_unwarped, eye_angles_unwarped = warper.unwarp(eye_angles, transformation)
                if err.isOk and err_unwarped.isOk:
                    # print(eye_angles)
                    transformation = warper.createTransformation(detection, landmarks5)
                    rect = detection.getRect()
                    center_x_left = landmarks5[0].x + rect.x
                    center_y_left = landmarks5[0].y + rect.y
                    center_x_right = landmarks5[1].x + rect.x
                    center_y_right = landmarks5[1].y + rect.y
                    rotated_gaze = rotate_point_3d(gaze[1],
                                                   eye_angles_unwarped.pitch / 180 * math.pi,
                                                   -eye_angles_unwarped.yaw / 180 * math.pi,
                                                   0)
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
                        cv2.circle(frame, (int(landmark.x + rect.x),
                                           int(landmark.y + rect.y)), 1, (0, 0, 255), -1)
        cv2.imshow('frame', frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # When everything done, release the capture
    cap.release()
    cv2.destroyAllWindows()
