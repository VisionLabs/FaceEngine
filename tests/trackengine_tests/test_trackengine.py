import unittest
import argparse
import sys
import os

from trackengine_tests.test_detector_scaling import TestTrackEngineDetectorScaling
from trackengine_tests.test_minimal_track_length import TestTrackEngineMinimalTrackLength
from trackengine_tests.test_skip_frames import TestTrackEngineSkipFrames
from trackengine_tests.test_tracker_type import TestTrackEngineTrackerType


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

# erase two first arguments for unittest argument parsing
del (sys.argv[1])
del (sys.argv[1])


def load_tests(loader, tests, pattern):
    suite = unittest.TestSuite()
    test_classes = (
        TestTrackEngineDetectorScaling,
        TestTrackEngineMinimalTrackLength,
        TestTrackEngineSkipFrames,
        TestTrackEngineTrackerType
    )
    for test_class in test_classes:
        tests = loader.loadTestsFromTestCase(test_class)
        suite.addTests(tests)
    return suite


if __name__ == '__main__':
    unittest.main()
