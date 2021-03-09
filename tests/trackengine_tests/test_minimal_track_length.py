from trackengine_base_class import TestTrackEngine
from trackengine_base_class import change_value_in_trackengine_conf
import time

import FaceEngine as fe
import TrackEngine as te


class TestTrackEngineMinimalTrackLength(TestTrackEngine):

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
