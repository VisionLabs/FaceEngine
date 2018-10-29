import sys
# import numpy as np
# from matplotlib import pyplot as plt

def help():
    print("Usage: python", sys.argv[1], " <path to FaceEngine*.so> <image> <imagesDir> <list> <threshold>\n"
    " *image - path to image\n"
    " *imagesDir - path to images directory\n"
    " *list - path to images names list\n"
    " *threshold - similarity threshold in range (0..1]\n")

# if len(sys.argv) != 6:
#     help()
#     exit(1)

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

def loadImages(_imagesDirPath, _listPath):
    imagesNamesList = []
    imagesList = []
    with open(_listPath) as file:
        lines = file.read().split()
        for line in lines:
            line = line.strip()
            print(line)
            imageName = line
            imagePath = _imagesDirPath + "/" + imageName
            image = fe.Image()
            err = image.load(imagePath)
            print(err)
            if not err.isOk:
                print("Failed to load image: {0}".format(imagePath))
            else:
                imagesNamesList.append(imageName)
                imagesList.append(image)
        assert(len(imagesNamesList) == len(imagesList))
    return imagesNamesList, imagesList

def extractDescriptor(_faceEngine, _detector, _descriptorExtractor, image):
    confidenceThreshold = 0.25
    if image.isValid():
        print("Request image is invalid.")
        return None
    imageBGR = fe.Image()
    imageData = image.getData()
    imageBGR.setData(imageData, fe.Format(fe.B8G8R8))
    if not imageBGR.isValid():
        print("Conversion to BGR has failed.")
        return None
    print("Detecting faces.")
    detectionsCount = 10

    detectorResult, det_list = _detector.detect5(image, image.getRect(), detectionsCount)
    detections = []
    landmarks5l = []

    for elem in det_list:
        detections.append(elem[0])
        landmarks5l.append(elem[0])
    if detectorResult.isError:
        print("Failed to detect face detection. Reason: {0}".foramt(detectorResult.what()))
    detectionsCount = len(detections)
    if detectionsCount == 0:
        print("Faces is not found.")
    print("Found {0} face(s).".format(detectionsCount))
    bestDetectionIndex = -1
    bestScore = -1.0
    for detectionIndex in range(detectionsCount):
        detection = detections[detectionIndex]
        print("Detecting facial features ({0}/{1})".format(detectionIndex + 1, detectionsCount))
        if detection.score > bestScore:
            bestScore = detection.score
            bestDetectionIndex = detectionIndex
    if bestScore < confidenceThreshold:
        print("Face detection succeeded, but no faces with good confidence found.")
    print("Best face confidence is {0}".format(bestScore))
    print("Extracting descriptor.")
    try:
        descriptor = _faceEngine.createDescriptor()
    except:
        print("Failed to create face descrtiptor instance.")
        return None
    # Extract face descriptor.
    # This is typically the most time-consuming task.
    extractorResult = _descriptorExtractor.extract(
        image,
        detections[bestDetectionIndex],
        landmarks5l[bestDetectionIndex],
        descriptor)
    data = descriptor.getDescriptor()

    # MSD out desc
    for i in data:
        print(i)

    if extractorResult.isError:
        print("Failed to extract face descriptor. Reason: {0}".format(extractorResult.what()))

    return descriptor

def doIndexStuff(getIndex, search, report, completion):
    ok = False
    batchStart = 0
    index = getIndex(batchStart, ok)

    if not ok:
        print("Failed to get index")
        return False

    results = search(index, ok)

    if not ok:
        print("Failed to search index")
        return False

    report(results, batchStart)

    if completion:
        completion(index, ok)
        if not ok:
            print("Failed to complete")
            return False
    return True
    # index building module usage example
    # IIndexBuilder lets you create indexed data structure out of appended descriptors.
    # This data structure is optimized for extremely fast nearest neighbors search.
    # This data structure has IDynamicIndex interface. You can serialize it as
    # Dense or Dynamic indexes into file. For difference of these types of serialization
    # refer to FaceEngineSDKHandbook.

    # Parse command line arguments.
    # Arguments:
    # 1) path to a image,
    # 2) path to a images directory,
    # 3) path to a images names list,
    # 4) matching threshold.
    # If matching score is above the threshold, then both images
    # belong to the same person, otherwise they belong to different persons.
    # Images should be in ppm format.                                                                                                                                       // Images should be in ppm format.


