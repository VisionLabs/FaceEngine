import sys


def help():
    print("example_extractor_matcher.py <path to FaceEngine*.so> <path to image> <path to image> ...")
    print("example of using: python3 example_extractor_matcher.py build images/warp1.ppm images/warp2.ppm "
          "images/photo_2017-03-30_14-47-43_p.ppm")

if len(sys.argv) <= 3:
    help()
    exit(1)

# if FaceEngine is not installed pass the path to dir with FaceEngine*.so and add it to system paths
sys.path.append(sys.argv[1])
import FaceEngine as fe

def extractor_example(_image_list, _batch_size):
    print("Batch descriptor example")
    assert(len(_image_list) == _batch_size)
    print("batchSize {0}".format(_batch_size))
    descriptor_batch = faceEngine.createDescriptorBatch(_batch_size)
    extractor = faceEngine.createExtractor()
    print(extractor)
    # table = faceEngine.createLSHTable(descriptor_batch)
    # descriptor, creating objects
    print("Creating descriptors")
    descriptor1 = faceEngine.createDescriptor()
    descriptor2 = faceEngine.createDescriptor()
    print(descriptor1, descriptor2)
    aggregation = faceEngine.createDescriptor()
    print("image_list = ", _image_list)
    print(type(extractor))
    print("Descriptor test befor = ", descriptor1.getModelVersion(), descriptor1.getDescriptorLength())
    ext1 = extractor.extractFromWarpedImage(_image_list[0], descriptor1)
    ext2 = extractor.extractFromWarpedImage(_image_list[1], descriptor2)
    print("Descriptor test after = ", descriptor1.getModelVersion(), descriptor1.getDescriptorLength())
    print("extractor result =", ext1)
    print("extractor result =", ext2)

    print("Descriptor batch test befor: ", descriptor_batch.getMaxCount(), descriptor_batch.getCount(),
          descriptor_batch.getModelVersion(), descriptor_batch.getDescriptorSize())
    ext_batch1 = extractor.extractFromWarpedImageBatch(_image_list, descriptor_batch, aggregation, _batch_size)
    print("aggregation: ", aggregation.getModelVersion(), aggregation.getDescriptorLength())
    ext_batch2 = extractor.extractFromWarpedImageBatch(_image_list, descriptor_batch, _batch_size)

    print("Garbage score list1 = ", ext_batch1)
    print("Garbage score list2 = ", ext_batch2)
    print("Descriptor batch test after", descriptor_batch.getMaxCount(), descriptor_batch.getCount(),
          descriptor_batch.getModelVersion(), descriptor_batch.getDescriptorSize())
    print(descriptor1)
    print(descriptor_batch)
    print(extractor)
    print(table)
    return descriptor1, descriptor2, descriptor_batch


def matcher_example(_descriptor1, _descriptor2, _descriptor_batch):
    print("\nMatcher example")
    matcher = faceEngine.createMatcher()
    print(matcher)
    result1 = matcher.match(_descriptor1, _descriptor2)
    result2 = matcher.match(_descriptor1, _descriptor_batch)
    result3 = matcher.match(_descriptor1, _descriptor_batch, [0, 1])
    result4 = matcher.matchCompact(_descriptor1, _descriptor_batch, [1])
    print("Match result of different descriptors: {0}".format(result1))
    print("Match result of descriptor and descriptor batch: {0}".format(result2))
    print("Match result of descriptor and 2 descriptors from batch: {0}".format(result3))
    print("MatchCompact result of descriptors: {0}".format(result3))


def load_list_of_images(_batch_size, _sys_argv):
    image_list = []
    for i in range(_batch_size):
        image = fe.Image()
        print("Adding image {0}".format(_sys_argv[i + 2]))
        image.load(_sys_argv[i + 2])
        if not image.isValid():
            print("Failed list creating: one of images is not found {0}".format(_sys_argv[i + 2]))
            exit(1)
        image_list.append(image)
    return image_list


def print_descriptor(descriptor):
    py_list_descriptor = descriptor.getDescriptor()
    for item_descriptor in py_list_descriptor:
        print(item_descriptor)


def set_logging(value):
    config = fe.createSettingsProvider("data/faceengine.conf")
    configPath = config.getDefaultPath()
    print("Config settings: DefaultPath {0}".format(configPath))
    config.setValue("system", "verboseLogging", fe.SettingsProviderValue(value))
    faceEngine.setSettingsProvider(config)
    val = config.getValue("system", "verboseLogging")
    print("Config settings: \"system\", \"verboseLogging\" = {0}".format(val.asInt()))


if __name__ == "__main__":
    batch_size = len(sys.argv) - 2
    # correct path or put directory "data" with example.py
    faceEngine = fe.createFaceEngine("data", "data/faceengine.conf")
    # more detailed about config see luna-sdk/doc/ConfigurationGuide.pdf
    set_logging(1)
    image_list = load_list_of_images(batch_size, sys.argv)
    (descriptor1, descriptor2, descriptor_batch) = extractor_example(image_list, batch_size)
    matcher_example(descriptor1, descriptor2, descriptor_batch)
    # print_descriptor(descriptor1)
