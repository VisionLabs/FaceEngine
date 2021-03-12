import time
from trackengine_base_class import TestTrackEngine
from trackengine_base_class import change_value_in_trackengine_conf

import FaceEngine as fe
import TrackEngine as te


class TestTrackEngineSkipFrames(TestTrackEngine):

    def test_skip_frames_track_ended(self):
        cases = (
            {'skip-frames': '3'},
            {'skip-frames': '5'}
        )
        for case in cases:
            with self.subTest(case=case):
                change_value_in_trackengine_conf("skip-frames", "x", case['skip-frames'], section_name="other")
                self.trackengine = te.createTrackEngine(self.faceEngine, "data/trackengine.conf")
                image_with_detection = self.load_image('testData/image_1080_1_face.jpg')
                image_without_detection = self.load_image('testData/black_1080.jpg')
                stream = self.trackengine.createStream()
                self.push_frames(stream, image_with_detection, 0, 5)
                self.push_frames(stream, image_without_detection, 5, 11)
                stream.waitStream()
                callbacks = stream.getCallbacks()
                visual_cb_count = 0
                bestshot_cb_count = 0
                for callback in callbacks:
                    if 'ctVisual' in str(callback.type):
                        visual_cb_count += 1
                    elif 'ctBestShot' in str(callback.type):
                        bestshot_cb_count += 1
                expected_visual_callbacks = 5 + int(case['skip-frames']) + 1
                self.assertEqual(visual_cb_count, expected_visual_callbacks,
                                 "ctVisual callbacks count dont match the expected value!, expected {0}".format(
                                     expected_visual_callbacks))
                self.assertEqual(bestshot_cb_count, 5,
                                 "ctBestShot callbacks count dont match the expected value!")
                self.assertIn('ctTrackEnd', str(callbacks[-1].type), "There were no ctTrackEnd callback!")

    def test_skip_frames_track_not_ended(self):
        skip_frames = '10'
        change_value_in_trackengine_conf("skip-frames", "x", skip_frames, section_name="other")
        self.trackengine = te.createTrackEngine(self.faceEngine, "data/trackengine.conf")
        image_with_detection = self.load_image('testData/image_1080_1_face.jpg')
        image_without_detection = self.load_image('testData/black_1080.jpg')
        stream = self.trackengine.createStream()
        self.push_frames(stream, image_with_detection, 0, 5)
        self.push_frames(stream, image_without_detection, 5, 11)
        stream.waitStream()
        callbacks = stream.getCallbacks()
        visual_cb_count = 0
        bestshot_cb_count = 0
        for callback in callbacks:
            if 'ctVisual' in str(callback.type):
                visual_cb_count += 1
            elif 'ctBestShot' in str(callback.type):
                bestshot_cb_count += 1
        expected_visual_callbacks = 11
        self.assertEqual(visual_cb_count, expected_visual_callbacks,
                         "ctVisual callbacks count dont match the expected value!, expected {0}".format(
                             expected_visual_callbacks))
        self.assertEqual(bestshot_cb_count, 5,
                         "ctBestShot callbacks count dont match the expected value!")
        self.assertNotIn('ctTrackEnd', str(callbacks[-1].type), "Unexpectedly got ctTrackEnd callback!")
