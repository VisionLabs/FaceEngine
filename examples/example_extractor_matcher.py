import sys


def help():
    print("example_extractor_matcher.py <path to dir with FaceEngine*.so> <path to image> <path to image> ...")
    print("example of using: python3 example_extractor_matcher.py build images/warp1.ppm images/warp2.ppm "
          "images/photo_2017-03-30_14-47-43_p.ppm")

if len(sys.argv) <= 3:
    help()
    exit(1)

# if FaceEngine is not installed pass the path to dir with FaceEngine*.so and add it to system paths
sys.path.append(sys.argv[1])
import FaceEngine as fe
from example_license import make_activation


def extractor_example(image_list, batch_size):
    print("Batch descriptor example")
    assert(len(image_list) == batch_size)
    print("BatchSize: {0}".format(batch_size))
    descriptor_batch = face_engine.createDescriptorBatch(batch_size)
    extractor = face_engine.createExtractor()
    print("Creating descriptors")
    descriptor1 = face_engine.createDescriptor()
    descriptor2 = face_engine.createDescriptor()
    aggregation = face_engine.createDescriptor()

    res1, _ = extractor.extractFromWarpedImage(image_list[0], descriptor1)
    if res1.isError:
        print("Failed to extract descriptor1, reason: ", res1.what)
        exit(-1)

    res2, _ = extractor.extractFromWarpedImage(image_list[1], descriptor2)
    if res2.isError:
        print("Failed to extract descriptor2, reason: ", res2.what)
        exit(-1)

    res_batch1, aggregated_garbage_score, value_batch_1 = extractor.extractFromWarpedImageBatch(image_list, descriptor_batch, aggregation, batch_size)
    if res_batch1.isError:
        print("Failed to extract batch descriptor1, reason: ", res_batch1.what)
        exit(-1)

    print("aggregation: version = {0}, desciptor size = {1}".format(aggregation.getModelVersion(), aggregation.getDescriptorLength()))
    print("Garbage score list1:", value_batch_1)
    res_batch2, value_batch_2 = extractor.extractFromWarpedImageBatch(image_list, descriptor_batch, batch_size)
    if res_batch2.isError:
        print("Failed to extract batch descriptor2, reason: ", res_batch2.what)
        exit(-1)
    print("Garbage score list2:", value_batch_2)
    print("Descriptor batch test: count = {0}, version = {1}, desciptor size = {2}".format(descriptor_batch.getCount(),
          descriptor_batch.getModelVersion(), descriptor_batch.getDescriptorSize()))
    return descriptor1, descriptor2, descriptor_batch


def matcher_example(descriptor1, descriptor2, descriptor_batch):
    print("\nMatcher example")
    matcher = face_engine.createMatcher()

    err1, value1 = matcher.match(descriptor1, descriptor2)
    if err1.isError:
        print("Failed to match descriptors, reason", err1.what)
        exit(-1)
    print("Matching result of different descriptors: {0}".format(value1))

    err2, value2 = matcher.match(descriptor1, descriptor_batch)
    if err2.isError:
        print("Failed to match descriptors, reason", err2.what)
        exit(-1)
    print("Matching result of descriptor and descriptor batch: {0}".format(value2))


def load_list_of_images(batch_size, sys_argv):
    image_list = []
    for i in range(batch_size):
        image = fe.Image()
        print("Adding image {0}".format(sys_argv[i + 2]))
        image.load(sys_argv[i + 2])
        if not image.isValid():
            print("Failed list creating: one of images is not found {0}".format(sys_argv[i + 2]))
            exit(1)
        image_list.append(image)
    return image_list


def print_descriptor(descriptor):
    descriptor_list = descriptor.getDescriptor()
    for item_descriptor in descriptor_list:
        print(item_descriptor)


def set_logging(value):
    config = fe.createSettingsProvider("data/faceengine.conf")
    config_path = config.getDefaultPath()
    print("Config settings: DefaultPath {0}".format(config_path))
    config.setValue("system", "verboseLogging", fe.SettingsProviderValue(value))
    face_engine.setSettingsProvider(config)
    val = config.getValue("system", "verboseLogging")
    print("Config settings: \"system\", \"verboseLogging\" = {0}".format(val.asInt()))


def extractor_test_aggregation(version, use_mobile_net, cpu_type, device):
    print("Extractor_test_aggregation")
    config = fe.createSettingsProvider("data/faceengine.conf")
    runtime_config = fe.createSettingsProvider("data/runtime.conf")
    config_path = config.getDefaultPath()
    print("Default path = ", config_path)
    config.setValue("DescriptorFactory::Settings", "model", fe.SettingsProviderValue(version))
    config.setValue("DescriptorFactory::Settings", "useMobileNet", fe.SettingsProviderValue(use_mobile_net))
    config.setValue("system", "verboseLogging", fe.SettingsProviderValue(1))
    runtime_config.setValue("Runtime", "deviceClass", fe.SettingsProviderValue(device))
    runtime_config.setValue("Runtime", "cpuClass", fe.SettingsProviderValue(cpu_type))
    face_engine.setSettingsProvider(config)
    face_engine.setRuntimeSettingsProvider(runtime_config)
    warps = [fe.Image(), fe.Image()]
    err_load_warp1 = warps[0].load("testData/warp1.ppm")
    err_load_warp2 = warps[1].load("testData/warp2.ppm")
    if err_load_warp1.isError or not warps[0].isValid():
        print("invalid warp image1: ", err_load_warp1.what)
        exit(-1)
    if err_load_warp2.isError or not warps[1].isValid():
        print("invalid warp image2:", err_load_warp2.what)
        exit(-1)
    batch_size = len(warps)
    descriptor_extractor = face_engine.createExtractor()
    batch = face_engine.createDescriptorBatch(batch_size)
    descriptor1 = face_engine.createDescriptor()
    descriptor2 = face_engine.createDescriptor()
    aggregation = face_engine.createDescriptor()

    result, aggregated_garbage_score, garbage_scores = descriptor_extractor.extractFromWarpedImageBatch(warps, batch, aggregation, batch_size)
    if result.isError:
        print("aggregation: failed to extract descriptor batch, reason:", result.what)
        exit(-1)
    print("Garbage scores:", garbage_scores)
    print("Agregated garbage score:", aggregated_garbage_score)

    result1, garbage_score1 = descriptor_extractor.extractFromWarpedImage(warps[0], descriptor1)
    if result1.isError:
        print("aggregation: failed to extract descriptor, reason:", result1.what)
        exit(-1)
    print("Garbage score1:", garbage_score1)

    result2, garbage_score2 = descriptor_extractor.extractFromWarpedImage(warps[1], descriptor2)
    if result2.isError:
        print("aggregation: failed to extract descriptor, reason:", result2.what)
        exit(-1)
    print("Garbage score2:", garbage_score2)


if __name__ == "__main__":
    batch_size = len(sys.argv) - 2
    # correct path or put directory "data" with example.py
    face_engine = fe.createFaceEngine("data")
    if not make_activation(face_engine):
        print("failed to activate license!")
        exit(-1)
    # more detailed description of config see in luna-sdk/doc/ConfigurationGuide.pdf
    set_logging(1)
    image_list = load_list_of_images(batch_size, sys.argv)
    try:
        (descriptor1, descriptor2, descriptor_batch) = extractor_example(image_list, batch_size)
        matcher_example(descriptor1, descriptor2, descriptor_batch)
        # print_descriptor(descriptor1)
        # print_descriptor(descriptor2)
        # as test
        extractor_test_aggregation(46, True, "auto", "cpu")
    except RuntimeError as ex:
        print(ex)
        exit(1)

