from trackengine_base_class import TestTrackEngine
from trackengine_base_class import change_value_in_trackengine_conf
import time

import FaceEngine as fe
import TrackEngine as te


class TestTrackEngineMinimalTrackLength(TestTrackEngine):

    def testMinimalTrackLength(self):
        test_values = {"FaceDetV1": (("10", 11), ("5", 16), ("19", 2)),
                       "FaceDetV2": (("10", 11), ("5", 16), ("19", 2)),
                       "FaceDetV3": (("10", 11), ("5", 16), ("19", 2))}
        for detector, track in test_values.items():
            for track_lenght, ref_bestshot in track:
                with self.subTest(detector_type=detector):
                    change_value_in_trackengine_conf('minimal-track-length', 'x', track_lenght, section_name="other")
                    self.config.setValue('system', 'defaultDetectorType', detector)
                    self.faceEngine.setSettingsProvider(self.config)
                    trackEngine = te.createTrackEngine(self.faceEngine, "data/trackengine.conf")
                    stream = trackEngine.createStream()
                    image = self.load_image("testData/image_1080_1_face.jpg")
                    self.push_frames(stream, image, 0, 20)
                    stream.waitStream()
                    callbacks = stream.getCallbacks()
                    number_of_bestshot = 0
                    for callback in callbacks:
                        if callback.type.name == 'ctBestShot':
                            number_of_bestshot += 1
                    self.assertEqual(number_of_bestshot, ref_bestshot)
