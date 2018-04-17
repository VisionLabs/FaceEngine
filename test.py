import fe as f

a = f.createFaceEngine("data",
                   "data/faceengine.conf")
b = a.createAttributeEstimator()
d = a.createQualityEstimator()
print(b)
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
# landmarks5[3] = f.Vector2(5, 3)
landmarks5.setX(3, 10)
landmarks5.setY(3, 65)
print("verify assigning", landmarks5.getitem(3))

print(landmarks5.getitem)
# landmarks5[3].y = 5.0
landmarks68.setX(45, 120)
landmarks68.setY(45, 240)
print("lanmarks68[45] = {0}".format(landmarks68.getitem(45)))
vector2 = f.Vector2(5, 3)



print(vector2)

print("lanmarks5 {0}".format(landmarks5[0]))

print(len(landmarks5))
print(len(landmarks68))

# b.estimate(image, attr)

a = f.createSettingsProvider("data/faceengine.conf")
# print(a)