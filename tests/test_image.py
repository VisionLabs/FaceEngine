import sys
import unittest
import argparse
import sys
import os

# if FaceEngine is not instaflled in system
parser = argparse.ArgumentParser()
parser.add_argument("-b", "--bind-path", type=str,
                    help="path to FaceEngine*.so file - binding of luna-sdk")
args = parser.parse_args()
path_to_binding = args.bind_path
print("Directory {0} with python bindings of FaceEngine was included".format(path_to_binding))
print(sys.argv)

# clean system arguments for test argument parsing
del(sys.argv[1])
del(sys.argv[1])

sys.path.append(path_to_binding)

import FaceEngine as f
# correct this!!!
faceEnginePtr = f.createPyFaceEnginePtr("data",
                                        "data/faceengine.conf")

image = f.Image()

load_error = image.load("testData/warp1.ppm")

print(image.getWidth())
print(image.getHeight())
print(image.isValid())
print(image.getRect())
new_file_path = "testData/test-warp1.ppm"
save_error = image.save(new_file_path)
loaded_image = f.Image()
loaded_image.load(new_file_path)

# print(os.path.isfile(new_file_path)f)
#
# print(os.path.isfile(new_file_path))

# test load in different formats
image2 = f.Image()
load_error_Unknown = image2.load("testData/warp2.pp", f.Format(f.FormatType.Unknown))
load_error_B8G8R8X8 = image2.load("testData/warp2.ppm", f.Format(f.FormatType.B8G8R8X8))
load_error_R8G8B8X8 = image2.load("testData/warp2.ppm", f.Format(f.FormatType.R8G8B8X8))
load_error_B8G8R8 = image2.load("testData/warp2.ppm", f.Format(f.FormatType.B8G8R8))
load_error_R8G8B8 = image2.load("testData/warp2.ppm", f.Format(f.FormatType.R8G8B8))
load_error_R8 = image2.load("testData/warp2.ppm", f.Format(f.FormatType.R8))
# expected error with R16
load_error_R16 = image2.load("testData/warp2.ppm", f.Format(f.FormatType.R16))

class TestFaceEngineImage(unittest.TestCase):

    def test_load(self):
        self.assertEqual(load_error.isOk, 1)
        self.assertEqual(load_error.isError, 0)
        self.assertEqual(load_error.what, 'Ok')
        self.assertEqual(load_error.imageError, f.ImageError.Ok)

    def test_load_format(self):
        self.assertEqual(load_error_B8G8R8X8.isOk, 1)
        self.assertEqual(load_error_R8G8B8X8.isOk, 1)
        self.assertEqual(load_error_B8G8R8.isOk, 1)
        self.assertEqual(load_error_R8G8B8.isOk, 1)
        self.assertEqual(load_error_R8.isOk, 1)
        # R16 is used for internal transformation for depth map and cannot be saved or downloaded
        self.assertEqual(load_error_R16.what, "Requiered conversion not implemented")
        self.assertEqual(load_error_Unknown.what, "Unsupported type")

    def test_save(self):
        self.assertTrue(os.path.isfile(new_file_path))
        self.assertEqual(save_error.isOk, 1)
        self.assertEqual(save_error.isError, 0)
        self.assertEqual(save_error.what, 'Ok')
        self.assertEqual(save_error.imageError, f.ImageError.Ok)
        print("deleting of saved {0}".format(new_file_path))
        os.remove(new_file_path)

    def test_identity(self):
        self.assertEqual(image.getHeight(), loaded_image.getHeight(), 250)
        self.assertEqual(image.getWidth(), loaded_image.getWidth(), 250)
        self.assertEqual(image.isValid(), loaded_image.isValid())
        self.assertEqual(image.getRect(), loaded_image.getRect())

class ExpectedFailureTestCase(unittest.TestCase):
    @unittest.expectedFailure
    def test_fail(self):
        self.assertEqual(load_error_Unknown.isOk, 0, "broken")
        # R16 is used for internal transformation for depth map and cannot be saved or downloaded
        self.assertEqual(load_error_R16.isOk, 1, "R16 cannot be saved or downloaded")



if __name__ == '__main__':
    unittest.main()

