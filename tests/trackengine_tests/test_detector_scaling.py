import time
from trackengine_base_class import TestTrackEngine
from trackengine_base_class import change_value_in_trackengine_conf


import FaceEngine as fe
import TrackEngine as te


class TestTrackEngineDetectorScaling(TestTrackEngine):

    def test_detector_scaling(self):
        cases = (
            {'scale_size': "640", "frames": 5, "expected_detections": 5},
            {'scale_size': "240", "frames": 5, "expected_detections": 0}
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
                    time_elapsed = 0.0
                    while not (stream.pushFrame(image_full_hd, x)) and time_elapsed < 10:
                        time.sleep(0.01)
                        time_elapsed += 0.01
                    if time_elapsed < 10:
                        print("pushed {0}".format(x), flush=True)
                    else:
                        print("Timeout expired while pushing frame.")
                        exit(1)
                print("All frames are pushed")
                stream.waitStream()
                clb = stream.getCallbacks()
                bestshots = 0
                for c in clb:
                    if c.type.name == 'ctBestShot':
                        bestshots += 1
                    print(c.type)
                    print(c.bbox)
                self.assertEqual(bestshots, case['expected_detections'], "Number of besthots dont match the number of frames.")

    def test_detector_scaling_small_resolution(self):
        cases = (
            {'image': 'testData/image_640_32.jpg', 'expected_detections': 0},
            {'image': 'testData/image_32_32.jpg', 'expected_detections': 0}
        )
        for case in cases:
            with self.subTest(case=case):
                change_value_in_trackengine_conf("detector-scaling", "x", "1", section_name="other")
                self.trackengine = te.createTrackEngine(self.faceEngine, "data/trackengine.conf")
                image_path = case['image']
                image_object = fe.Image()
                err_image_loaded = image_object.load(image_path)
                self.assertTrue(err_image_loaded.isOk)
                stream = self.trackengine.createStream()
                print('Stream created!')
                for x in range(0, 5):
                    time_elapsed = 0.0
                    while not (stream.pushFrame(image_object, x)) and time_elapsed < 10:
                        time.sleep(0.01)
                        time_elapsed += 0.01
                    if time_elapsed < 10:
                        print("pushed {0}".format(x), flush=True)
                    else:
                        print("Timeout expired while pushing frame.")
                        exit(1)
                print("All frames are pushed")
                stream.waitStream()
                clb = stream.getCallbacks()
                visual_cb_count = 0
                for callback in clb:
                    if 'ctVisual' in str(callback.type):
                        visual_cb_count += 1
                self.assertEqual(visual_cb_count, case['expected_detections'],
                                 "ctVisual callbacks count dont match the expected value!")