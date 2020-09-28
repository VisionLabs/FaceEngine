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

def load_images(images_dir_path, list_path):
    images_names_list = []
    images_list = []
    with open(list_path) as file:
        lines = file.readlines()
        for line in lines:
            line = line.strip()
            image_name = line
            image_path = images_dir_path + "/" + image_name
            image = fe.Image()
            err = image.load(image_path)
            if err.isError:
                print("Failed to load image: {0}".format(image_path))
            else:
                images_names_list.append(image_name)
                images_list.append(image)
        assert(len(images_names_list) == len(images_list))
    return images_names_list, images_list


def extract_descriptor(face_engine, warper, detector, descriptor_extractor, image, image_name):
    confidence_threshold = 0.25
    if not image.isValid():
        print("Request image is invalid.")
        return None
    image_bgr = fe.Image()
    if image.getFormat() == fe.FormatType.R8G8B8:
        image_bgr = image
    else:
        image_data = image.getData()
        image_bgr.setData(image_data, fe.FormatType.R8G8B8)
        if not image_bgr.isValid():
            print("Conversion to BGR has failed.")
            return None
    print("Detecting faces.")
    detection_count = 10

    detector_result, det_list = detector.detect(
        [image_bgr],
        [image_bgr.getRect()],
        detection_count,
        fe.DetectionType(fe.DT_LANDMARKS5))
    detections = []
    landmarks5l = []

    if detector_result.isError:
        print("Failed to detect face detection. Reason: {0}".format(detector_result.what()))
    for elem in det_list:
        for face in elem:
            detections.append(face.detection)
            if face.landmarks5_opt.isValid():
                landmarks5l.append(face.landmarks5_opt.value())
    detection_count = len(detections)
    if detection_count == 0:
        print("Faces is not found: \"{0}\".".format(image_name))
    print("Found {0} face(s).".format(detection_count))
    best_detection_index = -1
    best_score = -1.0
    for detection_index in range(detection_count):
        detection = detections[detection_index]
        print("Detecting facial features ({0}/{1})".format(detection_index + 1, detection_count))
        print(detection.score)
        if detection.score > best_score:
            best_score = detection.score
            best_detection_index = detection_index
    if best_score < confidence_threshold:
        print("Face detection succeeded, but no faces with good confidence found.")
        return None
    print("Best face confidence is {0}".format(best_score))
    print("Extracting descriptor.")
    try:
        descriptor = face_engine.createDescriptor()
    except:
        print("Failed to create face descrtiptor instance.")
        return None

    transformation = warper.createTransformation(detections[best_detection_index], landmarks5l[best_detection_index])
    warpResult, warpImage = warper.warp(image, transformation)
    if warpResult.isError:
        print("Failed to warp image!")

    extractor_result, _ = descriptor_extractor.extractFromWarpedImage(warpImage, descriptor)

    if extractor_result.isError:
        print("Failed to extract face descriptor. Reason: {0}".format(extractor_result.what()))

    return descriptor


class Progress:
    ok = False
    batch_start = 0
    external_batch_start = 0


