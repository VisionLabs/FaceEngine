import sys
from time import sleep

def help():
    print("python example_detector_warper.py <path to FaceEngine*.so> <path to image>")

if len(sys.argv) != 3:
    help()
    exit(1)

# if FaceEngine is not installed pass the path to dir with FaceEngine*.so and add it to system paths
sys.path.append(sys.argv[1])
# if FaceEngine is installed only
import FaceEngine as fe
import TrackEngine as te
# correct paths or put directory "data" with example_detector_warper.py
faceEngine = fe.createFaceEngine("data", "data/faceengine.conf")
trackEngine = te.createTrackEngine(faceEngine, "data/trackengine.conf")

if __name__ == "__main__":
    image_path = sys.argv[2]
    image = fe.Image()
    err_image_loaded = image.load(image_path)
    if not image.isValid():
        print("Image error = ", err_image_loaded)
        exit(-1)
    stream = trackEngine.createStream()
    print(trackEngine.getStr())
    print(trackEngine.getFSDKVer(faceEngine))
    for x in range(0, 200):
        stream.pushFrame(image)
    sleep(2)
