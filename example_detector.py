import sys

def help():
    print("python example_detector.py <path to FaceEngine*.so> <path to image>")

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

print("Detector example")
detector = faceEngine.createDetector(f.ODT_MTCNN)

maxDetections = 3
image_det = f.Image()
err = image_det.load(image_path)
print(image_det.getHeight(), image_det.getWidth(), image_det.isValid())
print("Image error = ", err)

# create detector
detector = faceEngine.createDetector(f.ODT_MTCNN)

# list of tuples: (detection, landmarks5, landmarks68)
detector_result = detector.detect(image_det, image_det.getRect(), maxDetections)
print("detector result = ", detector_result)
test = detector_result[0][1]
print("Landmarks test {0}".format(test[0]))
print("Landmarks test {0}".format(test[0]))

print("Detections: ")
for i, item in enumerate(detector_result, 1):
    print(i, item)

# warper example
print("\nWarper example: ")
warper = faceEngine.createWarper()
transformation = warper.createTransformation(detector_result[0][0],
                                             detector_result[0][1])
print("transformation = ", transformation)
warperResult = warper.warp(image_det, transformation)
warpImage = warperResult
print(warpImage)
print(warpImage.getWidth(), warpImage.getHeight(), warpImage.isValid())

transformedLandmarks5 = warper.warp(detector_result[0][1], transformation)
print("\ntransformedLandmarks:")
for i in range(len(transformedLandmarks5)):
    print(transformedLandmarks5[i])
transformedLandmarks68 = warper.warp(detector_result[0][2], transformation)

print("warperResult2 with Landmarks5 = ", transformedLandmarks5)
print("warperResult2 with Landmarks68 = ", transformedLandmarks68)

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
print(result1)
print(result2)
print(result3)
print(result4)