import sys
import unittest
import argparse
import sys
import os

# if FaceEngine is not installed in system
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
faceEnginePtr = f.createPyFaceEnginePtr("../data",
                                        "../data/faceengine.conf")

image = f.Image()
loaded_image = f.Image()
load_error = image.load("../images/warp1.ppm")
print(load_error)
print(image.getWidth())
print(image.getHeight())
print(image.isValid())
print(image.getRect())
new_file_path = "../images/test-warp1.ppm"
save_error = image.save(new_file_path)

loaded_image.load(new_file_path)
print(image.getWidth())
print(image.getHeight())
print(image.isValid())
print(image.getRect())
print(os.path.isfile(new_file_path))
os.remove(new_file_path)
print(os.path.isfile(new_file_path))


class TestFaceEngineImage(unittest.TestCase):

    def test_load(self):
        self.assertTrue(load_error.isOk, 1)
        self.assertFalse(load_error.isError, 0)
        self.assertEqual(load_error.what, 'Ok')

    def test_save(self):
        self.assertTrue('FOO'.isupper())
        self.assertFalse('Foo'.isupper())

    def test_split(self):
        s = 'hello world'
        self.assertEqual(s.split(), ['hello', 'world'])
        # check that s.split fails when the separator is not a string
        with self.assertRaises(TypeError):
            s.split(2)


class ExpectedFailureTestCase(unittest.TestCase):
    @unittest.expectedFailure
    def test_fail(self):
        self.assertEqual(1, 0, "broken")


if __name__ == '__main__':
    unittest.main()