import unittest
import os
import xml.etree.cElementTree as ET
import time
from license_helper import make_activation, ActivationLicenseError


# if FaceEngine is installed only this string of code is required for module importing
import FaceEngine as fe
import TrackEngine as te

def change_value_in_trackengine_conf(param_name, key, new_value, section_name):
    tree = ET.ElementTree(file='data/trackengine.conf')
    elem = tree.find(".//*[@name='{0}']/param[@name='{1}']".format(section_name, param_name))
    if elem is None:
        print("{0} in {1} not found".format(param_name, section_name))
        exit(1)
    if elem.get(key) is None:
        print("{0} not found in {1}".format(key, param_name))
        exit(1)
    elem.set(key, new_value)
    tree.write('data/trackengine.conf', "UTF-8")


class TestTrackEngine(unittest.TestCase):
    faceEngine = None
    trackEngine = None

    @classmethod
    def setUpClass(cls):
        cls.faceEngine = fe.createFaceEngine("data")
        if not make_activation(cls.faceEngine):
            raise ActivationLicenseError("License is not activated!")
        cls.image = fe.Image()

    def setUp(self):
        self.configPath = os.path.join("data", "faceengine.conf")
        self.config = fe.createSettingsProvider(self.configPath)
        change_value_in_trackengine_conf('detector-step', 'x', '7', section_name='other')
        change_value_in_trackengine_conf('skip-frames', 'x', '36', section_name='other')
        change_value_in_trackengine_conf('tracker-type', 'text', 'kcf', section_name='other')
        change_value_in_trackengine_conf('frg-subtractor', 'x', "0", section_name="other")
        change_value_in_trackengine_conf("detector-scaling", "x", "0", section_name="other")
        change_value_in_trackengine_conf("scale-result-size", "x", "640", section_name="other")
        change_value_in_trackengine_conf("minimal-track-length", "x", "1", section_name="other")

    def push_frames(self, stream, image, start_frame_number, end_frame_number):
        for x in range(start_frame_number, end_frame_number):
            time_elapsed = 0.0
            while not (stream.pushFrame(image, x)) and time_elapsed < 10:
                time.sleep(0.01)
                time_elapsed += 0.01
            if time_elapsed < 10:
                print("pushed {0}".format(x), flush=True)
            else:
                print("Timeout expired while pushing frame.")
                exit(1)

    def load_image(self, image_path):
        image_path = image_path
        image = fe.Image()
        err_image_loaded = image.load(image_path)
        self.assertTrue(err_image_loaded.isOk,
                        "Error while loading image\n{}".format(err_image_loaded.error.name))
        return image

