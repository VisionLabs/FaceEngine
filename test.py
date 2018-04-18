import fe as f

a = f.createFaceEngine("data",
                   "data/faceengine.conf")
attributeEstimator = a.createAttributeEstimator()
qualityEstimator = a.createQualityEstimator()
print(attributeEstimator)


image = f.Image()
print(f.Format_Type.R8)
print(image.load("testData/warp2.ppm"))
# print(image.load_as("testData/warp2.ppm", f.Format_Type.R8))
# print(image.load_as("testData/warp1.ppm", f.Format_Type.R8))
print(image)
print(image.getWidth())
print(image.getHeight())
print(image.isValid())

attr = f.AttributeEstimation()
qual = f.Quality()
attribute_result = f.AttibuteEstimator_estimate(attributeEstimator, image.getImage(), attr)
quality_result = f.QualityEstimator_estimate(qualityEstimator, image.getImage(), qual)

print(attribute_result)
print(quality_result)
# q = b.estimate(image.getImage(), attr)
# attr.gender = 0.7
# attr.glasses = 1
# attr.age = 0.3
# print(attr)
# landmarks5 = f.Landmarks5()
# landmarks68 = f.Landmarks68()
# # landmarks5[3] = f.Vector2(5, 3)
# landmarks5.setX(3, 10)
# landmarks5.setY(3, 65)
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



a = f.createSettingsProvider("data/faceengine.conf")
# print(a)