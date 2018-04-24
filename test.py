import fe as f

faceEnginePtr = f.createPyFaceEnginePtr("data",
                                      "data/faceengine.conf")

attributeEstimator = faceEnginePtr.createAttributeEstimator()
qualityEstimator = faceEnginePtr.createQualityEstimator()
ethnicityEstimator = faceEnginePtr.createEthnicityEstimator()
detector = faceEnginePtr.createDetector(f.ODT_MTCNN)

# faceEngine = f.createFaceEngine("data",
#                    "data/faceengine.conf")
# attributeEstimator = faceEngine.createAttributeEstimator()
# qualityEstimator = faceEngine.createQualityEstimator()
# ethnicityEstimator = faceEngine.createEthnicityEstimator()

print(attributeEstimator)
print(qualityEstimator)
print(ethnicityEstimator)


image = f.Image()
print(f.Type.R8)
print(f.Format())
print(image.load("testData/warp1.ppm"))
print(image.load("testData/warp2.ppm", f.Format(f.Type.R8G8B8)))
# print(image.load_as("testData/warp1.ppm", f.Format_Type.R8))
print(image)
print("image width {0}".format(image.getWidth()))
print("image height {0}".format(image.getHeight()))
print("image is valid {0}".format(image.isValid()))

attribute_result = attributeEstimator.estimate(image)
quality_result = qualityEstimator.estimate(image)
ethnicity_result = ethnicityEstimator.estimate(image)
ethn = ethnicity_result['EthnicityEstimation']
print(f.Ethnicity.Indian)

print(ethnicity_result)
print("test of getEthnicityScore() {0}".format(ethn.getEthnicityScore(f.Ethnicity.Caucasian)))
print("test of get getPredominantEthnicity() {0}".format(ethn.getPredominantEthnicity()))

print(attribute_result)
print(quality_result)
print(ethnicity_result)

# Rect test
# constructors
rect1 = f.Rect()
print(rect1)
# print(rect1.x, rect1.y. rect1.width, rect1.height)
print(rect1.size())
print(rect1.x, rect1.y, rect1.width, rect1.height)
rect2 = f.Rect(2, 5, 7, 10)
print(rect2)
rect3 = f.Rect(f.Vector2i(0, 0), f.Vector2i(100, 120))
print(rect3)
rect3 = rect2
print(rect3 == rect2)
rect4 = f.Rect()
rect4.x = 100
rect4.y = 100
rect4.width = 100
rect4.height = 100
print(rect4)
print("size={0}, topLeft={1}, topLeft={2}, center={3}, bottomRight={4}, bottom={5}, left={6}, right={7}, "
      "getArea={8}, isValid={9} ".format(rect4.size(), rect4.topLeft(),
      rect4.center(), rect4.bottomRight(), rect4.top(), rect4.bottom(),
      rect4.left(), rect4.right(), rect4.getArea(), rect4.isValid()))
rect4.adjust(0, 0, 100, 100)
rect_adjusted = rect4.adjusted(10, 10, 10, 10)
print("adjust={0}, adjusted={1}".format(rect4, rect_adjusted))
rect4.set(f.Vector2i(20, 20), f.Vector2i(40, 40))
print("set={0}".format(rect4))

# detector test and example
maxDetections = 3
image_det = f.Image()
err = image_det.load("testData/image2.ppm")
print("Image error = ", err)

detector = faceEnginePtr.createDetector(f.ODT_MTCNN)
detector_result = detector.detect(image_det, image_det.getRect(), maxDetections)
test = detector_result[0]["Landmarks5"]
print("Landmarks test {0}".format(test[0]))
print("Landmarks test {0}".format(test[0]))

for i, item in enumerate(detector_result, 1):
    print(i, item)


# warper test and example
warper = faceEnginePtr.createWarper()
transformation = warper.createTransformation(detector_result[0]["Detection"],
                                             detector_result[0]["Landmarks5"])
print(transformation)
warperResult = warper.warp(image_det, transformation)
warpImage = warperResult["transformedImage"]
print(warpImage)
print(warpImage.getWidth(), warpImage.getHeight(), warpImage.isValid())
print(warperResult)
ethnicityEstimator = faceEnginePtr.createEthnicityEstimator()
ethnicityEstimator.estimate(warpImage)

warperResult2 = warper.warp(detector_result[0]["Landmarks5"], transformation)
warperResult3 = warper.warp(detector_result[0]["Landmarks68"], transformation)

print("warperResult2 with Landmarks5 = ", warperResult2)
print("warperResult2 with Landmarks68 = ", warperResult3)


