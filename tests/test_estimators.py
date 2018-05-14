import sys
import unittest
import argparse
import sys
import os
import glob

# if FaceEngine is not installed within the system, add the directory with FaceEngine*.so to system paths
parser = argparse.ArgumentParser()
parser.add_argument("-b", "--bind-path", type=str,
                    help="path to FaceEngine*.so file - binding of luna-sdk")

args = parser.parse_args()
path_to_binding = args.bind_path
print(path_to_binding)
if not os.path.isdir(path_to_binding):
    print("Directory with FaceEngine*.so was not found.")
    exit(1)


print("Directory {0} with python bindings of FaceEngine was included".format(path_to_binding))
print(sys.argv)

sys.path.append(path_to_binding)
print("sys.path = ", sys.path)

# if FaceEngine is installed only this string of code is required for module importing
import FaceEngine as f

# erase two first arguments for unittest argument parsing
del(sys.argv[1])
del(sys.argv[1])

faceEnginePtr = f.createPyFaceEnginePtr("data",
                                        "data/faceengine.conf")

class TestFaceEngineRect(unittest.TestCase):

    def test_AttributeEstimator(self):
        attributeEstimator = faceEnginePtr.createAttributeEstimator()
        image = f.Image()
        image.load("testData/warp1.ppm")
        self.assertTrue(image.isValid())
        attribute_result = attributeEstimator.estimate(image)
        print(attribute_result)
        self.assertEqual(attribute_result.gender, 1)
        self.assertAlmostEqual(attribute_result.glasses, 0.0, places=3)
        self.assertEqual(attribute_result.age, 24.0)

    def test_QualityEstimator(self):
        qualityEstimator = faceEnginePtr.createQualityEstimator()
        image = f.Image()
        image.load("testData/warp1.ppm")
        self.assertTrue(image.isValid())
        quality_result = qualityEstimator.estimate(image)
        print(quality_result)
        self.assertAlmostEqual(quality_result.light, 0.961, places=2)
        self.assertAlmostEqual(quality_result.dark, 0.975, places=2)
        self.assertAlmostEqual(quality_result.gray, 0.979, places=2)
        self.assertAlmostEqual(quality_result.blur, 0.955, places=2)

    def test_EthnicityEstimator(self):
        ethnicityEstimator = faceEnginePtr.createEthnicityEstimator()
        image = f.Image()
        image.load("testData/warp1.ppm")
        self.assertTrue(image.isValid())
        ethnicity_result = ethnicityEstimator.estimate(image)
        print(ethnicity_result)
        self.assertAlmostEqual(ethnicity_result.africanAmerican, 0.0, places=2)
        self.assertAlmostEqual(ethnicity_result.indian, 0.0, places=2)
        self.assertAlmostEqual(ethnicity_result.asian, 0.0, places=2)
        self.assertAlmostEqual(ethnicity_result.caucasian, 1.0, places=2)

if __name__ == '__main__':
    unittest.main()

