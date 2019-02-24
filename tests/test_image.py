import sys
import unittest
import argparse
import sys
import os
import numpy as np



# if FaceEngine is not instaflled in system
parser = argparse.ArgumentParser()
parser.add_argument("-b", "--bind-path", type=str,
                    help="path to FaceEngine*.so file - binding of luna-sdk")
args = parser.parse_args()
path_to_binding = args.bind_path
print("Directory {0} with python bindings of FaceEngine was included".format(path_to_binding))
print(sys.argv)

# erase two first arguments for unittest argument parsing
del(sys.argv[1])
del(sys.argv[1])

sys.path.append(path_to_binding)

import FaceEngine as f
# correct this!!!
faceEnginePtr = f.createFaceEngine("data",
                                        "data/faceengine.conf")

image = f.Image()
load_error = image.load("testData/warp1.ppm")

new_file_path = "testData/test-warp1.ppm"
save_error = image.save(new_file_path)
loaded_image = f.Image()
loaded_image.load(new_file_path)

# test load in different formats
image2 = f.Image()
load_error_Unknown = image2.load("testData/warp2.pp", f.FormatType.Unknown)
load_error_B8G8R8X8 = image2.load("testData/warp2.ppm", f.FormatType.B8G8R8X8)
load_error_R8G8B8X8 = image2.load("testData/warp2.ppm", f.FormatType.R8G8B8X8)
load_error_B8G8R8 = image2.load("testData/warp2.ppm", f.FormatType.B8G8R8)
load_error_R8G8B8 = image2.load("testData/warp2.ppm", f.FormatType.R8G8B8)
load_error_R8 = image2.load("testData/warp2.ppm", f.FormatType.R8)
# expected error with R16
load_error_R16 = image2.load("testData/warp2.ppm", f.FormatType.R16)

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

    def test_identity(self):
        self.assertEqual(image.getHeight(), loaded_image.getHeight(), 250)
        self.assertEqual(image.getWidth(), loaded_image.getWidth(), 250)
        self.assertEqual(image.isValid(), loaded_image.isValid())
        self.assertEqual(image.getRect(), loaded_image.getRect())

    def test_get_data_identity(self):
        image_np = image.getData()
        w, h, c = image_np.shape
        self.assertEqual(image.getHeight(), h)
        self.assertEqual(image.getWidth(), w)
        self.assertEqual(image.getChannelCount(), c)
        image_temp = f.Image()
        image_temp.load("testData/smile.ppm", f.FormatType.B8G8R8X8)
        image_np = image_temp.getData()
        w, h, c = image_np.shape
        self.assertEqual(image_temp.getHeight(), h)
        self.assertEqual(image_temp.getWidth(), w)
        self.assertEqual(image_temp.getChannelCount(), c)
        image_temp.load("testData/smile.ppm", f.FormatType.R8G8B8X8)
        image_np = image_temp.getData()
        w, h, c = image_np.shape
        self.assertEqual(image_temp.getHeight(), h)
        self.assertEqual(image_temp.getWidth(), w)
        self.assertEqual(image_temp.getChannelCount(), c)
        image_temp.load("testData/smile.ppm", f.FormatType.B8G8R8)
        image_np = image_temp.getData()
        w, h, c = image_np.shape
        self.assertEqual(image_temp.getHeight(), h)
        self.assertEqual(image_temp.getWidth(), w)
        self.assertEqual(image_temp.getChannelCount(), c)
        image_temp.load("testData/smile.ppm", f.FormatType.R8G8B8)
        image_np = image_temp.getData()
        w, h, c = image_np.shape
        self.assertEqual(image_temp.getHeight(), h)
        self.assertEqual(image_temp.getWidth(), w)
        self.assertEqual(image_temp.getChannelCount(), c)
        image_temp.load("testData/smile.ppm", f.FormatType.R8)
        image_np = image_temp.getData()
        w, h, c = image_np.shape
        self.assertEqual(image_temp.getHeight(), h)
        self.assertEqual(image_temp.getWidth(), w)
        self.assertEqual(image_temp.getChannelCount(), c)

    def test_set_data(self):
        print("Tests for image.setData are enabled.")
        test_image1 = f.Image()
        test_image2 = f.Image()
        test_image1.load("testData/overlap_image1.jpg")
        test_image2.load("testData/overlap_image1_copy.jpg")
        image_np1 = test_image1.getData()
        image_np2 = test_image2.getData()
        self.assertTrue(np.array_equal(image_np1, image_np2))

        test_image2.setData(image_np2, f.FormatType.R8G8B8X8)
        image_np2_4_channels = test_image2.getData()
        self.assertEqual(4, test_image2.getChannelCount())
        self.assertEqual(f.FormatType.R8G8B8X8, test_image2.getFormat())
        self.assertEqual("FormatType.R8G8B8X8", str(test_image2.getFormat()))
        # by default numpy is converted to R8G8B8 or R8
        test_image2.setData(image_np1)
        image_np3 = test_image2.getData()
        self.assertTrue(np.array_equal(image_np3, image_np1))
        self.assertFalse(np.array_equal(image_np3, image_np2_4_channels))

    def test_load_from_memory(self):
        with open("testData/6big.ppm", 'rb') as file:
            data = file.read()
            test_image1 = f.Image()
            err = test_image1.loadFromMemory(data, len(data))
            self.assertTrue(err.isOk)
            test_image2 = f.Image()
            test_image2.load("testData/6big.ppm")
            self.assertTrue(np.array_equal(test_image1.getData(), test_image2.getData()))
            self.assertTrue(test_image1.getWidth(), test_image2.getWidth())
            self.assertTrue(test_image1.getHeight(), test_image2.getHeight())
            self.assertTrue(test_image1.isValid(), test_image2.isValid())
            self.assertTrue(np.array_equal(test_image1.getData(), test_image2.getData()))

    def test_load_from_memory_format(self):
        with open("testData/6big.ppm", 'rb') as file:
            data = file.read()
            test_image1 = f.Image()
            err = test_image1.loadFromMemory(data, len(data), f.FormatType.R8G8B8X8)
            self.assertTrue(err.isOk)
            test_image2 = f.Image()
            test_image2.load("testData/6big.ppm", f.FormatType.R8G8B8X8)
            self.assertTrue(np.array_equal(test_image1.getData(), test_image2.getData()))
            self.assertTrue(test_image1.getWidth(), test_image2.getWidth())
            self.assertTrue(test_image1.getHeight(), test_image2.getHeight())
            self.assertTrue(test_image1.isValid(), test_image2.isValid())
            self.assertTrue(np.array_equal(test_image1.getData(), test_image2.getData()))

    def test_save(self):
        self.assertTrue(os.path.isfile(new_file_path))
        self.assertEqual(save_error.isOk, 1)
        self.assertEqual(save_error.isError, 0)
        self.assertEqual(save_error.what, 'Ok')
        self.assertEqual(save_error.imageError, f.ImageError.Ok)
        # print("deleting of saved {0}".format(new_file_path))
        os.remove(new_file_path)

class ExpectedFailureTestCase(unittest.TestCase):
    @unittest.expectedFailure
    def test_fail(self):
        self.assertEqual(load_error_Unknown.isOk, 0, "broken")
        # R16 is used for internal transformation for depth map and cannot be saved or downloaded
        self.assertEqual(load_error_R16.isOk, 1, "R16 cannot be saved or downloaded")



if __name__ == '__main__':
    unittest.main()

