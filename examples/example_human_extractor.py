import sys


def help():
    print("example_human_extractor.py <path to dir with FaceEngine*.so> <path to image> <path to image> ...")

if len(sys.argv) <= 3:
    help()
    exit(1)

# if FaceEngine is not installed pass the path to dir with FaceEngine*.so and add it to system paths
sys.path.append(sys.argv[1])
import FaceEngine as fe
from example_license import make_activation


def simple_human_extractor_example(all_warps):
    extractor = face_engine.createExtractor(fe.DV_MIN_HUMAN_DESCRIPTOR_VERSION)
    all_descriptors = []
    for i_warp_list, warp_list in enumerate(all_warps):
        descriptor_list = []
        for j_warp, warp in enumerate(warp_list):
            descriptor = face_engine.createDescriptor(fe.DV_MIN_HUMAN_DESCRIPTOR_VERSION)
            res, _ = extractor.extractFromWarpedImage(warp, descriptor)
            if res.isError:
                print("Failed to extract descriptor, reason: {0}, list number={1}, image number={2}".format(res.what, i_warp_list, j_warp))
                continue
            descriptor_list.append(descriptor)
        all_descriptors.append(descriptor_list)
    return all_descriptors


def human_detect_warp_example(images):
    detector = face_engine.createHumanDetector()
    rects = []
    for image in image_list:
        rects.append(image.getRect())
    result, all_humans = detector.detect(
        images,
        rects,
        10,
        fe.HumanDetectionType(fe.DCT_BOX))
    if not result.isOk:
        print("human_detect_example - failed to detect! Reason: {0}".format(result[0].what))
        return []
    all_warps = []
    human_warper = face_engine.createHumanWarper()
    for i_human_list, human_list in enumerate(all_humans):
        warps_list = []
        if len(human_list) == 0:
            print("human_detect_warp_example - no human on the image {0}!".format(i_human_list))
        else:
            for human in human_list:
                err_human, human_warp = human_warper.warp(human)
                if err_human.isError or not human_warp.isValid():
                    print("failed to warp {0}. Reason: {1}", human, err_human.what)
                    continue
                warps_list.append(human_warp)
        all_warps.append(warps_list)

    return all_warps


def matcher_example(descriptors):
    matcher = face_engine.createMatcher(fe.DV_MIN_HUMAN_DESCRIPTOR_VERSION)
    pairs = dict()
    if len(descriptors[0]) == 0:
        return pairs
    # for example we take only first detection of first image as current
    current_descriptor = descriptors[0][0]
    for i in range(1, len(descriptors)):
        for j in range(len(descriptors[i])):
            err, value = matcher.match(current_descriptor, descriptors[i][j])
            if err.isError:
                print("Failed to match descriptors, reason {0}, list number={1}, image number={2}".format(err.what, i, j))
                continue
            pairs[value.similarity] = (i, j)

    return pairs


def load_list_of_images(size, sys_argv):
    image_list = []
    for i in range(size):
        image = fe.Image()
        print("Loading image {0}".format(sys_argv[i + 2]))
        err_load = image.load(sys_argv[i + 2])
        if err_load.isError or not image.isValid():
            print("Failed to load image: {0} . Reason: {1}".format(sys_argv[i + 2]), err_load.what)
            exit(1)
        image_list.append(image)
    return image_list


if __name__ == "__main__":

    try:
        print("This example is only for server platforms!")
        n_images = len(sys.argv) - 2
        # correct path or put directory "data" with example*.py
        face_engine = fe.createFaceEngine("data")
        if not make_activation(face_engine):
            print("failed to activate license!")
            exit(-1)
        image_list = load_list_of_images(n_images, sys.argv)
        warps = human_detect_warp_example(image_list)
        descriptors = simple_human_extractor_example(warps)
        pairs = matcher_example(descriptors)

        print("\nSimilarity of first descriptor and all human descriptors in other images in descent order:")
        for key in sorted(pairs.keys(), reverse=True):
            print(key, " :: ", pairs[key])
    except RuntimeError as ex:
        print(ex)
        exit(1)

