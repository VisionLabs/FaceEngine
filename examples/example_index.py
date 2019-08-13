import sys
from example_license import make_activation


def help():
    print("Usage: python", sys.argv[1], " <path to dir with FaceEngine*.so> <image> <imagesDir> <list> <threshold>\n"
    " *image - path to image\n"
    " *imagesDir - path to images directory\n"
    " *list - path to images names list\n"
    " *threshold - similarity threshold in range (0..1]\n")


if len(sys.argv) != 6:
    help()
    exit(1)

# pass path dir with FaceEngine*.so and add it to system directory
sys.path.append(sys.argv[1])
import FaceEngine as fe

configPath = "data" + "/faceengine.conf"
testDataPath = "testData"


def loadImages(_imagesDirPath, _listPath):
    imagesNamesList = []
    imagesList = []
    with open(_listPath) as file:
        lines = file.readlines()
        for line in lines:
            line = line.strip()
            imageName = line
            imagePath = _imagesDirPath + "/" + imageName
            image = fe.Image()
            err = image.load(imagePath)
            if err.isError:
                print("Failed to load image: {0}".format(imagePath))
            else:
                imagesNamesList.append(imageName)
                imagesList.append(image)
        assert(len(imagesNamesList) == len(imagesList))
    return imagesNamesList, imagesList


def extractDescriptor(_faceEngine, _detector, _descriptorExtractor, image, image_name):
    confidenceThreshold = 0.25
    if not image.isValid():
        print("Request image is invalid.")
        return None
    imageBGR = fe.Image()
    if image.getFormat() == fe.FormatType.R8G8B8:
        imageBGR = image
    else:
        imageData = image.getData()
        imageBGR.setData(imageData, fe.FormatType.R8G8B8)
        if not imageBGR.isValid():
            print("Conversion to BGR has failed.")
            return None
    print("Detecting faces.")
    detectionsCount = 10

    detectorResult, det_list = _detector.detect([imageBGR], [imageBGR.getRect()], detectionsCount, fe.DetectionType(fe.dt5Landmarks))
    detections = []
    landmarks5l = []

    if detectorResult.isError:
        print("Failed to detect face detection. Reason: {0}".format(detectorResult.what()))
    for elem in det_list:
        for elem_elem in elem:
            detections.append(elem_elem.detection)
            if elem_elem.landmarks5_opt.isValid():
                landmarks5l.append(elem_elem.landmarks5_opt.value())
    detectionsCount = len(detections)
    if detectionsCount == 0:
        print("Faces is not found: \"{0}\".".format(image_name))
    print("Found {0} face(s).".format(detectionsCount))
    bestDetectionIndex = -1
    bestScore = -1.0
    for detectionIndex in range(detectionsCount):
        detection = detections[detectionIndex]
        print("Detecting facial features ({0}/{1})".format(detectionIndex + 1, detectionsCount))
        print(detection.score)
        if detection.score > bestScore:
            bestScore = detection.score
            bestDetectionIndex = detectionIndex
    if bestScore < confidenceThreshold:
        print("Face detection succeeded, but no faces with good confidence found.")
        return None
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
    if extractorResult.isError:
        print("Failed to extract face descriptor. Reason: {0}".format(extractorResult.what()))

    return descriptor


class Progress:
    ok = False
    batchStart = 0
    externalBatchStart = 0


def doIndexStuff(getIndex, _search, _report, _completion=None):
    index = getIndex()

    if not Progress.ok:
        print("Failed to get index")
        return False

    results = _search(index)
    if not Progress.ok:
        print("Failed to search index")
        return False

    _report(results)

    if _completion:
        _completion(index)
        if not Progress.ok:
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


