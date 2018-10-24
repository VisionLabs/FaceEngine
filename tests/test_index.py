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


class ProgressTracker(fe.ProgressTracker):
    reports = []
    sz = 0

    def __init__(self, sz, reports):
        self.reports = reports
        self.sz = sz

    def __init__(self):
        self.reports = []
        self.sz = 0

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
        l = descriptor.getDescriptor()
        for i in l:
            print(i)
        return descriptor


def loadAcquiredBatch(_faceEngine, _fileName):
    descriptorBatch = _faceEngine.createDescriptorBatch(sizeOfBatch)
    with open(_fileName, "rb") as file:
        data = file.read()
        fileBatchSize = len(data)
        descriptorBatch.load(data, fileBatchSize)
        print(descriptorBatch.getMaxCount())
        print(descriptorBatch.getCount())
        print(descriptorBatch.getModelVersion())
        print(descriptorBatch.getDescriptorSize())
        print(descriptorBatch.getDescriptorSize())
        return descriptorBatch


def buildAcquiredIndexWithBatchAndDoBeforeBuild(
        _indexBuilder,
        _faceEngine,
        _batch,
        _indexesToRemove,
        _funcToCallBeforeBuild,
        _checkProgress = True):
    print("MSD buildAcquiredIndexWithBatchAndDoBeforeBuild")
    _indexBuilder.appendBatch(_batch)
    if _funcToCallBeforeBuild:
        _funcToCallBeforeBuild(_indexBuilder, _indexesToRemove, _batch, _faceEngine)
    tracker = ProgressTracker()
    buildRes = _indexBuilder.buildIndex()
    index = buildRes[1]
    assert(buildRes[0].isOk and index)

    # if _checkProgress:
    #     assert(tracker.sz == sizeOfBatch/100)
    #     for i in range(tracker.sz):
    #         assert(tracker.reports[i] - 0.1 * (i + 1) > 0)
    return index


def buildAcquiredIndexWithBatch(_faceEngine, _indexPath):
    # return tuple (error code, value)
    denseIndexRes = _faceEngine.loadDenseIndex(_indexPath)
    assert(denseIndexRes[0].isOk)
    loadedIndex = denseIndexRes[1]
    return loadedIndex


def loadAcquiredDenseIndex(_faceEngine, _indexPath):
    # return tuple (error code, value)
    loadIndexRes = _faceEngine.loadDenseIndex(_indexPath)
    print(loadIndexRes)
    assert(loadIndexRes[0].isOk)
    loadedIndex = loadIndexRes[1]
    return loadedIndex



class TestFaceEngineRect(unittest.TestCase):

    def areSearchResultsEqual(self, firstRes, secondRes, firstArray, secondArray):
        self.assertEqual(firstRes.isOk, secondRes.isOk)
        self.assertEqual(firstRes.what, secondRes.what)
        self.assertEqual(len(firstArray), len(secondArray))
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
        for i, elem in enumerate(desc1):
            print(desc1[i], desc2[i])
        self.assertTrue(desc1 == desc2)
        self.assertEqual(d1.getModelVersion(), d2.getModelVersion())
        self.assertEqual(d1.getDescriptorLength(), d2.getDescriptorLength())

    def query(self, _batch, _storage, _faceEngine, index):
        # test good
        print(_batch.getCount())
        batchDescr = _batch.getDescriptorSlow(index.good)
        emptyDescr = _faceEngine.createDescriptor()
        newDescr = _storage.descriptorByIndex(index.good, emptyDescr)[1]
        print(newDescr)
        l = newDescr.getDescriptor()
        for i in l:
            print(i)
        self.assertTrue(_storage.descriptorByIndex(index.good, emptyDescr))
        self.checkDescriptorsEquality(batchDescr, newDescr)
        # test bad
        emptyDescr = _faceEngine.createDescriptor()
        print(_storage.descriptorByIndex(index.bad, emptyDescr)[0])
        self.assertEqual(_storage.descriptorByIndex(index.bad, emptyDescr)[0].FSDKError, fe.FSDKError.InvalidInput)

    def testIndexSearchResult(self):
        similarity = 0.6543
        distance = 8295526.0
        index = 923924
        res = fe.SearchResult(distance, similarity, index)
        print("MSD", sys.getsizeof(res))
        print(res)
        self.assertEqual(res.distance, distance)
        self.assertAlmostEqual(res.similarity, similarity)
        self.assertEqual(res.index, index)

    def testDenseSerialization(self):
        faceEngine = loadAcquiredFaceEngineWithCnn46()
        descriptor = loadAcquiredDescriptor(faceEngine, testDataPath + "/descriptor1_46.bin")
        batch = loadAcquiredBatch(faceEngine, testDataPath + "/batch46_eq1k.bin")

        def funcToPass(_builder, _indexesToRemove, _batch, _faceEngine):
            for index in _indexesToRemove:
                _builder.removeDescriptor(index)
                self.query(_batch, _builder, _faceEngine, IndexTest(188, 1141))
        countOfRemovals = 5
        indexesToRemove = [459, 245, 651, 832, 634]
        indexBuilder = faceEngine.createIndexBuilder()
        builtIndex = buildAcquiredIndexWithBatchAndDoBeforeBuild(indexBuilder,
                                                                 faceEngine,
                                                                 batch,
                                                                 indexesToRemove,
                                                                 funcToPass)
        resBuilt = builtIndex.search(descriptor, searchResultSize)

        self.assertEqual(builtIndex.size(), sizeOfBatch - countOfRemovals)
        self.assertEqual(builtIndex.countOfIndexedDescriptors(), sizeOfBatch - countOfRemovals)
        self.query(batch, builtIndex, faceEngine, IndexTest(169, 7712))
        densePath = testDataPath + "/dense_index.txt"
        err = builtIndex.saveToDenseIndex(densePath)
        print("MSD saved image {0}".format(err))
        loadedDenseIndex = loadAcquiredDenseIndex(faceEngine, densePath)
        print(loadedDenseIndex)
        self.query(batch, loadedDenseIndex, faceEngine, IndexTest(108, 9228))
        resDeser = loadedDenseIndex.search(descriptor, searchResultSize)
        print(resDeser)
        print(resBuilt[0], resDeser[0])
        print(resBuilt[1], resDeser[1])
        self.areSearchResultsEqual(resBuilt[0], resDeser[0], resBuilt[1], resDeser[1])
        self.assertTrue(self.areSearchResultsEqual(resBuilt[0], resDeser[0], resBuilt[1], resDeser[1]))



if __name__ == '__main__':
    unittest.main()

