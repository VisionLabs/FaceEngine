import sys
import unittest
import argparse
import sys
import os
from license_helper import make_activation, ActivationLicenseError

# if FaceEngine is not installed within the system, add the directory with FaceEngine*.so to system paths
parser = argparse.ArgumentParser()
parser.add_argument("-b", "--bind-path", type=str,
                    help="path to dir with FaceEngine*.so file - binding of luna-sdk")

print("This test is only for server platforms!")

args = parser.parse_args()

if len(sys.argv) == 1 or not args.bind_path or not os.path.isdir(args.bind_path):
    parser.print_help(sys.stderr)
    sys.exit(1)

path_to_binding = args.bind_path
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


def loadAcquiredFaceEngineWithCnn46():
    faceEnginePtr = fe.createFaceEngine("data",
                                        configPath)
    if not make_activation(faceEnginePtr):
        raise ActivationLicenseError("License is not activated!")
    config = fe.createSettingsProvider(configPath)
    config.setValue("DescriptorFactory::Settings", "model", fe.SettingsProviderValue(46))
    faceEnginePtr.setSettingsProvider(config)
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
        _indexBuilder,
        _faceEngine,
        _batch,
        _funcToCallBeforeBuild=None,
        _indexesToRemove=[],
        _checkProgress=True):
    _indexBuilder.appendBatch(_batch)
    if _funcToCallBeforeBuild:
        _funcToCallBeforeBuild(_indexBuilder, _indexesToRemove, _batch, _faceEngine)
    # tracker = ProgressTracker()
    buildRes = _indexBuilder.buildIndex()
    index = buildRes[1]
    assert(buildRes[0].isOk and index)
    return index


def buildAcquiredIndexWithBatch(_faceEngine, _batch):
    # return tuple (error code, value)
    indexBuilder = _faceEngine.createIndexBuilder()
    return buildAcquiredIndexWithBatchAndDoBeforeBuild(indexBuilder, _faceEngine, _batch, None, False)


def loadAcquiredDenseIndex(_faceEngine, _indexPath):
    # return tuple (error code, value)
    loadIndexRes = _faceEngine.loadDenseIndex(_indexPath)
    assert(loadIndexRes[0].isOk)
    loadedIndex = loadIndexRes[1]
    return loadedIndex


def load(descrFileName, batchFileName):
    faceEngine = loadAcquiredFaceEngineWithCnn46()
    descriptor = loadAcquiredDescriptor(faceEngine, testDataPath + "/" + descrFileName)
    batch = loadAcquiredBatch(faceEngine, testDataPath + "/" + batchFileName)
    return faceEngine, descriptor, batch


