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
    # pointRoll = [0, 0, 0]
    # pointRoll[0] = point[0] * math.cos(roll) - point[1] * math.sin(roll)
    # pointRoll[1] = point[0] * math.sin(roll) + point[1] * math.cos(roll)
    # pointRoll[2] = point[2]
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
    gaze_estimator_rgb = faceEngine.createGazeEstimator()
    eyeEstimator = faceEngine.createEyeEstimator()
    cropper = fe.EyeCropper()
    segment = 100
    gaze = [(0, 0, 0), (0, 0, segment)]

    cap = cv2.VideoCapture(0)
    process = True
    frame = np.zeros((480, 640, 3))
    while process:
        # Capture frame-by-frame
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
        if err_detect.isOk or face.isValid():
            if face.landmarks5_opt.isValid() and face.landmarks68_opt.isValid() and face.detection.isValid():
                (detection, landmarks5, landmarks68) = face.detection, face.landmarks5_opt.value(), face.landmarks68_opt.value()
                transformation = warper.createTransformation(detection, landmarks5)
                err_transformed_landmarks5, transformed_landmarks5 = warper.warp(landmarks5, transformation)
                err_warp, warp = warper.warp(image, transformation)
                if err_warp.isOk and err_transformed_landmarks5.isOk:
                    err, eye_angles = gaze_estimator_rgb.estimate(warp, landmarks5, transformed_landmarks5)
                if err.isOk:
                    # print(eye_angles)
                    transformation = warper.createTransformation(detection, landmarks5)
                    center_x_left = landmarks5[0].x + detection.rect.x
                    center_y_left = landmarks5[0].y + detection.rect.y
                    center_x_right = landmarks5[1].x + detection.rect.x
                    center_y_right = landmarks5[1].y + detection.rect.y
                    rotated_gaze = rotate_point_3d(gaze[1],
                                                   eye_angles.pitch / 180 * math.pi,
                                                   -eye_angles.yaw / 180 * math.pi,
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
                        cv2.circle(frame, (int(landmark.x + detection.rect.x),
                                           int(landmark.y + detection.rect.y)), 1, (0, 0, 255), -1)
        cv2.imshow('frame', frame)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    # When everything done, release the capture
    cap.release()
    cv2.destroyAllWindows()