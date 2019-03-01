import unittest
import argparse
import sys
import os
import logging
import struct

# if FaceEngine is not installed within the system, add the directory with FaceEngine*.so to system paths
parser = argparse.ArgumentParser()
parser.add_argument("-b", "--bind-path", type=str,
                    help="path to FaceEngine*.so file - binding of luna-sdk")

args = parser.parse_args()
path_to_binding = args.bind_path
print(path_to_binding)
if not os.path.isdir(path_to_binding):
    print("Directory with FaceEngine*.so was not found.")
    exit(1)


print("Directory {0} with python bindings of FaceEngine was included".format(path_to_binding))
print(sys.argv)

sys.path.append(path_to_binding)

# if FaceEngine is installed only this string of code is required for module importing
import FaceEngine as fe

# erase two first arguments for unittest argument parsing
del(sys.argv[1])
del(sys.argv[1])

faceEngine = fe.createFaceEngine("data", "data/faceengine.conf")

test_data_path = "testData"
dataPath = "data"

class TestFaceEngineRect(unittest.TestCase):

    # helpers
    def are_equal(self, desc1, desc2):
        self.assertTrue(len(desc1) == len(desc2))
        for i, _ in enumerate(desc1):
            self.assertTrue(desc1[i], desc2[i])

    def test_Version(self):
        extractor = faceEngine.createExtractor()
        matcher = faceEngine.createMatcher()
        descriptor = faceEngine.createDescriptor()
        aggregation = faceEngine.createDescriptor()
        batch_size = 2
        descriptorBatch = faceEngine.createDescriptorBatch(2)
        images = []
        for i in range(batch_size):
            image = fe.Image()
            err = image.load(os.path.join(test_data_path, "warp1.ppm"))
            self.assertTrue(err.isOk)
            images.append(image)
        self.assertTrue(extractor.extractFromWarpedImage(images[0], descriptor))
        self.assertTrue(extractor.extractFromWarpedImageBatch(images, descriptorBatch, aggregation, batch_size))

        def assertMatchingResult(result):
            self.assertEqual(result.similarity, 1.0)
            self.assertEqual(result.distance, 0.0)

        def checkDescriptorsEquality(desc1, desc2):
            result = matcher.match(desc1, desc2)
            self.assertTrue(result.isOk)
            assertMatchingResult(result.value)

        checkDescriptorsEquality(aggregation, descriptor)
        checkDescriptorsEquality(aggregation, aggregation)
        result = matcher.match(aggregation, descriptorBatch)
        self.assertTrue(result[0].isOk)
        assertMatchingResult(result[1][0])
        assertMatchingResult(result[1][1])

    def extractor(self, version, refGS, useMobileNet, cpuType, device):
        versionString = str(version) + ("", "_mobilenet")[useMobileNet]
        configPath = os.path.join(dataPath, "faceengine.conf")
        faceEngine = fe.createFaceEngine(dataPath)
        config = fe.createSettingsProvider(configPath)
        config.setValue("DescriptorFactory::Settings", "model", fe.SettingsProviderValue(version))
        config.setValue("DescriptorFactory::Settings", "useMobileNet", fe.SettingsProviderValue(useMobileNet))
        config.setValue("flower", "deviceClass", fe.SettingsProviderValue(device))
        config.setValue("flower", "verboseLogging", fe.SettingsProviderValue(4))
        config.setValue("system", "cpuClass", fe.SettingsProviderValue(cpuType))
        config.setValue("system", "verboseLogging", fe.SettingsProviderValue(5))

        faceEngine.setSettingsProvider(config)
        warp = fe.Image()
        err = warp.load(os.path.join(test_data_path, "warp1.bmp"))
        self.assertTrue(err.isOk)
        warp.save(os.path.join(test_data_path, "outbmp.bmp"))
        extractor = faceEngine.createExtractor()
        descriptor = faceEngine.createDescriptor()
        res = extractor.extractFromWarpedImage(warp, descriptor)
        self.assertTrue(res.isOk)
        data1 = descriptor.getData()
        with open(test_data_path + "/descriptor1_" + versionString + "_actual.bin", "wb") as out_file:
            out_file.write(data1)
        self.assertAlmostEqual(refGS, res.value, delta=(0.02, 0.03)[useMobileNet])
        refPath = os.path.join(test_data_path, "descriptor1_" + versionString + ".bin")
        with open(refPath, "rb") as file:
            read_data = file.read()
            print(len(read_data))
            descriptorExpected = faceEngine.createDescriptor()
            descriptorExpected.load(read_data, len(read_data))
            dataActual = descriptor.getData()
            dataExpected = descriptorExpected.getData()
            self.assertEqual(descriptorExpected.getModelVersion(), descriptor.getModelVersion())
            for i in range(descriptor.getDescriptorLength()):
                self.assertEqual(dataActual[i], dataExpected[i])

            # by default full version with headears
            _, full_data_actual_default1 = descriptor.save()
            _, full_data_actual_default2 = descriptor.save(fe.Save.Default)
            _, full_data_actual_no_signature = descriptor.save(fe.Save.NoSignature)

            _, full_data_exp_default1 = descriptorExpected.save()
            _, full_data_exp_default2 = descriptorExpected.save(fe.Save.Default)
            _, full_data_exp_no_signature = descriptorExpected.save(fe.Save.NoSignature)

            diff_actual_default1 = len(full_data_actual_default1) - len(dataActual)
            diff_actual_default2 = len(full_data_actual_default2) - len(dataActual)
            diff_actual_no_signature = len(full_data_actual_no_signature) - len(dataActual)
            diff_exp_default1 = len(full_data_exp_default1) - len(dataActual)
            diff_exp_default2 = len(full_data_exp_default2) - len(dataActual)
            diff_exp_no_signature = len(full_data_exp_no_signature) - len(dataActual)
            for i in range(descriptor.getDescriptorLength()):
                self.assertEqual(dataActual[i], full_data_actual_default1[i + diff_actual_default1])
                self.assertEqual(dataActual[i], full_data_actual_default2[i + diff_actual_default2])
                self.assertEqual(dataActual[i], full_data_exp_no_signature[i + diff_actual_no_signature])

                self.assertEqual(dataExpected[i], full_data_exp_default1[i + diff_exp_default1])
                self.assertEqual(dataExpected[i], full_data_exp_default2[i + diff_exp_default2])
                self.assertEqual(dataExpected[i], full_data_exp_no_signature[i + diff_exp_no_signature])


    def test_extractor(self):
        self.extractor(51, 0.9718, True, "auto", "cpu")
        self.extractor(46, 0.9718, True, "auto", "cpu")
        self.extractor(52, 1.0, True, "auto", "cpu")
        self.extractor(46, 0.9718, False, "auto", "cpu")
        self.extractor(51, 0.9718, False, "auto", "cpu")
        self.extractor(46, 0.9718, False, "cpu", "cpu")
        self.extractor(46, 0.9718, True, "cpu", "cpu")
        self.extractor(51, 0.9718, False, "cpu", "cpu")
        self.extractor(51, 0.9718, True, "cpu", "cpu")
        self.extractor(52, 0.8926, False, "cpu", "cpu")
        self.extractor(52, 1.0, True, "cpu", "cpu")
        self.extractor(46, 0.9718, False, "avx2", "cpu")
        self.extractor(46, 0.9718, True, "avx", "cpu")
        self.extractor(51, 0.9718, False, "avx", "cpu")
        self.extractor(51, 0.9718, False, "avx2", "cpu")
        self.extractor(51, 0.9718, True, "avx", "cpu")
        self.extractor(51, 0.9718, True, "avx2", "cpu")
        self.extractor(52, 0.8926, False, "avx", "cpu")
        self.extractor(52, 0.8926, False, "avx2", "cpu")
        self.extractor(52, 1.0, True, "avx", "cpu")
        self.extractor(52, 1.0, True, "avx2", "cpu")

    def extractor_batch(self, version, useMobileNet, cpuType, device):
        configPath = os.path.join(dataPath, "faceengine.conf")
        faceEngine = fe.createFaceEngine(dataPath)
        config = fe.createSettingsProvider(configPath)
        config.setValue("DescriptorFactory::Settings", "model", fe.SettingsProviderValue(version))
        config.setValue("DescriptorFactory::Settings", "useMobileNet", fe.SettingsProviderValue(useMobileNet))
        config.setValue("flower", "deviceClass", fe.SettingsProviderValue(device))
        config.setValue("system", "cpuClass", fe.SettingsProviderValue(cpuType))
        config.setValue("system", "verboseLogging", fe.SettingsProviderValue(5))
        faceEngine.setSettingsProvider(config)
        warps = [fe.Image(), fe.Image()]
        err1 = warps[0].load(os.path.join(test_data_path, "warp1.ppm"))
        err2 = warps[1].load(os.path.join(test_data_path, "warp2.ppm"))
        # warps[0].load(os.path.join(test_data_path, "warp1.ppm"))
        # warps[0].load(os.path.join(test_data_path, "warp_eyeglasses.jpg"))

        extractor = faceEngine.createExtractor()
        batch = faceEngine.createDescriptorBatch(2)
        descriptor = faceEngine.createDescriptor()

        res_batch = extractor.extractFromWarpedImageBatch(warps, batch, descriptor, 2)
        self.assertTrue(res_batch[0].isOk)
        with open(test_data_path + "/batch12_" + str(version) + "_actual.bin", "wb") as out_file:
            for i in range(2):
                descriptor_from_batch = batch.getDescriptorFast(i)
                out_file.write(descriptor_from_batch.getData())
        for i_desc in range(2):
            res = extractor.extractFromWarpedImage(warps[i_desc], descriptor)
            self.assertTrue(res.isOk)
            self.assertEqual(descriptor.getModelVersion(), batch.getModelVersion())
            dataExpected = descriptor.getData()
            dataActual = batch.getDescriptorFast(i_desc).getData()
            descLength = descriptor.getDescriptorLength()
            for j in range(descLength):
                self.assertEqual(dataExpected[j], dataActual[j])
            self.assertAlmostEqual(res.value, res_batch[1][i_desc], delta=0.0001)

    def test_extractor_batch(self):
        self.extractor_batch(46, True, "auto", "cpu")
        self.extractor_batch(46, False, "auto", "cpu")

    def extractor_aggregation(self, version, useMobileNet, cpuType, device):
        configPath = os.path.join(dataPath, "faceengine.conf")
        faceEngine = fe.createFaceEngine(dataPath)
        config = fe.createSettingsProvider(configPath)
        config.setValue("DescriptorFactory::Settings", "model", fe.SettingsProviderValue(version))
        config.setValue("DescriptorFactory::Settings", "useMobileNet", fe.SettingsProviderValue(useMobileNet))
        config.setValue("flower", "deviceClass", fe.SettingsProviderValue(device))
        config.setValue("system", "cpuClass", fe.SettingsProviderValue(cpuType))
        config.setValue("system", "verboseLogging", fe.SettingsProviderValue(5))
        faceEngine.setSettingsProvider(config)
        warps = [fe.Image(), fe.Image()]
        err1 = warps[0].load(os.path.join(test_data_path, "warp1.ppm"))
        err2 = warps[1].load(os.path.join(test_data_path, "warp2.ppm"))
        extractor = faceEngine.createExtractor()
        batch = faceEngine.createDescriptorBatch(2)
        descriptor = faceEngine.createDescriptor()
        aggr = faceEngine.createDescriptor()

        # res_batch = extractor.extractFromWarpedImageBatch(warps, None, aggr, 1)
        # self.assertFalse(res_batch.isError)
        res = extractor.extractFromWarpedImageBatch(warps, batch, aggr, 1)
        self.assertFalse(res[0].isError)
        res = extractor.extractFromWarpedImage(warps[0], descriptor)
        self.assertTrue(res.isOk)
        self.assertEqual(descriptor.getModelVersion(), batch.getModelVersion())
        data_expected = descriptor.getData()
        data_actual = batch.getDescriptorFast(0).getData()
        descLength = descriptor.getDescriptorLength()
        for j in range(descLength):
            self.assertEqual(data_expected[j], data_actual[j])

    def test_extractor_aggregation(self):
        self.extractor_aggregation(46, True, "auto", "cpu")
        self.extractor_aggregation(46, False, "auto", "cpu")

if __name__ == '__main__':
    unittest.main()

