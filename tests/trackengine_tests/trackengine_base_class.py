import unittest
import argparse
import sys
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
        #при frg-subtractor=1 в детектор отправляются только области с движением
        change_value_in_trackengine_conf('frg-subtractor', 'x', "0", section_name="other")
        change_value_in_trackengine_conf("detector-scaling", "x", "0", section_name="other")
        #change_value_in_trackengine_conf('use-preprocessing-thread', 'x', "0", section_name="other")