if __name__ == "__main__":
    imagePath = sys.argv[2]
    imagesDirPath = sys.argv[3]
    listPath = sys.argv[4]
    threshold = float(sys.argv[5])

    print("imagePath: {0}\n,"
          "imagesDirPath: {1}\n"
          "listPath: {2}\n"
          "threshold: {3}\n", imagePath, imagesDirPath, listPath, threshold)
    # correct path or put directory "data" with example.py
    # Create FaceEngine root SDK object.
    faceEngine = fe.createFaceEngine("data",
                                     "data/faceengine.conf")
    if not make_activation(faceEngine):
        print("failed to activate license!")
        exit(-1)
    if faceEngine.getFaceEngineEdition() != fe.CompleteEdition:
        print("FaceEngine SDK Frontend edition doesn't support face descriptors. Use FaceEngine SDK Complete edition")
        exit(-1)

    def searcher(_index):
        maxResCount = 20
        err, searchList = _index.search(descriptor, maxResCount)
        if err.isError:
            print("Failed to search")
            Progress.ok = False
            return
        Progress.ok = True
        return searchList

    def reporter(results):
        for j, _ in enumerate(results):
            print("Images: \"{0}\" and \"{1}\" matched with score: \"{2}\" ".format(imagePath, addedImages[results[j].index - Progress.batchStart], results[j].similarity * 100.0))
            s = "Images: \"{0}\", and \"{1}\"".format(imagePath, addedImages[results[j].index - Progress.batchStart])
            if results[j].similarity > threshold:
                print(s + "belong to one person.")
            else:
                print(s + "belong to different persons.")

    detector = faceEngine.createDetector(fe.FACE_DET_V1)
    descriptorExtractor = faceEngine.createExtractor()
    imagesNamesList, imagesList = loadImages(imagesDirPath, listPath)
    if not imagesNamesList or not imagesList:
        print("Failed to load images.")
        exit(-1)

    print("Creating descriptor batch.")
    # Extract faces descriptors.
    descriptorBatch = faceEngine.createDescriptorBatch(len(imagesList))

    addedImages = []
    for i, image in enumerate(imagesList):
        descriptor = extractDescriptor(faceEngine, detector, descriptorExtractor, image, imagesNamesList[i])
        if descriptor is None:
            continue
        descriptorBatchAddResult = descriptorBatch.add(descriptor)

        if descriptorBatchAddResult.isError:
            print("Failed to add descriptor to descriptor batch.")
            exit(-1)
        addedImages.append(imagesNamesList[i])

    print("\nImages with EXTRACTED DESCRIPTORS: ")
    for im in addedImages:
        print(im)

    image = fe.Image()
    if image.load(imagePath).isError:
        print("Failed to load image:{0}".format())

    # Extract face descriptor of ref image.
    descriptor = extractDescriptor(faceEngine, detector, descriptorExtractor, image, imagePath)

    # Create Index Builder, build index, search it, print results, deserialize
    denseIndexPath = imagesDirPath + "/index.dense.tmp"
    dynamicIndexPath = imagesDirPath + "/index.dynamic.tmp"
    Progress.externalBatchStart = 0

    print("\nBUILDER PIPELINE: ")


    def getIndex():
        indexBuilder = faceEngine.createIndexBuilder()
        #  Put batch into builder
        appendResult = indexBuilder.appendBatch(descriptorBatch)
        print(appendResult)
        if appendResult.isError:
            print("Failed to append batch to builder")
            Progress.ok = False
            return None
        # index of first batch element. might be used to query/remove descriptors
        Progress.batchStart = appendResult.value
        # since we cant get batch start on deserialization, but on append
        # we have to save it here
        Progress.externalBatchStart = Progress.batchStart
        err, index = indexBuilder.buildIndex()
        if err.isError:
            print("Failed to build index")
            Progress.ok = False
            return None

        Progress.ok = True
        return index


    # pass only dynamic index
    def deserialize(index):
        serDenseResult = index.saveToDenseIndex(denseIndexPath)
        if serDenseResult.isError:
            print("Failed to serialized as dense")
            Progress.ok = False
            return
        serDynamicResult = index.saveToDynamicIndex(dynamicIndexPath)
        if serDynamicResult.isError:
            print("Failed to serialized as dynamic")
            Progress.ok = False
            return

        Progress.ok = True
        return

    Progress.ok = False
    Progress.ok = doIndexStuff(getIndex, searcher, reporter, deserialize)
    if not Progress.ok:
        print("Failed to do index builder pipeline")
        exit(-1)

    # Deserialize dense index, search in it
    print("\nDENSE INDEX PIPELINE: ")

    # Load dense index
    def denseIndexLoader():
        err, index = faceEngine.loadDenseIndex(denseIndexPath)
        if err.isError:
            print("Failed to deser dense index")
            Progress.ok = False
            return None
        Progress.batchStart = Progress.externalBatchStart
        Progress.ok = True
        return index

    if not doIndexStuff(denseIndexLoader, searcher, reporter):
        print("Failed to do dense index pipeline")
        exit(-1)

    print("\nDYNAMIC INDEX PIPELINE: ")

    # Load dynamic index
    def dynamicIndexLoader():
        err, index = faceEngine.loadDynamicIndex(dynamicIndexPath)
        if err.isError:
            print("Failed to deser dynamic index")
            Progress.ok = False
            return None

        Progress.batchStart = Progress.externalBatchStart
        Progress.ok = True
        return index

    if not doIndexStuff(dynamicIndexLoader, searcher, reporter, deserialize):
        print("Failed to do dense index pipeline")