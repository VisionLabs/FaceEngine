from trackengine_tests.trackengine_base_class import TestTrackEngine
from trackengine_tests.trackengine_base_class import change_value_in_trackengine_conf
import time

import FaceEngine as fe
import TrackEngine as te


class TestTrackEngineTrackerType(TestTrackEngine):

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
