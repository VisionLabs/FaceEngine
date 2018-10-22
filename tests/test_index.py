import sys
import unittest
import argparse
import sys
import os
import glob
import logging
import struct

# if FaceEngine is not installed within the system, add the directory with FaceEngine*.so to system paths
parser = argparse.ArgumentParser()
parser.add_argument("-b", "--bind-path", type=str,
                    help="path to FaceEngine*.so file - binding of luna-sdk")

args = parser.parse_args()
path_to_binding = args.bind_path
print(path_to_binding)
if not os.path.isdir(path_to_binding):
    print("Directory with FaceEngine*.so was not found.")
    exit(1)


print("Directory {0} with python bindings of FaceEngine was included".format(path_to_binding))
print(sys.argv)

sys.path.append(path_to_binding)

# if FaceEngine is installed only this string of code is required for module importing
import FaceEngine as fe

# erase two first arguments for unittest argument parsing
del(sys.argv[1])
del(sys.argv[1])

configPath = "data" + "/faceengine.conf"
testDataPath = "testData"
faceEnginePtr = fe.createFaceEngine("data",
                                   configPath)

class IndexTest:
    good = 0
    bad = 0

    def __init__(self, _good, _bad):
        self.good = _good
        self.bad = _bad

# warper example
max_detections = 3
image_det = fe.Image()
err = image_det.load("testData/00205_9501_p.ppm")


searchResultSize = 10
sizeOfBatch = 999
reference = [763, 762, 852, 850, 851, 600, 936, 886, 739, 152]


class ProgressTracker(fe.IProgressTracker):
    reports = []
    sz = 0

    def progress(self, completion):
        print("Progress: %f", completion)
        self.reports.append(completion)
        self.sz += 1


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
    descriptorBatch = _faceEngine.createDescriptorBatch(sizeOfBatch)
    with open(_fileName, "rb") as file:
        data = file.read()
        fileBatchSize = len(data)
        descriptorBatch.load(data, fileBatchSize)
        return descriptorBatch

def buildAcquiredIndexWithBatchAndDoBeforeBuild(
        _faceEngine,
        batch,
        funcToCallBeforeBuild, checkProgress = True):
    indexBuilder = _faceEngine.createIndexBuilder()
    indexBuilder.appendBatch(batch)
    if funcToCallBeforeBuild:
        funcToCallBeforeBuild(indexBuilder)
    tracker = ProgressTracker()
    buildRes = indexBuilder.buildIndex(tracker)
    index = buildRes[1]
    assert(buildRes.isOk and index)

    if checkProgress:
        assert(tracker.sz == sizeOfBatch/100)
        for i in range(tracker.sz):
            assert(tracker.reports[i] - 0.1 * (i + 1) > 0)
    return index

def buildAcquiredIndexWithBatch(_faceEngine, _indexPath):
    # return tuple (error code, value)
    denseIndexRes = _faceEngine.loadDenseIndex(_indexPath)
    assert(denseIndexRes[0].isOk)
    loadedIndex = denseIndexRes[1]
    return loadedIndex

def loadAcquiredDenseIndex(_faceEngine, _indexPath):
    # return tuple (error code, value)
    loadIndexRes = _faceEngine.loadDynamicIndex(_indexPath)
    assert(loadIndexRes[0].isOk)
    loadedIndex = loadIndexRes[1]
    return loadedIndex



class TestFaceEngineRect(unittest.TestCase):

    def areSearchResultsEqual(self, firstRes, secondRes, firstArray, secondArray):
        self.assertEqual(firstRes[0].isOk, secondRes[0].isOk)
        self.assertEqual(firstRes[1], secondRes[1])
        self.assertEqual(len(firstRes[1]), len(secondRes[1]))
        allEqual = True
        size = len(firstArray)
        def areEqual(firstArray, secondArray):
            self.assertEqual(len(firstArray), len(secondArray))
            for i, elem in enumerate(firstArray):
                if (firstArray[i].index != secondArray[i].index or
                firstArray[i].distance != secondArray[i].distance or
                firstArray[i].similarity != secondArray[i].similarity):
                    return False
            return True
        equal = areEqual(firstArray, secondArray)
        self.assertTrue(equal)
        allEqual = allEqual and equal
        self.assertTrue(allEqual)
        return allEqual

    def checkDescriptorsEquality(self, d1, d2):
        desc1 = d1.getDescriptor()
        desc2 = d2.getDescriptor()
        self.assertTrue(desc1 == desc2)
        self.assertEqual(d1.getModelVersion, d2.getModelVersion())
        self.assertEqual(d1.getDescriptorLength(), d2.getDescriptorLength())

    def query(self, _batch, _storage, _faceEngine, index):
        # test good
        batchDescr = _batch.getDescriptorSlow(index.good)
        emptyDescr = _faceEngine.createDescriptor()
        self.assertTrue(_storage.descriptorByIndex(index.good, emptyDescr))
        self.checkDescriptorsEquality(batchDescr, emptyDescr)
        # test bad
        emptyDescr = _faceEngine.createDescriptor()
        self.assertEqual(_storage.descriptorByIndex(index.bad, emptyDescr).FSDKError, _faceEngine.FSDKError.InvalidInput)
        self.checkDescriptorsEquality(batchDescr, emptyDescr)

    def testIndexSearchResult(self):
        similarity = 0.6543
        distance = 8295439.0
        index = 923924
        res = fe.SearchResult(distance, similarity, index)
        print(res)
        self.assertEqual(res.distance, distance)
        self.assertAlmostEqual(res.similarity, similarity)
        self.assertEqual(res.index, index)

    def testDenseSerialization(self):
        faceEngine = loadAcquiredFaceEngineWithCnn46()
        descriptor = loadAcquiredDescriptor(faceEngine, testDataPath + "/descriptor1_46.bin")
        batch = loadAcquiredBatch(faceEngine, testDataPath + "/batch46_eq1k.bin")
        def funcToPass(builder):
            for index in indexesToRemove:
                builder.removeDescriptor(index)
                self.query(batch, builder, faceEngine, IndexTest(188, 1141))
        countOfRemovals = 5
        indexesToRemove = [459, 245, 651, 832, 634]
        # builtIndex = bui




if __name__ == '__main__':
    unittest.main()