# descriptor, creating objects
descriptor1 = faceEnginePtr.createDescriptor()
descriptor2 = faceEnginePtr.createDescriptor()
aggregation = faceEnginePtr.createDescriptor()


images = [f.Image(), f.Image(), f.Image()]
# for i in range(2):
images[0].load("testData/warp1.ppm")
images[1].load("testData/warp2.ppm")
images[2].load("testData/photo_2017-03-30_14-47-43_p.ppm")

batchSize = len(images)
descriptorBatch = faceEnginePtr.createDescriptorBatch(batchSize)
extractor = faceEnginePtr.createExtractor()
matcher = faceEnginePtr.createMatcher()
table = faceEnginePtr.createLSHTable(descriptorBatch)

print(images)
print(type(extractor))
print("Descriptor test befor = ", descriptor1.getModelVersion(), descriptor1.getDescriptorLength())
ext1 = extractor.extractFromWarpedImage(images[0], descriptor1)
ext2 = extractor.extractFromWarpedImage(images[1], descriptor2)
print("Descriptor test after = ", descriptor1.getModelVersion(), descriptor1.getDescriptorLength())
print("extractor result =", ext2[1])

print("Descriptor batch test befor", descriptorBatch.getMaxCount(), descriptorBatch.getCount(),
      descriptorBatch.getModelVersion(), descriptorBatch.getDescriptorSize())
ext_batch1 = extractor.extractFromWarpedImageBatch(images, descriptorBatch, aggregation, batchSize)
# print("aggregation: ", aggregation.getModelVersion(), aggregation.getDescriptorLength())
# ext_batch2 = extractor.extractFromWarpedImageBatch(images, descriptorBatch, batchSize)

print("Batch result = ", ext_batch1)
# print(ext_batch2)
print("Descriptor batch test after", descriptorBatch.getMaxCount(), descriptorBatch.getCount(),
      descriptorBatch.getModelVersion(), descriptorBatch.getDescriptorSize())
print(descriptor1)
print(descriptorBatch)
print(extractor)
print(matcher)
print(table)

# matcher test

result1 = matcher.match(descriptor1, descriptor2)
result2 = matcher.match(descriptor1, descriptorBatch)
result3 = matcher.match(descriptor1, descriptorBatch, [0,])
result4 = matcher.matchCompact(descriptor1, descriptorBatch, [1])
print(result1)
print(result2)
print(result3)
print(result4)

# test of second part estimators

headPoseEstimator = faceEnginePtr.createHeadPoseEstimator()
blackWhiteEstimator = faceEnginePtr.createBlackWhiteEstimator()
depthEstimator = faceEnginePtr.createDepthEstimator()
iREstimator = faceEnginePtr.createIREstimator()
smileEstimator = faceEnginePtr.createSmileEstimator()
faceFlowEstimator = faceEnginePtr.createFaceFlowEstimator()
eyeEstimator = faceEnginePtr.createEyeEstimator()
emotionsPoseEstimator = faceEnginePtr.createEmotionsEstimator()
gazeEstimator = faceEnginePtr.createGazeEstimator()

print(headPoseEstimator)
print(blackWhiteEstimator)
print(depthEstimator)
print(iREstimator)
print(smileEstimator)
print(faceFlowEstimator)
print(eyeEstimator)
print(emotionsPoseEstimator)
print(gazeEstimator)
# for i in enumerate(detector_result):
#     print(detector_result[i])

# q = b.estimate(image.getImage(), attr)
# attr.gender = 0.7
# attr.glasses = 1
# attr.age = 0.3
# print(attr)
landmarks5 = f.Landmarks5()
landmarks68 = f.Landmarks68()
# landmarks5[3] = f.Vector2(5, 3)

# print("verify assigning", landmarks5.getitem(3))
#
# print(landmarks5.getitem)
# # landmarks5[3].y = 5.0
# landmarks68.setX(45, 120)
# landmarks68.setY(45, 240)
# print("lanmarks68[45] = {0}".format(landmarks68.getitem(45)))
# vector2f = f.Vector2f(5, 3)
#
# transformation = f.Transformation()
#
# transformation.angleDeg = 10.0
# transformation.scale = 1.1
# transformation.centerP = f.Vector2f(500.0, 10.0)
# transformation.detectionTopLeft = f.Vector2i(10, 5)
# print(transformation.detectionTopLeft)
#
# print("transformation = ", transformation)
#
# quality = f.Quality()
# quality.light = 7
# print(quality)
# print(quality.getQuality())
# print(len(landmarks5))
# print(len(landmarks68))

# a = f.createSettingsProvider("data/faceengine.conf")
# print(a)