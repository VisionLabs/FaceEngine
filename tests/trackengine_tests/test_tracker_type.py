from trackengine_base_class import TestTrackEngine
from trackengine_base_class import change_value_in_trackengine_conf
import time

import FaceEngine as fe
import TrackEngine as te


class TestTrackEngineTrackerType(TestTrackEngine):

    def testTrackerType(self):
        tracker_types = {'kcf', 'none'}
        for type in tracker_types:
            with self.subTest(tracker_type=type):
                change_value_in_trackengine_conf('tracker-type', 'text', type, section_name="other")
                change_value_in_trackengine_conf('detector-step', 'x', "1", section_name="other")
                self.config.setValue('system', 'defaultDetectorType', "FaceDetV3")
                self.faceEngine.setSettingsProvider(self.config)
                trackEngine = te.createTrackEngine(self.faceEngine, "data/trackengine.conf")
                image = self.load_image("testData/image_480.jpg")
                stream = trackEngine.createStream()
                self.push_frames(stream, image, 1, 6)
                stream.waitStream()
                callbacks = stream.getCallbacks()
                self.assertNotEqual(len(callbacks), 0)
                number_of_frame = 0
                ref_bbox = callbacks[0].bbox
                for callback in callbacks:
                    number_of_frame += 1
                    self.assertEqual(callback.bbox, ref_bbox)
                self.assertEqual(number_of_frame, 10)
