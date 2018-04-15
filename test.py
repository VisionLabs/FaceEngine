import fe as f

a = f.createFaceEngine("/home/mar/Desktop/luna-sdk/data",
                   "/home/mar/Desktop/luna-sdk/data/faceengine.conf")
b = a.createAttributeEstimator()
d = a.createQualityEstimator()
print(d)
# b.estimate()
g = f.createFaceEngine2("/home/mar/Desktop/luna-sdk/data",
                         "/home/mar/Desktop/luna-sdk/data/faceengine.conf")
a = f.createSettingsProvider("/home/mar/Desktop/luna-sdk/data/faceengine.conf")
print(a)