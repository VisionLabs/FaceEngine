import fe as f

a = f.createFaceEngine("data",
                   "data/faceengine.conf")
b = a.createAttributeEstimator()
d = a.createQualityEstimator()
print(d)
# b.estimate()

image = f.Image()
# print(image.load("testData/overlap.ppm"))
attr = f.AttributeEstimation()
attr.gender = 0.7
attr.glasses = 1
attr.age = 0.3
print(attr)
landmarks5 = f.Landmarks5()
landmarks68 = f.Landmarks68()
# landmarks5[3].x = f.Vector2(5, 3)
print("verify assigning", landmarks5[3].x)
landmarks5.setX(3, 10)
landmarks5.setY(3, 65)
landmarks5[3].x = 210
# landmarks5[3].y = 5.0
print("verify assigning", landmarks5[3])
print(landmarks68[56])
landmarks68[45].x = 10
print("lanmarks68[45] = {0}".format(landmarks68[45]))
vector2 = f.Vector2(5, 3)

print(vector2)

landmarks5[0].y = 9
print("lanmarks5 {0}".format(landmarks5[0]))
# must be error here
# print(landmarks68[69])
print(len(landmarks5))
print(len(landmarks68))
print()


# b.estimate(image, attr)

a = f.createSettingsProvider("data/faceengine.conf")
print(a)