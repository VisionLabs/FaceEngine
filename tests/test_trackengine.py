import unittest
import argparse
import sys
import os
import xml.etree.cElementTree as ET
import time

from license_helper import make_activation, ActivationLicenseError

# if FaceEngine is not installed within the system, add the directory with FaceEngine*.so to system paths
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

sys.path.append(path_to_binding)

# if FaceEngine is installed only this string of code is required for module importing
import FaceEngine as fe
import TrackEngine as te

# erase two first arguments for unittest argument parsing
del (sys.argv[1])
del (sys.argv[1])


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
        #change_value_in_trackengine_conf('use-preprocessing-thread', 'x', "0", section_name="other")

    def testMinimalTrackLength(self):
        test_values = {"FaceDetV1": (("10", 10), ("5", 15), ("19", 1)),
                       "FaceDetV2": (("10", 10), ("5", 15), ("19", 1)),
                       "FaceDetV3": (("10", 10), ("5", 15), ("19", 1))}
        for detector, track in test_values.items():
            for track_lenght, ref_bestshot in track:
                with self.subTest(detector_type=detector):
                    change_value_in_trackengine_conf('minimal-track-length', 'x', track_lenght, section_name="other")
                    self.config.setValue('system', 'defaultDetectorType', detector)
                    self.faceEngine.setSettingsProvider(self.config)
                    trackEngine = te.createTrackEngine(self.faceEngine, "data/trackengine.conf")
                    stream = trackEngine.createStream()
                    err = self.image.load("testData/image_480.jpg")
                    self.assertTrue(err.isOk)

                    for i in range(1, 20):
                        while not (stream.pushFrame(self.image, i)):
                            time.sleep(0.01)
                    stream.waitStream()
                    callback = stream.getCallbacks()
                    number_of_bestshot = 0
                    for c in callback:
                        if c.type.name == 'ctBestShot':
                            number_of_bestshot += 1
                    self.assertEqual(number_of_bestshot, ref_bestshot)

    def testTrackerType(self):
        tracker_types = {'kcf', 'none'}
        for type in tracker_types:
            with self.subTest(tracker_type=type):
                change_value_in_trackengine_conf('minimal-track-length', 'x', "1", section_name="other")
                change_value_in_trackengine_conf('tracker-type', 'text', type, section_name="other")
                change_value_in_trackengine_conf('detector-step', 'x', "1", section_name="other")
                self.config.setValue('system', 'defaultDetectorType', "FaceDetV3")
                self.faceEngine.setSettingsProvider(self.config)
                trackEngine = te.createTrackEngine(self.faceEngine, "data/trackengine.conf")
                stream = trackEngine.createStream()
                err = self.image.load("testData/image_480.jpg")
                self.assertTrue(err.isOk)

                for i in range(1, 6):
                    push = stream.pushFrame(self.image, i)
                    self.assertTrue(push)
                stream.waitStream()
                callback = stream.getCallbacks()
                self.assertNotEqual(len(callback), 0)
                number_of_frame = 0
                ref_bbox = fe.Rect(246, 43, 214, 313)
                for c in callback:
                    number_of_frame += 1
                    self.assertEqual(c.bbox, ref_bbox)
                self.assertEqual(number_of_frame, 10)

    def test_skip_frames(self):
        cases = (
            {'skip-frames': '3'},
            {'skip-frames': '6'}
        )
        for case in cases:
            with self.subTest(case=case):
                min_track_length = '5'
                change_value_in_trackengine_conf("skip-frames", "x", case['skip-frames'], section_name="other")
                change_value_in_trackengine_conf("minimal-track-length", "x", min_track_length, section_name="other")
                self.trackengine = te.createTrackEngine(self.faceEngine, "data/trackengine.conf")
                max_frames_det = 10
                max_frames_no_det = 5
                image_with_detection_path = 'testData/image_1080_1_face.jpg'
                image_without_detection_path = 'testData/black_1080.jpg'
                image_with_detection = fe.Image()
                image_without_detection = fe.Image()
                err_image_loaded = image_with_detection.load(image_with_detection_path)
                err_image2_loaded = image_without_detection.load(image_without_detection_path)
                self.assertTrue(err_image_loaded.isOk)
                self.assertTrue(err_image2_loaded.isOk)

                stream = self.trackengine.createStream()
                print('Stream created!')
                for x in range(0, max_frames_det):
                    while not (stream.pushFrame(image_with_detection, x)):
                        time.sleep(0.01)
                    print("pushed {0}".format(x), flush=True)
                print('Pushing frames with no detection')
                for x in range(max_frames_det, max_frames_det + max_frames_no_det):
                    while not (stream.pushFrame(image_without_detection, x)):
                        time.sleep(0.01)
                    print("pushed {0}".format(x), flush=True)

                print("All frames are pushed")
                stream.waitStream()
                clb = stream.getCallbacks()
                for c in clb:
                    print(c.type)
                    print(c.bbox)

                expected_callback_count = (max_frames_det - int(min_track_length) + 1) + \
                                          min(int(case['skip-frames']) + 1, max_frames_no_det)
                visual_cb_count = 0
                for callback in clb:
                    if 'ctVisual' in str(callback.type):
                        visual_cb_count += 1
                self.assertEqual(visual_cb_count, expected_callback_count,
                                 "ctVisual callbacks count dont match the expected value!")
                bestshot_cb_count = 0
                for callback in clb:
                    if 'ctBestShot' in str(callback.type):
                        bestshot_cb_count += 1
                self.assertEqual(bestshot_cb_count, expected_callback_count,
                                 "ctBestShot callbacks count dont match the expected value!")

                if int(case['skip-frames']) < max_frames_no_det:
                    self.assertIn('ctTrackEnd', str(clb[-1].type), "There were no ctTrackEnd callback!")
                else:
                    self.assertNotIn('ctTrackEnd', str(clb[-1].type), "Unexpectedly got ctTrackEnd callback!")

    def test_detector_scaling(self):
        cases = (
            {'scale_size': "640", "frames": 5, "expected": 5},
            {'scale_size': "240", "frames": 5, "expected": 0}
        )
        for case in cases:
            with self.subTest(case=case):
                change_value_in_trackengine_conf("detector-scaling", "x", "1", section_name="other")
                change_value_in_trackengine_conf("scale-result-size", "x", case['scale_size'], section_name="other")
                self.trackengine = te.createTrackEngine(self.faceEngine, "data/trackengine.conf")
                image_full_hd_path = 'testData/image_1080_1_face.jpg'
                image_full_hd = fe.Image()
                err_image_loaded = image_full_hd.load(image_full_hd_path)
                self.assertTrue(err_image_loaded.isOk)
                stream = self.trackengine.createStream()
                print('Stream created!')
                for x in range(0, case['frames']):
                    while not (stream.pushFrame(image_full_hd, x)):
                        time.sleep(0.01)
                    print("pushed {0}".format(x), flush=True)
                print("All frames are pushed")
                stream.waitStream()
                clb = stream.getCallbacks()
                bestshots = 0
                for c in clb:
                    if c.type.name == 'ctBestShot':
                        bestshots += 1
                    print(c.type)
                    print(c.bbox)
                self.assertEqual(bestshots, case['expected'], "Number of besthots dont match the number of frames.")


if __name__ == '__main__':
    unittest.main()