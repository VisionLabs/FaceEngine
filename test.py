import fe as f

a = f.createFaceEngine("/home/mar/Desktop/luna-sdk/data",
                   "/home/mar/Desktop/luna-sdk/data/faceengine.conf")
b = a.createAttributeEstimator()
d = a.createQualityEstimator()
print(d)
# b.estimate()

image = f.Image()
image.load("/home/mar/Pictures/2017-11-22-161637.jpg")
a = f.createSettingsProvider("/home/mar/Desktop/luna-sdk/data/faceengine.conf")
print(a)