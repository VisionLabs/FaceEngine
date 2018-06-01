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
    # table = faceEngine.createLSHTable(descriptor_batch)
    # descriptor, creating objects
    print("Creating descriptors")
    descriptor1 = faceEngine.createDescriptor()
    descriptor2 = faceEngine.createDescriptor()
    aggregation = faceEngine.createDescriptor()
    ext1 = extractor.extractFromWarpedImage(_image_list[0], descriptor1)
    ext2 = extractor.extractFromWarpedImage(_image_list[1], descriptor2)

    ext_batch1 = extractor.extractFromWarpedImageBatch(_image_list, descriptor_batch, aggregation, _batch_size)
    print("aggregation: ", aggregation.getModelVersion(), aggregation.getDescriptorLength())
    ext_batch2 = extractor.extractFromWarpedImageBatch(_image_list, descriptor_batch, _batch_size)

    print("Garbage score list1 = ", ext_batch1)
    print("Garbage score list2 = ", ext_batch2)
    print("Descriptor batch test after", descriptor_batch.getMaxCount(), descriptor_batch.getCount(),
          descriptor_batch.getModelVersion(), descriptor_batch.getDescriptorSize())
    return descriptor1, descriptor2, descriptor_batch


def matcher_example(_descriptor1, _descriptor2, _descriptor_batch):
    print("\nMatcher example")
    matcher = faceEngine.createMatcher()
    result1 = matcher.match(_descriptor1, _descriptor2)
    result2 = matcher.match(_descriptor1, _descriptor_batch)
    result3 = matcher.match(_descriptor1, _descriptor_batch, [0, 1])
    result4 = matcher.matchCompact(_descriptor1, _descriptor_batch, [1])
    print("Match result of different descriptors: {0}".format(result1.value))
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

def extractor_test_aggregation(version, use_mobile_net, cpu_type, device):
    print("Extractor_test_aggregation")
    config = fe.createSettingsProvider("data/faceengine.conf")
    configPath = config.getDefaultPath()
    print("Default path = ", configPath)
    config.setValue("DescriptorFactory::Settings", "model", fe.SettingsProviderValue(version))
    config.setValue("DescriptorFactory::Settings", "useMobileNet", fe.SettingsProviderValue(use_mobile_net))
    config.setValue("flower", "deviceClass", fe.SettingsProviderValue(device))
    config.setValue("system", "cpuClass", fe.SettingsProviderValue(cpu_type))
    config.setValue("system", "verboseLogging", fe.SettingsProviderValue(1))
    # config.setValue("QualityEstimator::Settings", "logGray", f.SettingsProviderValue(0.05, 3.3, 0.05, 0.012))
    faceEngine.setSettingsProvider(config)
    # val = config.getValue("QualityEstimator::Settings", "platt")
    # val = config.getValue("QualityEstimator::Settings", "expBlur")

    faceEngine.setSettingsProvider(config)
    val = config.getValue("MTCNNDetector::Settings", "scaleFactor")
    print(val.asFloat())

    warps = [fe.Image(), fe.Image()]

    warps[0].load("testData/warp1.ppm")
    warps[1].load("testData/warp2.ppm")
    batchSize = len(warps)
    descriptorExtractor = faceEngine.createExtractor()
    batch = faceEngine.createDescriptorBatch(batchSize)
    descriptor = faceEngine.createDescriptor()
    descriptor2 = faceEngine.createDescriptor()
    aggregation = faceEngine.createDescriptor()

    result1 = descriptorExtractor.extractFromWarpedImageBatch(warps, batch, aggregation, batchSize)
    print("GarbageScore: ", result1)
    result2 = descriptorExtractor.extractFromWarpedImage(warps[0], descriptor)
    result3 = descriptorExtractor.extractFromWarpedImage(warps[1], descriptor2)
    print("Result value 2: ", result2.value)
    print("Result value 3: ", result3.value)
    # print("Result2 value: ", result2.value)
    desc = descriptor.getDescriptor()

    # for i, element in enumerate(desc1):
    #     print(i, ")", desc[i], desc_from_batch[i])
    # print("Descriptors are equal {0}".format(are_equal(desc1, desc2)))

if __name__ == "__main__":
    batch_size = len(sys.argv) - 2
    # correct path or put directory "data" with example.py
    faceEngine = fe.createFaceEngine("data", "data/faceengine.conf")
    # more detailed description of config see in luna-sdk/doc/ConfigurationGuide.pdf
    set_logging(1)
    image_list = load_list_of_images(batch_size, sys.argv)
    try:
        (descriptor1, descriptor2, descriptor_batch) = extractor_example(image_list, batch_size)
        matcher_example(descriptor1, descriptor2, descriptor_batch)
        # print_descriptor(descriptor1)
        # print_descriptor(descriptor2)
        # as test
        extractor_test_aggregation(46, True, "cpu", "cpu")
    except RuntimeError:
        print("RuntimeError")
        exit(1)

