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

configPath = "data" + "/faceengine.conf"
testDataPath = "testData"

searchResultSize = 10
sizeOfBatch = 999
reference = [763, 762, 852, 850, 851, 600, 936, 886, 739, 152]


def loadAcquiredFaceEngineWithCnn46():
    faceEnginePtr = fe.createFaceEngine("data",
                                       configPath)
    config = fe.createSettingsProvider(configPath)
    config.setValue("DescriptorFactory::Settings", "model", fe.SettingsProviderValue(46))
    faceEnginePtr.setSettingsProvider(config)
    print(config.getValue("DescriptorFactory::Settings", "model").asInt())
    return faceEnginePtr


def loadAcquiredDescriptor(_faceEngine, _fileName):
    descriptor = _faceEngine.createDescriptor()
    with open(_fileName, "rb") as file:
        data = file.read()
        fileDescriptorSize = len(data)
        descriptor.load(data, fileDescriptorSize)
        return descriptor


def loadAcquiredBatch(_faceEngine, _fileName):
    batch = _faceEngine.createDescriptorBatch(sizeOfBatch)
    with open(_fileName, "rb") as file:
        data = file.read()
        fileBatchSize = len(data)
        print("MSD len of batch = %d", fileBatchSize)
        batch.load(data, fileBatchSize)

    return batch

if __name__ == "__main__":
    # correct path or put directory "data" with example.py
    faceEngine = fe.createFaceEngine("data",
                                     "data/faceengine.conf")
    faceEngine.createIndexBuilder()

    faceEngine = loadAcquiredFaceEngineWithCnn46()
    descriptor = loadAcquiredDescriptor(faceEngine, testDataPath + "/descriptor1_46.bin")
    batch = loadAcquiredBatch(faceEngine, testDataPath + "/batch46_eq1k.bin")
    countOfRemovals = 5
    indexesToRemove = [459, 245, 651, 832, 634]