class TestFaceEngineRect(unittest.TestCase):
    faceEngine = fe.createFaceEngine("data",
                                     configPath)
    @classmethod
    def setUp(cls):
        if not make_activation(cls.faceEngine):
            raise ActivationLicenseError("License is not activated!")

    def loadAcquiredDynamicIndex(self, _faceEngine, _indexPath):
        loadIndexRes = _faceEngine.loadDynamicIndex(_indexPath)
        self.assertTrue(loadIndexRes[0].isOk)
        loadedIndex = loadIndexRes[1]
        return loadedIndex

    def areSearchResultsEqual(self, _firstRes, _secondRes, _firstArray, _secondArray):
        self.assertEqual(_firstRes.isOk, _secondRes.isOk)
        self.assertEqual(_firstRes.what, _secondRes.what)
        self.assertEqual(len(_firstArray), len(_secondArray))
        allEqual = True
        def areEqual(firstArray, secondArray):
            self.assertEqual(len(firstArray), len(secondArray))
            for i, elem in enumerate(firstArray):
                if (firstArray[i].index != secondArray[i].index or
                firstArray[i].distance != secondArray[i].distance or
                firstArray[i].similarity != secondArray[i].similarity):
                    return False
            return True
        equal = areEqual(_firstArray, _secondArray)
        self.assertTrue(equal)
        allEqual = allEqual and equal
        self.assertTrue(allEqual)
        return allEqual

    def checkDescriptorsEquality(self, d1, d2):
        desc1 = d1.getDescriptor()
        desc2 = d2.getDescriptor()
        self.assertTrue(desc1 == desc2)
        self.assertEqual(d1.getModelVersion(), d2.getModelVersion())
        self.assertEqual(d1.getDescriptorLength(), d2.getDescriptorLength())

    def query(self, _batch, _storage, _faceEngine, index):
        # test good
        batchDescr = _batch.getDescriptorSlow(index.good)
        emptyDescr = _faceEngine.createDescriptor()
        newDescr = _storage.descriptorByIndex(index.good, emptyDescr)[1]
        self.assertTrue(_storage.descriptorByIndex(index.good, emptyDescr))
        self.checkDescriptorsEquality(batchDescr, newDescr)
        # test bad
        emptyDescr = _faceEngine.createDescriptor()
        self.assertEqual(_storage.descriptorByIndex(index.bad, emptyDescr)[0].error, fe.FSDKError.InvalidInput)

    def testIndexSearchResult(self):
        similarity = 0.6543
        distance = 8295526.0
        index = 923924
        res = fe.SearchResult(distance, similarity, index)
        self.assertEqual(res.distance, distance)
        self.assertAlmostEqual(res.similarity, similarity)
        self.assertEqual(res.index, index)

    def testDenseSerialization(self):
        faceEngine, descriptor, batch = load("descriptor1_46.bin", "batch46_eq1k.bin")

        def funcToPass(_builder, _indexesToRemove, _batch, _faceEngine):
            for index in _indexesToRemove:
                _builder.removeDescriptor(index)
                self.query(_batch, _builder, _faceEngine, IndexTest(188, 1141))
        countOfRemovals = 5
        indicesToRemove = [459, 245, 651, 832, 634]
        indexBuilder = faceEngine.createIndexBuilder()
        builtIndex = buildAcquiredIndexWithBatchAndDoBeforeBuild(indexBuilder, faceEngine, batch, funcToPass, indicesToRemove)
        resBuilt = builtIndex.search(descriptor, searchResultSize)

        self.assertEqual(builtIndex.size(), sizeOfBatch - countOfRemovals)
        self.assertEqual(builtIndex.countOfIndexedDescriptors(), sizeOfBatch - countOfRemovals)
        self.query(batch, builtIndex, faceEngine, IndexTest(169, 7712))
        densePath = testDataPath + "/dense_index.txt"
        err = builtIndex.saveToDenseIndex(densePath)
        loadedDenseIndex = loadAcquiredDenseIndex(faceEngine, densePath)
        self.query(batch, loadedDenseIndex, faceEngine, IndexTest(108, 9228))
        resDeser = loadedDenseIndex.search(descriptor, searchResultSize)
        self.assertTrue(self.areSearchResultsEqual(resBuilt[0], resDeser[0], resBuilt[1], resDeser[1]))

    # DynamicSerialization
    def testDynamicSerialization(self):
        faceEngine, descriptor, batch = load("descriptor1_46.bin", "batch46_eq1k.bin")
        builtIndex = buildAcquiredIndexWithBatch(faceEngine, batch)
        self.assertEqual(builtIndex.size(), sizeOfBatch)
        self.assertEqual(builtIndex.countOfIndexedDescriptors(), sizeOfBatch)
        resBuilt = builtIndex.search(descriptor, searchResultSize);
        indexPath = testDataPath + "/dynamic_index_tmp"
        res = builtIndex.saveToDynamicIndex(indexPath)
        self.assertTrue(res.isOk)
        loadedIndex = self.loadAcquiredDynamicIndex(faceEngine, indexPath)
        self.assertEqual(loadedIndex.size(), sizeOfBatch)
        self.assertEqual(loadedIndex.size(), sizeOfBatch)
        self.assertEqual(loadedIndex.countOfIndexedDescriptors(), sizeOfBatch)
        self.query(batch, loadedIndex, faceEngine, IndexTest(212, 68705))
        resDeser = loadedIndex.search(descriptor, searchResultSize)
        self.assertTrue(self.areSearchResultsEqual(resBuilt[0], resDeser[0], resBuilt[1], resDeser[1]))

    def testSearchDeser(self):
        faceEngine, descriptor, batch = load("descriptor1_46.bin", "batch46_eq1k.bin")
        builtIndex = buildAcquiredIndexWithBatch(faceEngine, batch)
        indexPath = testDataPath + "/dense_index"
        builtIndex.saveToDenseIndex(indexPath)
        loadedIndex = loadAcquiredDenseIndex(faceEngine, indexPath)
        resDeser = loadedIndex.search(descriptor, searchResultSize)
        self.assertTrue(resDeser[0].isOk)
        searchArrayDeser = resDeser[1]
        for i in range(len(resDeser[1])):
            self.assertEqual(searchArrayDeser[i].index, reference[i])

    def testDynamicSearchBuilt(self):
        faceEngine, descriptor, batch = load("descriptor1_46.bin", "batch46_eq1k.bin")
        builtIndex = buildAcquiredIndexWithBatch(faceEngine, batch)
        resBuilt = builtIndex.search(descriptor, searchResultSize)
        self.assertTrue(resBuilt[0].isOk)
        for i in range(len(resBuilt[1])):
            self.assertEqual(resBuilt[1][i].index, reference[i])

    def testDynamicSearchDeser(self):
        faceEngine, descriptor, batch = load("descriptor1_46.bin", "batch46_eq1k.bin")
        builtIndex = buildAcquiredIndexWithBatch(faceEngine, batch)
        indexPath = testDataPath + "/dynamic_index_tmp"
        builtIndex.saveToDynamicIndex(indexPath)
        loadedIndex = self.loadAcquiredDynamicIndex(faceEngine, indexPath)
        resDeser = loadedIndex.search(descriptor, searchResultSize)
        self.assertTrue(resDeser[0].isOk)
        for i in range(len(resDeser[1])):
            self.assertEqual(resDeser[1][i].index, reference[i])

    def testDynamicAppend(self):
        faceEngine, descriptor, batch = load("descriptor1_46.bin", "batch46_eq1k.bin")
        builtIndex = buildAcquiredIndexWithBatch(faceEngine, batch)
        # fake descr
        fakeDescriptor = faceEngine.createDescriptor()
        data = bytes(256)
        fakeDescriptor.load(data, len(data))
        appendRes = builtIndex.appendDescriptor(fakeDescriptor)
        self.assertTrue(appendRes.isOk)
        self.assertEqual(appendRes.value, sizeOfBatch)
        self.assertEqual(builtIndex.size(), sizeOfBatch + 1)
        self.assertEqual(builtIndex.countOfIndexedDescriptors(), sizeOfBatch + 1)

        # Search reference
        resRef = builtIndex.search(descriptor, searchResultSize)
        self.assertTrue(resRef[0].isOk)
        self.assertEqual(len(resRef[1]), searchResultSize)
        for i in range(len(resRef[1])):
            self.assertEqual(resRef[1][i].index, reference[i])

        # Search fake
        err, arr = builtIndex.search(fakeDescriptor, searchResultSize)
        self.assertTrue(err.isOk)
        # since we look for appended index, assume its found and is exactly the same
        self.assertEqual(arr[0].index, appendRes.value)
        self.assertEqual(arr[0].similarity, 1.0)
        self.assertEqual(arr[0].distance, 0.0)

    def testDynamicAppendBatch(self):
        faceEngine, descriptor, batch = load("descriptor1_46.bin", "batch46_eq1k.bin")
        builtIndex = buildAcquiredIndexWithBatch(faceEngine, batch)
        appendRes = builtIndex.appendBatch(batch)
        self.assertTrue(appendRes.isOk)
        # compare to sizeOfBatch because existing inside index batch is the same batch
        # so if append returns first index of inserted batch, it should return last position + 1
        # which equals size of batch
        self.assertEqual(appendRes.value, sizeOfBatch)
        self.assertEqual(builtIndex.size(), 2 * sizeOfBatch)
        self.assertEqual(builtIndex.countOfIndexedDescriptors(), 2 * sizeOfBatch)
        # Search in dynamic index: reference
        err, searchArray = builtIndex.search(descriptor, searchResultSize)
        self.assertTrue(err.isOk)
        size = len(searchArray)
        self.assertEqual(size, searchResultSize)
        for i in range(int(size / 2)):
            firstEqFirst = searchArray[2 * i].index == reference[i]
            firstEqSecond = searchArray[2 * i].index == reference[i] + sizeOfBatch
            secondEqFirst = searchArray[2 * i + 1].index == reference[i]
            secondEqSecond = searchArray[2 * i + 1].index == reference[i] + sizeOfBatch
            # ASSERT_NE(firstEqFirst && secondEqSecond, firstEqSecond && secondEqFirst);
            self.assertNotEqual(firstEqFirst and secondEqSecond, firstEqSecond and secondEqFirst)

    def testDynamicRemove(self):
        faceEngine, descriptor, batch = load("descriptor1_46.bin", "batch46_eq1k.bin")
        builtIndex = buildAcquiredIndexWithBatch(faceEngine, batch)
        self.assertEqual(builtIndex.size(), sizeOfBatch)
        self.assertEqual(builtIndex.countOfIndexedDescriptors(), sizeOfBatch)
        removeResult = builtIndex.removeDescriptor(reference[0])
        self.assertTrue(removeResult.isOk)
        # 	yep size does not shrink for implementation reasons
        self.assertEqual(builtIndex.size(), sizeOfBatch)
        self.assertEqual(builtIndex.countOfIndexedDescriptors(), sizeOfBatch - 1)

        # Search fake
        err, arr = builtIndex.search(descriptor, searchResultSize)
        self.assertTrue(err.isOk)
        size = len(arr)
        self.assertEqual(size, searchResultSize)

        # since we have removed first reference element, it should find everything but
        # this element
        for i in range(size - 1):
            self.assertEqual(arr[i].index, reference[i + 1])

    def testDynamicRemoveAll(self):
        faceEngine, descriptor, batch = load("descriptor1_46.bin", "batch46_eq1k.bin")
        builtIndex = buildAcquiredIndexWithBatch(faceEngine, batch)
        self.assertEqual(builtIndex.size(), sizeOfBatch)
        self.assertEqual(builtIndex.countOfIndexedDescriptors(), sizeOfBatch)
        # remove everything
        for i in range(sizeOfBatch):
            removeResult = builtIndex.removeDescriptor(i)
            self.assertTrue(removeResult.isOk)
            self.assertEqual(builtIndex.size(), sizeOfBatch)
            self.assertEqual(builtIndex.countOfIndexedDescriptors(), sizeOfBatch - (i + 1))
        # Search fake. should fail as index is empty
        err, arr = builtIndex.search(descriptor, searchResultSize)
        self.assertFalse(err.isOk)
        self.assertEqual(arr, [])

if __name__ == '__main__':
    unittest.main()

