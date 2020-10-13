import sys

def help():
    print("python example_detector_warper.py <path to dir with FaceEngine*.so> <path to image>")

if len(sys.argv) != 3:
    help()
    exit(1)

# if FaceEngine is not installed pass the path to dir with FaceEngine*.so and add it to system paths
sys.path.append(sys.argv[1])
# if FaceEngine is installed only
import FaceEngine as fe
import TrackEngine as te
from example_license import make_activation


if __name__ == "__main__":
    # correct paths or put directory "data" with example_detector_warper.py
    faceEngine = fe.createFaceEngine("data")
    if not make_activation(faceEngine):
        print("failed to activate license!")
        exit(-1)
    trackEngine = te.createTrackEngine(faceEngine, "data/trackengine.conf")
    image_path = sys.argv[2]
    image = fe.Image()
    err_image_loaded = image.load(image_path, fe.FormatType.R8G8B8)
    if not image.isValid():
        print("Image error = ", err_image_loaded)
        exit(-1)
    stream = trackEngine.createStream()
    for x in range(0, 20):
        if not stream.pushFrame(image, x):
            print("push error {0}".format(x))

    print("All frames are pushed")
    stream.waitStream()
    clb = stream.getCallbacks()
    for c in clb:
        if c.type == te.ctVisual:
            print("have callback {0} {1}".format(c.frameId, c.trackId))
