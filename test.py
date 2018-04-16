import fe as f

a = f.createFaceEngine("data",
                   "data/faceengine.conf")
b = a.createAttributeEstimator()
d = a.createQualityEstimator()
print(d)
# b.estimate()

image = f.Image()
print(image.load("testData/overlap.ppm"))
attr = f.AttributeEstimation()
# attr.gender = 0.7
# attr.glasses = 1
# attr.age = 0.3
# print(attr)

b.estimate(image, attr)

a = f.createSettingsProvider("data/faceengine.conf")
print(a)