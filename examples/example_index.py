import sys
# import numpy as np
# from matplotlib import pyplot as plt

def help():
    print("python example.py <path to FaceEngine*.so> <path to image>")


if len(sys.argv) != 2:
    help()
    exit(1)

# pass path dir with FaceEngine*.so and add it to system directory
sys.path.append(sys.argv[1])
import FaceEngine as fe

if __name__ == "__main__":
    # correct path or put directory "data" with example.py
    faceEngine = fe.createFaceEngine("data",
                                     "data/faceengine.conf")
    faceEngine.createIndexBuilder()
    # res1 = faceEngine.loadDenseIndex("")
    # print("DenseIndex", res1)
    res2 = faceEngine.loadDynamicIndex("")
    print("DynamicIndex", res2)
