import time
from trackengine_base_class import TestTrackEngine
from trackengine_base_class import change_value_in_trackengine_conf


import FaceEngine as fe
import TrackEngine as te


class TestTrackEngineSkipFrames(TestTrackEngine):

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
                self.assertTrue(err_image_loaded.isOk, "Error while loading image\n{}".format(err_image_loaded.error.name))
                self.assertTrue(err_image2_loaded.isOk, "Error while loading image\n{}".format(err_image2_loaded.error.name))

                stream = self.trackengine.createStream()
                print('Stream created!')
                for x in range(0, max_frames_det):
                    time_elapsed = 0.0
                    while not (stream.pushFrame(image_with_detection, x)) and time_elapsed < 10:
                        time.sleep(0.01)
                        time_elapsed += 0.01
                    if time_elapsed < 10:
                        print("pushed {0}".format(x), flush=True)
                    else:
                        print("Timeout expired while pushing frame.")
                        exit(1)
                print('Pushing frames with no detection')
                for x in range(max_frames_det, max_frames_det + max_frames_no_det):
                    time_elapsed = 0.0
                    while not (stream.pushFrame(image_without_detection, x)) and time_elapsed < 10:
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
                self.assertEqual(bestshot_cb_count, max_frames_det - int(min_track_length) + 1,
                                 "ctBestShot callbacks count dont match the expected value!")

                if int(case['skip-frames']) < max_frames_no_det:
                    self.assertIn('ctTrackEnd', str(clb[-1].type), "There were no ctTrackEnd callback!")
                else:
                    self.assertNotIn('ctTrackEnd', str(clb[-1].type), "Unexpectedly got ctTrackEnd callback!")