def do_index_stuff(get_index, search, report, completion=None):
    index = get_index()

    if not Progress.ok:
        print("Failed to get index")
        return False

    results = search(index)
    if not Progress.ok:
        print("Failed to search index")
        return False

    report(results)

    if completion:
        completion(index)
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
    print("This example is only for server platforms!")
    image_path = sys.argv[2]
    images_dir_path = sys.argv[3]
    list_path = sys.argv[4]
    threshold = float(sys.argv[5])

    print("imagePath: {0}\n,"
          "imagesDirPath: {1}\n"
          "listPath: {2}\n"
          "threshold: {3}\n", image_path, images_dir_path, list_path, threshold)
    # correct path or put directory "data" with example.py
    # Create FaceEngine root SDK object.
    face_engine = fe.createFaceEngine("data")
    if not make_activation(face_engine):
        print("failed to activate license!")
        exit(-1)
    if face_engine.getFaceEngineEdition() != fe.CompleteEdition:
        print("FaceEngine SDK Frontend edition doesn't support face descriptors. Use FaceEngine SDK Complete edition")
        exit(-1)
    
    warper = face_engine.createWarper()
    
    def searcher(_index):
        max_res_count = 20
        err, search_list = _index.search(descriptor, max_res_count)
        if err.isError:
            print("Failed to search")
            Progress.ok = False
            return
        Progress.ok = True
        return search_list

    def reporter(results):
        for j, _ in enumerate(results):
            print("Images: \"{0}\" and \"{1}\" matched with score: \"{2}\" ".format(image_path, added_images[results[j].index - Progress.batch_start], results[j].similarity * 100.0))
            s = "Images: \"{0}\", and \"{1}\"".format(image_path, added_images[results[j].index - Progress.batch_start])
            if results[j].similarity > threshold:
                print(s + "belong to one person.")
            else:
                print(s + "belong to different persons.")

    detector = face_engine.createDetector(fe.FACE_DET_V1)
    descriptor_extractor = face_engine.createExtractor()
    images_names_list, images_list = load_images(images_dir_path, list_path)
    if not images_names_list or not images_list:
        print("Failed to load images.")
        exit(-1)

    print("Creating descriptor batch.")
    # Extract faces descriptors.
    descriptor_batch = face_engine.createDescriptorBatch(len(images_list))

    added_images = []
    for i, image in enumerate(images_list):
        descriptor = extract_descriptor(face_engine, warper, detector, descriptor_extractor, image, images_names_list[i])
        if descriptor is None:
            continue
        descriptorBatchAddResult = descriptor_batch.add(descriptor)

        if descriptorBatchAddResult.isError:
            print("Failed to add descriptor to descriptor batch.")
            exit(-1)
        added_images.append(images_names_list[i])

    print("\nImages with EXTRACTED DESCRIPTORS: ")
    for im in added_images:
        print(im)

    image = fe.Image()
    if image.load(image_path).isError:
        print("Failed to load image:{0}".format())

    # Extract face descriptor of ref image.
    descriptor = extract_descriptor(face_engine, warper, detector, descriptor_extractor, image, image_path)
    if descriptor is None:
        print("Failed to extract descriptor, from {0}".format(image_path))
        exit(1)
    # Create Index Builder, build index, search it, print results, deserialize
    dense_index_path = images_dir_path + "/index.dense.tmp"
    dynamic_index_path = images_dir_path + "/index.dynamic.tmp"
    Progress.external_batch_start = 0

    print("\nBUILDER PIPELINE: ")


    def get_index():
        index_builder = face_engine.createIndexBuilder()
        #  Put batch into builder
        append_result = index_builder.appendBatch(descriptor_batch)
        print(append_result)
        if append_result.isError:
            print("Failed to append batch to builder")
            Progress.ok = False
            return None
        # index of first batch element. might be used to query/remove descriptors
        Progress.batch_start = append_result.value
        # since we cant get batch start on deserialization, but on append
        # we have to save it here
        Progress.external_batch_start = Progress.batch_start
        err, index = index_builder.buildIndex()
        if err.isError:
            print("Failed to build index")
            Progress.ok = False
            return None

        Progress.ok = True
        return index

    # pass only dynamic index
    def deserialize(index):
        ser_dense_result = index.saveToDenseIndex(dense_index_path)
        if ser_dense_result.isError:
            print("Failed to serialized as dense")
            Progress.ok = False
            return
        ser_dynamic_result = index.saveToDynamicIndex(dynamic_index_path)
        if ser_dynamic_result.isError:
            print("Failed to serialized as dynamic")
            Progress.ok = False
            return

        Progress.ok = True
        return

    Progress.ok = False
    Progress.ok = do_index_stuff(get_index, searcher, reporter, deserialize)
    if not Progress.ok:
        print("Failed to do index builder pipeline")
        exit(-1)

    # Deserialize dense index, search in it
    print("\nDENSE INDEX PIPELINE: ")

    # Load dense index
    def dense_index_loader():
        err, index = face_engine.loadDenseIndex(dense_index_path)
        if err.isError:
            print("Failed to deser dense index")
            Progress.ok = False
            return None
        Progress.batch_start = Progress.external_batch_start
        Progress.ok = True
        return index

    if not do_index_stuff(dense_index_loader, searcher, reporter):
        print("Failed to do dense index pipeline")
        exit(-1)

    print("\nDYNAMIC INDEX PIPELINE: ")

    # Load dynamic index
    def dynamic_index_loader():
        err, index = face_engine.loadDynamicIndex(dynamic_index_path)
        if err.isError:
            print("Failed to deser dynamic index")
            Progress.ok = False
            return None

        Progress.batch_start = Progress.external_batch_start
        Progress.ok = True
        return index

    if not do_index_stuff(dynamic_index_loader, searcher, reporter, deserialize):
        print("Failed to do dense index pipeline")