if __name__ == "__main__":

    # imagePath = sys.argv[2]
    # imagesDirPath = sys.argv[3]
    # listPath = sys.argv[4]
    # threshold = float(sys.argv[5])

    # print("imagePath: {0}\n,"
    #       "imagesDirPath: {1}\n"
    #       "listPath: {2}\n"
    #       "threshold: {3}\n", imagePath, imagesDirPath, listPath, threshold)
    # correct path or put directory "data" with example.py
    faceEngine = fe.createFaceEngine("data",
                                     "data/faceengine.conf")
    if faceEngine.getFaceEngineEdition() != fe.CompleteEdition:
        print("FaceEngine SDK Frontend edition doesn't support face descriptors. Use FaceEngine SDK Complete edition")
        exit(-1)

    detector = faceEngine.createDetector(fe.ODT_MTCNN)
    descriptorExtractor = faceEngine.createExtractor()
    imagesNamesList, imagesList = loadImages(imagesDirPath, listPath)
    if not imagesNamesList or not imagesList:
        print("Failed to load images.")
        exit(-1)

    print("Creating descriptor batch.")
    # Extract faces descriptors.
    descriptorBatch = faceEngine.createDescriptorBatch(len(imagesList))
    for image in imagesList:
        descriptor = extractDescriptor(faceEngine, detector, descriptorExtractor, image)
    if descriptor.getData():
        exit(-1)
    descriptorBatchAddResult = descriptorBatch.add(descriptor)
    if descriptorBatchAddResult.isError():
        print("Failed to add descriptor to descriptor batch.")
        exit(-1)

    image = fe.Image()
    if not image.load(imagePath).isOk:
        print("Failed to load image:{0}".format())

    # Extract face descriptor.
    descriptor = extractDescriptor(faceEngine, detector, descriptorExtractor, image)

    if descriptor.getData():
        exit(-1)

    def reporter(imagePath, results, batchStart, threshold):
        for j in len(results):
            print("Images: \"{0}\" and \"{1}\" matched with score: \"{2}\"".format(imagePath, imagesNamesList[results[j].index - batchStart], results[j].similarity * 100.0))
            s = "Images: \"{0}\", and \"{1}\"".format(imagePath,
                  imagesNamesList[results[j].index - batchStart],
                  )
            if results[j].similarity > threshold:
                print(s + "belong to one person.")
            else:
                print(s + "belong to different persons.")

    def searcher(index, ok):
        maxResCount = 10
        err, arr = index.search(descriptor, maxResCount)
        if err.isError:
            print("Failed to search")
            ok = False
            return
        resCount = err.value
        ok = True
        return arr

    # Create Index Builder, build index, search it, print results, deserialize
    denseIndexPath = imagesDirPath + "/index.dense.tmp"
    dynamicIndexPath = imagesDirPath + "/index.dynamic.tmp"
    externalBatchStart = 0



    # def deserializeDenseIndexSearch(denseIndexPath, ):

    print("BUILDER PIPELINE: ")

    def getIndex():
        indexBuilder = faceEngine.createIndexBuilder()
        #  Put batch into builder
        appendResult = indexBuilder.appendBatch(descriptorBatch)
        if appendResult.isError:
            print("Failed to append batch to builder")
            ok = False
            return None
        # index of first batch element. might be used to query/remove descriptors
        batchStart = appendResult.value
        # since we cant get batch start on deserialization, but on append
        # we have to save it here
        externalBatchStart = batchStart
        err, index = indexBuilder.buildIndex()
        if err.isError:
            print("Failed to build index")
            ok = False
            return None

        ok = True
        return index


    def deserialize(index, ok):
        serDenseResult = index.saveToDenseIndex(denseIndexPath)
        if serDenseResult.isError:
            print("Failed to serialized as dense")
            ok = False
            return

        serDynamicResult = index.saveToDynamicIndex(dynamicIndexPath)
        if serDynamicResult.isError:
            print("Failed to serialized as dynamic")
            ok = False
            return

        ok = True
        return


    if not doIndexStuff(getIndex, searcher, reporter, deserialize):
        print("Failed to do index builder pipeline")
        exit(-1)

    # Deserialize dense index, search in it
    print("DENSE INDEX PIPELINE: ")

    # Load dense index
    def denseIndexLoader(batchStart, ok):
        err, index = faceEngine.loadDenseIndex(denseIndexPath)
        if err.isError:
            print("Failed to deser dense index")
            ok = False
            return None
        batchStart = externalBatchStart
        ok = True
        return index

    if not doIndexStuff(denseIndexLoader, searcher, reporter):
        print("Failed to do dense index pipeline")
        exit(-1)

    print("DYNAMIC INDEX PIPELINE: ")

    def dynamicIndexLoader(batchStart, ok):
        err, index = faceEngine.loadDynamicIndex(dynamicIndexPath)
        if err.isError:
            print("Failed to deserilize dynamic index")
            ok = False
            return None

        batchStart = externalBatchStart
        ok = True
        return index

    if not doIndexStuff(dynamicIndexLoader, searcher, reporter):
        print("Failed to do dense index pipeline")






    faceEngine.createIndexBuilder()

    faceEngine = loadAcquiredFaceEngineWithCnn46()
    descriptor = loadAcquiredDescriptor(faceEngine, testDataPath + "/descriptor1_46.bin")
    batch = loadAcquiredBatch(faceEngine, testDataPath + "/batch46_eq1k.bin")
    countOfRemovals = 5
    indexesToRemove = [459, 245, 651, 832, 634]