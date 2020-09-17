import unittest
import argparse
import sys
import os
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
import FaceEngine as f

# erase two first arguments for unittest argument parsing
del (sys.argv[1])
del (sys.argv[1])


class TestFaceEngineEstimators(unittest.TestCase):
    faceEngine = None
    config_desc = None
    config = None

    @classmethod
    def setUp(cls):
        cls.faceEngine = f.createFaceEngine("data")
        if not make_activation(cls.faceEngine):
            raise ActivationLicenseError("License is not activated!")
        cls.config_desc = f.createSettingsProvider("data/cnndescriptor_54.conf")
        cls.config = f.createSettingsProvider("data/faceengine.conf")

    def testGetDescriptorValue2i(self):
        value_2i = self.config_desc.getValue("descriptor", "inputSize")
        self.assertNotEqual(value_2i[0], 0)
        self.assertNotEqual(value_2i[1], 0)

    def testGetDescriptorValue4i(self):
        # you must not change this setting, it is only for research purposes
        value_4i = self.config_desc.getValue("descriptor", "centerROI")
        self.assertEqual(value_4i[0], 16)
        self.assertEqual(value_4i[1], 16)
        self.assertEqual(value_4i[2], 224)
        self.assertEqual(value_4i[3], 224)

    def testGetDescriptorValue3f(self):
        # you must not change this setting, it is only for research purposes
        value_3f = self.config_desc.getValue("descriptor", "mean")
        self.assertAlmostEqual(value_3f[0], 0.485, delta=0.0001)
        self.assertAlmostEqual(value_3f[1], 0.456, delta=0.0001)
        self.assertAlmostEqual(value_3f[2], 0.406, delta=0.0001)

    def testGetDescriptorValueBool(self):
        self.config_desc.setValue("descriptor", "filterAggregation", 0)
        value = self.config_desc.getValue("descriptor", "filterAggregation")
        self.assertEqual(value[0], 0)

    def testGetDescriptorValueStr(self):
        self.config_desc.setValue("descriptor", "gpuInt", "off")
        value_off = self.config_desc.getValue("descriptor", "gpuInt")
        self.assertEqual(value_off[0], "off")

    def testGetDescriptorValue2f(self):
        # you must not change this setting, it is only for research purposes
        value_2f = self.config_desc.getValue("PlattScaling", "params_L2")
        self.assertAlmostEqual(value_2f[0], 13.01686452, delta=0.00001)
        self.assertAlmostEqual(value_2f[1], -15.84096833, delta=0.00001)

    def testGetFaceEngineValueBool(self):
        self.config.setValue("system", "betaMode", True)
        value = self.config.getValue("system", "betaMode")
        self.assertEqual(value[0], True)

    def testGetFaceEngineValue1i(self):
        self.config.setValue("system", "verboseLogging", 4)
        value = self.config.getValue("system", "verboseLogging")
        self.assertEqual(value[0], 4)

    def testSetGetFaceEngineValueStr(self):
        self.config.setValue("system", "defaultDetectorType", "FaceDetV3")
        value = self.config.getValue("system", "defaultDetectorType")
        self.assertEqual(value[0], "FaceDetV3")

    def testGetFaceEngineValue1i(self):
        self.config.setValue("system", "verboseLogging", 4)
        value = self.config.getValue("system", "verboseLogging")
        self.assertEqual(value[0], 4)

    def testGetFaceEngineValue1f(self):
        # you must not change this setting, it is only for research purposes
        value = self.config.getValue("FaceDetV3::Settings", "ScoreThreshold")
        self.assertAlmostEqual(value[0], 0.89, delta=0.0001)

    def testGetFaceEngineValue4f(self):
        # you must not change this setting, it is only for research purposes
        value = self.config.getValue("FaceDetV1::Settings", "paddings")
        self.assertAlmostEqual(value[0], -0.20099958, delta=0.000001)
        self.assertAlmostEqual(value[1], 0.10210337, delta=0.000001)
        self.assertAlmostEqual(value[2], 0.20363552, delta=0.000001)
        self.assertAlmostEqual(value[3], 0.08490226, delta=0.000001)

    def testGetFaceEngineValue4fDeprecated(self):
        self.config.setValue("system", "verboseLogging", f.SettingsProviderValue(2))
        value = self.config.getValue("system", "verboseLogging")
        self.assertEqual(value[0], 2)

    def testGetSettingProvider(self):
        config1 = self.faceEngine.getSettingsProvider()
        ref_value_i = 4

        config1.setValue("system", "verboseLogging", ref_value_i)
        value1 = config1.getValue("system", "verboseLogging")
        self.assertEqual(value1[0], ref_value_i)

        # you must not change this setting, it is only for research purposes
        ref_value_f = 0.1
        config1.setValue("FaceDetV1::Settings", "paddings", [ref_value_f, ref_value_f, ref_value_f, ref_value_f])

        self.faceEngine.setSettingsProvider(config1)
        config2 = self.faceEngine.getSettingsProvider()
        value2 = config2.getValue("system", "verboseLogging")
        self.assertEqual(value2[0], ref_value_i)

        value_4f = config2.getValue("FaceDetV1::Settings", "paddings")
        self.assertAlmostEqual(value_4f[0], ref_value_f, delta=0.000001)
        self.assertAlmostEqual(value_4f[1], ref_value_f, delta=0.000001)
        self.assertAlmostEqual(value_4f[2], ref_value_f, delta=0.000001)
        self.assertAlmostEqual(value_4f[3], ref_value_f, delta=0.000001)

    def testGetRuntimeSettingProvider(self):
        config1 = self.faceEngine.getRuntimeSettingsProvider()
        ref_value_i = 4

        config1.setValue("Runtime", "numThreads", ref_value_i)
        value1 = config1.getValue("Runtime", "numThreads")
        self.assertEqual(value1[0], ref_value_i)

        ref_value_logging = 1
        config1.setValue("Runtime", "verboseLogging", [ref_value_logging])

        config2 = self.faceEngine.getRuntimeSettingsProvider()
        value2 = config2.getValue("Runtime", "verboseLogging")
        self.assertEqual(value2[0], ref_value_logging)


if __name__ == '__main__':
    unittest.main()

