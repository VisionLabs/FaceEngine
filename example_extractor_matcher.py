import sys

def help():
    print("example_extractor_matcher.py <path to FaceEngine*.so> <path to image>")

if len(sys.argv) != 3:
    help()
    exit(1)

# pass path dir with FaceEngine*.so and add it to system directory
sys.path.append(sys.argv[1])
import FaceEngine as f

image_path = sys.argv[2]

# correct paths to data or put directories data and testData with example.py
faceEngine = f.createFaceEngine("data",
                                "data/faceengine.conf")

# descriptor, creating objects
print("Creating descriptors")
descriptor1 = faceEngine.createDescriptor()
descriptor2 = faceEngine.createDescriptor()
aggregation = faceEngine.createDescriptor()

print("Batch descriptor example")
image_list = [f.Image(), f.Image(), f.Image()]
# for i in range(2):
image_list[0].load("testData/warp1.ppm")
image_list[1].load("testData/warp2.ppm")
image_list[2].load("testData/photo_2017-03-30_14-47-43_p.ppm")

if not image_list[0].isValid() or not image_list[1].isValid() or not image_list[2].isValid():
    print("Failed list creatnig: one of paths to images for batch is not found")
    exit(1)

batchSize = len(image_list)
print("batchSize {0}".format(batchSize))
descriptorBatch = faceEngine.createDescriptorBatch(batchSize)
extractor = faceEngine.createExtractor()
matcher = faceEngine.createMatcher()
table = faceEngine.createLSHTable(descriptorBatch)

print(image_list)
print(type(extractor))
print("Descriptor test befor = ", descriptor1.getModelVersion(), descriptor1.getDescriptorLength())
ext1 = extractor.extractFromWarpedImage(image_list[0], descriptor1)
ext2 = extractor.extractFromWarpedImage(image_list[1], descriptor2)
print("Descriptor test after = ", descriptor1.getModelVersion(), descriptor1.getDescriptorLength())
print("extractor result =", ext2)

print("Descriptor batch test befor: ", descriptorBatch.getMaxCount(), descriptorBatch.getCount(),
      descriptorBatch.getModelVersion(), descriptorBatch.getDescriptorSize())
ext_batch1 = extractor.extractFromWarpedImageBatch(image_list, descriptorBatch, aggregation, batchSize)
print("aggregation: ", aggregation.getModelVersion(), aggregation.getDescriptorLength())
ext_batch2 = extractor.extractFromWarpedImageBatch(image_list, descriptorBatch, batchSize)

print("Garbage score list1 = ", ext_batch1)
print("Garbage score list2 = ", ext_batch2)
print("Descriptor batch test after", descriptorBatch.getMaxCount(), descriptorBatch.getCount(),
      descriptorBatch.getModelVersion(), descriptorBatch.getDescriptorSize())
print(descriptor1)
print(descriptorBatch)
print(extractor)
print(matcher)
print(table)

# matcher test
print("\nMatcher example")
result1 = matcher.match(descriptor1, descriptor2)
result2 = matcher.match(descriptor1, descriptorBatch)
result3 = matcher.match(descriptor1, descriptorBatch, [0, 1])
result4 = matcher.matchCompact(descriptor1, descriptorBatch, [1])
print("Match result of different descriptors: {0}".format(result2))
print("Match result of descriptor and descriptor batch: {0}".format(result2))
print("Match result of descriptor and 2 descriptors from batch: {0}".format(result3))
print("MatchCompact result of descriptors: {0}".format(result3))