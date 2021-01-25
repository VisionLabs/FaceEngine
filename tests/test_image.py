import sys
import unittest
import argparse
import sys
import os
import numpy as np
import cv2


# if FaceEngine is not instaflled in system
parser = argparse.ArgumentParser()
parser.add_argument("-b", "--bind-path", type=str,
                    help="path to dir with FaceEngine*.so file - binding of luna-sdk")

args = parser.parse_args()

if len(sys.argv) == 1 or not args.bind_path or not os.path.isdir(args.bind_path):
    parser.print_help(sys.stderr)
    sys.exit(1)

path_to_binding = args.bind_path
print("Directory {0} with python bindings of FaceEngine was included".format(path_to_binding))
print(sys.argv)

# erase two first arguments for unittest argument parsing
del(sys.argv[1])
del(sys.argv[1])

sys.path.append(path_to_binding)

import FaceEngine as f

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
        self.assertEqual(load_error.error, f.ImageError.Ok)

    def test_load_format(self):
        self.assertEqual(load_error_B8G8R8X8.isOk, 1)
        self.assertEqual(load_error_R8G8B8X8.isOk, 1)
        self.assertEqual(load_error_B8G8R8.isOk, 1)
        self.assertEqual(load_error_R8G8B8.isOk, 1)
        self.assertEqual(load_error_R8.isOk, 1)
        # R16 is used for internal transformation for depth map and cannot be saved or downloaded
        self.assertEqual(load_error_R16.what, "Required conversion not implemented")
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
        image_temp.load("testData/smile.ppm", f.FormatType.IR_X8X8X8)
        image_np = image_temp.getData()
        w, h, c = image_np.shape
        self.assertEqual(image_temp.getHeight(), h)
        self.assertEqual(image_temp.getWidth(), w)
        self.assertEqual(image_temp.getChannelCount(), c)
        self.assertEqual(image_temp.getFormat(), f.FormatType.IR_X8X8X8)

    def test_getR16_data_identity(self):
        depth_image = f.loadImage("testData/warp.depth")
        image_np = depth_image.getDataR16()
        w, h, c = image_np.shape
        self.assertEqual(depth_image.getHeight(), h)
        self.assertEqual(depth_image.getWidth(), w)
        self.assertEqual(depth_image.getChannelCount(), c)
        err_save1 = depth_image.save("testData/test_depth1.png", f.FormatType.R16)
        self.assertTrue(err_save1.isOk)
        depth_image_load = f.Image()
        err_open1 = depth_image_load.load("testData/test_depth1.png", f.FormatType.R16)
        self.assertTrue(err_open1.isOk)
        np_depth_arr1 = depth_image.getDataR16()
        np_depth_arr2 = depth_image_load.getDataR16()
        image_with_set_data = f.Image()
        image_with_set_data.setData(np_depth_arr1, f.FormatType.R16)
        np_depth_arr3 = image_with_set_data.getDataR16()
        arr_cv = cv2.imread("testData/test_depth1.png", cv2.IMREAD_ANYDEPTH)
        arr_cv_reshaped = arr_cv.reshape(w, h, c)
        self.assertTrue(arr_cv_reshaped.dtype == np_depth_arr1.dtype == np.uint16)
        self.assertTrue(arr_cv_reshaped.dtype == np_depth_arr2.dtype == np.uint16)
        self.assertTrue(arr_cv_reshaped.dtype == np_depth_arr3.dtype == np.uint16)
        self.assertTrue(arr_cv_reshaped.shape[0] == np_depth_arr1.shape[0] == np_depth_arr2.shape[0] == np_depth_arr3.shape[0])
        self.assertTrue(arr_cv_reshaped.shape[1] == np_depth_arr1.shape[1] == np_depth_arr2.shape[1] == np_depth_arr3.shape[1])
        self.assertTrue(arr_cv_reshaped.shape[2] == np_depth_arr1.shape[2] == np_depth_arr2.shape[2] == np_depth_arr3.shape[2])
        self.assertTrue(np.equal(image_np, np_depth_arr1).all())
        self.assertTrue(np.equal(arr_cv_reshaped, np_depth_arr1).all())
        self.assertTrue(np.equal(arr_cv_reshaped, np_depth_arr2).all())
        self.assertTrue(np.equal(arr_cv_reshaped, np_depth_arr3).all())
        self.assertTrue(np.equal(np_depth_arr1, np_depth_arr2).all())
        self.assertTrue(image_with_set_data.getFormat() == depth_image_load.getFormat())
        self.assertTrue(image_with_set_data.getWidth() == depth_image_load.getWidth())
        self.assertTrue(image_with_set_data.getHeight() == depth_image_load.getHeight())
        self.assertTrue(image_with_set_data.getChannelCount() == depth_image_load.getChannelCount())
        self.assertTrue(image_with_set_data.getBitDepth() == depth_image_load.getBitDepth())
        self.assertTrue(image_with_set_data.getByteDepth() == depth_image_load.getByteDepth())
        self.assertTrue(image_with_set_data.getChannelSize() == depth_image_load.getChannelSize())
        self.assertTrue(image_with_set_data.getChannelStep() == depth_image_load.getChannelStep())

    def test_set_data(self):
        print("Tests for image.setData are enabled.")
        test_image1 = f.Image()
        test_image2 = f.Image()
        test_image1.load("testData/overlap_image1.jpg")
        test_image2.load("testData/overlap_image1_copy.jpg")
        image_np_rgb_1 = test_image1.getData()
        image_np_rgb_2 = test_image2.getData()
        self.assertTrue(np.array_equal(image_np_rgb_1, image_np_rgb_2))

        test_image2.setData(image_np_rgb_2, f.FormatType.R8G8B8X8)
        image_np_rgbx_2 = test_image2.getData()
        _, _, c = image_np_rgbx_2.shape
        self.assertEqual(c, test_image2.getChannelCount())
        self.assertEqual(f.FormatType.R8G8B8X8, test_image2.getFormat())
        self.assertEqual("FormatType.R8G8B8X8", str(test_image2.getFormat()))
        image_np_rgbx_3 = test_image2.getData()
        self.assertTrue(np.array_equal(image_np_rgb_2, image_np_rgb_1))
        self.assertTrue(np.array_equal(image_np_rgbx_3, image_np_rgbx_2))

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

    def save_image_with_params(self, _new_file_path):
        _save_error = image.save(_new_file_path)
        self.assertTrue(os.path.isfile(_new_file_path))
        self.assertEqual(_save_error.isOk, 1)
        self.assertEqual(_save_error.isError, 0)
        self.assertEqual(_save_error.what, 'Ok')
        self.assertEqual(_save_error.error, f.ImageError.Ok)
        _save_error = image.save(_new_file_path)
        self.assertEqual(_save_error.isOk, 1)
        _save_error = image.save(_new_file_path, f.ImageCompression.IC_NO_COMPRESSION)
        self.assertEqual(_save_error.isOk, 1)
        _save_error = image.save(_new_file_path, f.ImageCompression.IC_SMALL_COMPRESSION)
        self.assertEqual(_save_error.isOk, 1)
        _save_error = image.save(_new_file_path, f.ImageCompression.IC_MEDIUM_COMPRESSION)
        self.assertEqual(_save_error.isOk, 1)
        _save_error = image.save(_new_file_path, f.ImageCompression.IC_HARD_COMPRESSION)
        self.assertEqual(_save_error.isOk, 1)
        _save_error = image.save(_new_file_path, f.ImageCompression.IC_BEST_COMPRESSION)
        self.assertEqual(_save_error.isOk, 1)
        os.remove(_new_file_path)

    def test_save(self):
        self.save_image_with_params("testData/test-warp1.png")
        self.save_image_with_params("testData/test-warp1.jpg")
        # print("deleting of saved {0}".format(new_file_path))

    def compare_image_sizes(self, image1, image2):
        self.assertTrue(image1.isValid())
        self.assertTrue(image2.isValid())
        self.assertTrue(image1.getWidth(), image2.getWidth())
        self.assertTrue(image1.getHeight(), image2.getHeight())

    def convert_image(self, src_image):
        err_convert, dest_image = src_image.convert(f.FormatType.B8G8R8X8)
        self.assertTrue(err_convert.isOk)
        self.assertEqual(dest_image.getFormat(), f.FormatType.B8G8R8X8)
        self.compare_image_sizes(src_image, dest_image)

        err_convert, dest_image = src_image.convert(f.FormatType.R8G8B8X8)
        self.assertTrue(err_convert.isOk)
        self.assertEqual(dest_image.getFormat(), f.FormatType.R8G8B8X8)
        self.compare_image_sizes(src_image, dest_image)

        err_convert, dest_image = src_image.convert(f.FormatType.B8G8R8)
        self.assertTrue(err_convert.isOk)
        self.assertEqual(dest_image.getFormat(), f.FormatType.B8G8R8)
        self.compare_image_sizes(src_image, dest_image)

        err_convert, dest_image = src_image.convert(f.FormatType.R8G8B8)
        self.assertEqual(dest_image.getFormat(), f.FormatType.R8G8B8)
        self.assertTrue(err_convert.isOk)
        self.compare_image_sizes(src_image, dest_image)

        err_convert, dest_image = src_image.convert(f.FormatType.R8)
        self.assertTrue(err_convert.isOk)
        self.assertEqual(dest_image.getFormat(), f.FormatType.R8)
        self.compare_image_sizes(src_image, dest_image)

        # IR_X8X8X8 is special IR format, it is not recommended to convert ordinary images to this format
        err_convert, dest_image = src_image.convert(f.FormatType.IR_X8X8X8)
        self.assertTrue(err_convert.isOk)
        self.assertEqual(dest_image.getFormat(), f.FormatType.IR_X8X8X8)
        self.compare_image_sizes(src_image, dest_image)

        # R16 is special depth map format, it is impossible to convert standard image to R16
        err_convert, dest_image = src_image.convert(f.FormatType.R16)
        self.assertTrue(err_convert.isError)

    def test_convert(self):
        src_image = f.Image()
        err_load = src_image.load("testData/warp1.ppm")
        self.assertTrue(err_load.isOk)
        self.convert_image(src_image)
        err_load = src_image.load("testData/warp1.ppm", f.FormatType.R8)
        self.assertTrue(err_load.isOk)
        self.convert_image(src_image)
        err_load = src_image.load("testData/warp1.ppm", f.FormatType.R8G8B8X8)
        self.assertTrue(err_load.isOk)
        self.convert_image(src_image)
        err_load = src_image.load("testData/warp1.ppm", f.FormatType.R8G8B8X8)
        self.assertTrue(err_load.isOk)
        self.convert_image(src_image)
        err_load = src_image.load("testData/warp1.ppm", f.FormatType.B8G8R8)
        self.assertTrue(err_load.isOk)
        self.convert_image(src_image)
        err_load = src_image.load("testData/warp1.ppm", f.FormatType.IR_X8X8X8)
        self.assertTrue(err_load.isOk)
        self.convert_image(src_image)


class ExpectedFailureTestCase(unittest.TestCase):
    @unittest.expectedFailure
    def test_fail(self):
        self.assertEqual(load_error_Unknown.isOk, 0, "broken")
        # R16 is used for internal transformation for depth map and cannot be saved or downloaded
        self.assertEqual(load_error_R16.isOk, 1, "R16 cannot be saved or downloaded")



if __name__ == '__main__':
    unittest.main()

