import sys
import numpy as np
import cv2

if len(sys.argv) != 3:
    help()
    exit(1)

# if FaceEngine is not installed pass the path to dir with FaceEngine*.so and add it to system paths
sys.path.append(sys.argv[1])
# if FaceEngine is installed only
import FaceEngine as fe
import TrackEngine as te

bboxes = {}
bestshots = {}


def process_callbacks(clb):
    for c in clb:
        if c.type == te.ctVisual:
            bboxes[c.trackId] = c.bbox
        elif c.type == te.ctBestShot:
            if c.trackId in bestshots.keys():
                if bestshots[c.trackId].score < c.score:
                    bestshots[c.trackId] = c
            else:
                bestshots[c.trackId] = c
        elif c.type == te.ctTrackEnd:
            del bboxes[c.trackId]
            if c.trackId in bestshots.keys():
                bestshot = bestshots[c.trackId]
                bestshot_image = bestshot.image.getData()
                bestshot = bestshot_image[bestshot.bbox.y:bestshot.bbox.y + bestshot.bbox.height, bestshot.bbox.x:bestshot.bbox.x + bestshot.bbox.width]
                bestshot = cv2.cvtColor(bestshot, cv2.COLOR_BGR2RGB)
                cv2.imshow(str(c.trackId), bestshot)
                del bestshots[c.trackId]


def draw_bounding_boxes(frame_to_draw, bboxes_to_draw):
    for t, b in bboxes_to_draw.items():
        cv2.rectangle(frame_to_draw,
                      (b.x, b.y),
                      (b.x + b.width, b.y + b.height),
                      (25, 240, 25),
                      2)
        font = cv2.FONT_HERSHEY_SIMPLEX
        cv2.putText(frame_to_draw, str(t), (b.x, b.y - 10), font, 1, (25, 240, 25), 2)


if __name__ == "__main__":
    cap = cv2.VideoCapture(int(sys.argv[2]))
    faceEngine = fe.createFaceEngine("data", "data/faceengine.conf")
    trackEngine = te.createTrackEngine(faceEngine, "data/trackengine.conf")
    stream = trackEngine.createStream()
    x = 0
    while True:
        # Capture frame-by-frame
        ret, frame = cap.read()

        image = fe.Image()
        rgbframe = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
        image.setData(rgbframe)

        if not stream.pushFrame(image, x):
            print("push error {0}".format(x))

        draw_bounding_boxes(frame, bboxes)

        x = x + 1
        callbacks = stream.getCallbacks()

        cv2.imshow('frame', frame)

        process_callbacks(callbacks)

        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    stream.waitStream()
    callbacks = stream.getCallbacks()
    process_callbacks(callbacks)

    # When everything done, release the capture
    cap.release()
    cv2.destroyAllWindows()