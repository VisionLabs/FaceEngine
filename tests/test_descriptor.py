import unittest
import argparse
import sys
import os
import logging
import struct
from collections import OrderedDict

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
import FaceEngine as fe

# erase two first arguments for unittest argument parsing
del(sys.argv[1])
del(sys.argv[1])


class TestFaceEngineRect(unittest.TestCase):
    faceEngine = None
    test_data_path = "testData"
    dataPath = "data"

    @classmethod
    def setUp(cls):
        cls.faceEngine = fe.createFaceEngine("data")
        if not make_activation(cls.faceEngine):
            raise ActivationLicenseError("License is not activated!")

    # helpers
    def areEqual(self, desc1, desc2):
        self.assertTrue(len(desc1) == len(desc2))
        for i, _ in enumerate(desc1):
            self.assertTrue(desc1[i], desc2[i])

    def setLogging(self, value):
        config = fe.createSettingsProvider("data/faceengine.conf")
        config.setValue("system", "verboseLogging", fe.SettingsProviderValue(value))
        self.faceEngine.setSettingsProvider(config)

    def testVersion(self):

        # face descriptor
        extractor_default = self.faceEngine.createExtractor()
        matcher_default = self.faceEngine.createMatcher()
        descriptor_default = self.faceEngine.createDescriptor()
        aggregation_default = self.faceEngine.createDescriptor()

        extractor46 = self.faceEngine.createExtractor(46)
        matcher46 = self.faceEngine.createMatcher(46)
        descriptor46 = self.faceEngine.createDescriptor(46)
        aggregation46 = self.faceEngine.createDescriptor(46)

        extractor52 = self.faceEngine.createExtractor(52)
        matcher52 = self.faceEngine.createMatcher(52)
        descriptor52 = self.faceEngine.createDescriptor(52)
        aggregation52 = self.faceEngine.createDescriptor(52)

        extractor54 = self.faceEngine.createExtractor(54)
        matcher54 = self.faceEngine.createMatcher(54)
        descriptor54 = self.faceEngine.createDescriptor(54)
        aggregation54 = self.faceEngine.createDescriptor(54)

        extractor56 = self.faceEngine.createExtractor(56)
        matcher56 = self.faceEngine.createMatcher(56)
        descriptor56 = self.faceEngine.createDescriptor(56)
        aggregation56 = self.faceEngine.createDescriptor(56)

        # face descriptor
        self.assertEqual(46, extractor46.getModelVersion())
        self.assertEqual(46, matcher46.getModelVersion())
        self.assertEqual(46, descriptor46.getModelVersion())
        self.assertEqual(46, aggregation46.getModelVersion())
        self.assertEqual(fe.DT_FACE, extractor46.getDescriptorType())
        self.assertEqual(fe.DT_FACE, descriptor46.getDescriptorType())

        self.assertEqual(52, extractor52.getModelVersion())
        self.assertEqual(52, matcher52.getModelVersion())
        self.assertEqual(52, descriptor52.getModelVersion())
        self.assertEqual(52, aggregation52.getModelVersion())
        self.assertEqual(fe.DT_FACE, extractor52.getDescriptorType())
        self.assertEqual(fe.DT_FACE, descriptor52.getDescriptorType())

        self.assertEqual(54, extractor54.getModelVersion())
        self.assertEqual(54, matcher54.getModelVersion())
        self.assertEqual(54, descriptor54.getModelVersion())
        self.assertEqual(54, aggregation54.getModelVersion())
        self.assertEqual(fe.DT_FACE, extractor54.getDescriptorType())
        self.assertEqual(fe.DT_FACE, descriptor54.getDescriptorType())

        self.assertEqual(56, extractor56.getModelVersion())
        self.assertEqual(56, matcher56.getModelVersion())
        self.assertEqual(56, descriptor56.getModelVersion())
        self.assertEqual(56, aggregation56.getModelVersion())
        self.assertEqual(fe.DT_FACE, extractor56.getDescriptorType())
        self.assertEqual(fe.DT_FACE, descriptor56.getDescriptorType())

        # face descriptor
        batch_size = 2
        descriptorBatch = self.faceEngine.createDescriptorBatch(2)
        images = []
        for i in range(batch_size):
            image = fe.Image()
            err = image.load(os.path.join(self.test_data_path, "warp1.ppm"))
            self.assertTrue(err.isOk)
            images.append(image)
        self.assertTrue(extractor_default.extractFromWarpedImage(images[0], descriptor_default))
        self.assertTrue(extractor_default.extractFromWarpedImageBatch(images, descriptorBatch, aggregation_default, batch_size))

        def assertMatchingResult(result):
            self.assertEqual(result.similarity, 1.0)
            self.assertEqual(result.distance, 0.0)

        def checkDescriptorsEquality(desc1, desc2):
            result, value = matcher_default.match(desc1, desc2)
            self.assertTrue(result.isOk)
            assertMatchingResult(value)

        checkDescriptorsEquality(aggregation_default, descriptor_default)
        checkDescriptorsEquality(aggregation_default, aggregation_default)
        result = matcher_default.match(aggregation_default, descriptorBatch)
        self.assertTrue(result[0].isOk)
        assertMatchingResult(result[1][0])
        assertMatchingResult(result[1][1])

    def extractor(self, version, refGS, useMobileNet, cpuType, device):
        versionString = str(version) + ("", "_mobilenet")[useMobileNet]
        configPath = os.path.join(self.dataPath, "faceengine.conf")
        runtimeConfigPath = os.path.join(self.dataPath, "runtime.conf")

        faceEngine = fe.createFaceEngine(self.dataPath)
        self.assertTrue(make_activation(faceEngine))
        config = fe.createSettingsProvider(configPath)
        runtimeConf = fe.createSettingsProvider(runtimeConfigPath)

        config.setValue("DescriptorFactory::Settings", "model", fe.SettingsProviderValue(version))
        config.setValue("DescriptorFactory::Settings", "useMobileNet", fe.SettingsProviderValue(useMobileNet))
        config.setValue("system", "verboseLogging", fe.SettingsProviderValue(5))
        runtimeConf.setValue("Runtime", "deviceClass", fe.SettingsProviderValue(device))
        runtimeConf.setValue("Runtime", "verboseLogging", fe.SettingsProviderValue(4))
        runtimeConf.setValue("Runtime", "cpuClass", fe.SettingsProviderValue(cpuType))

        faceEngine.setSettingsProvider(config)
        faceEngine.setRuntimeSettingsProvider(runtimeConf)

        warp = fe.Image()
        err = warp.load(os.path.join(self.test_data_path, "warp1.bmp"))
        self.assertTrue(err.isOk)
        warp.save(os.path.join(self.test_data_path, "outbmp.bmp"))
        extractor = faceEngine.createExtractor()
        descriptor = faceEngine.createDescriptor()
        res, value = extractor.extractFromWarpedImage(warp, descriptor)
        self.assertTrue(res.isOk)
        data1 = descriptor.getData()
        with open(self.test_data_path + "/descriptor1_" + versionString + "_actual.bin", "wb") as out_file:
            out_file.write(data1)

        self.assertAlmostEqual(refGS, value, delta=(0.02, 0.03)[useMobileNet])
        refPath = os.path.join(self.test_data_path, "descriptor1_" + versionString + ".bin")
        with open(refPath, "rb") as file:
            read_data = file.read()
            descriptorExpected = faceEngine.createDescriptor()
            err_load = descriptorExpected.load(read_data, len(read_data))
            print(err_load)
            self.assertTrue(err_load.isOk)
            dataActual = descriptor.getData()
            dataExpected = descriptorExpected.getData()
            self.assertEqual(descriptorExpected.getModelVersion(), descriptor.getModelVersion())
            for i in range(descriptor.getDescriptorLength()):
                self.assertEqual(dataActual[i], dataExpected[i])

            # by Default full version with headears, NoSignature - write only version
            err, full_data_actual_default1 = descriptor.save()
            self.assertTrue(err.isOk)
            err, full_data_actual_default2 = descriptor.save(fe.Save.Default)
            self.assertTrue(err.isOk)
            err, full_data_actual_no_signature = descriptor.save(fe.Save.NoSignature)
            self.assertTrue(err.isOk)

            err, full_data_exp_default1 = descriptorExpected.save()
            self.assertTrue(err.isOk)
            err, full_data_exp_default2 = descriptorExpected.save(fe.Save.Default)
            self.assertTrue(err.isOk)
            err, full_data_exp_no_signature = descriptorExpected.save(fe.Save.NoSignature)
            self.assertTrue(err.isOk)

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

    def testExtractor(self):
        params = {"46_mobilenet": [46, 0.9718, True, "auto", "cpu"], "46_no_mobilnet": [46, 0.9718, False, "auto", "cpu"],
                  "52_mobilnet": [52, 1.0, True, "auto", "cpu"], "52_no_mobilnet": [52, 0.8926, False, "auto", "cpu"],
                  "54_mobilnet": [54, 0.9094, True, "auto", "cpu"], "54_no_mobilnet": [54, 0.9411, False, "auto", "cpu"],
                  "56_no_mobilnet": [56, 0.7673, False, "auto", "cpu"]}
        for key, value in params.items():
            version, refGS, useMobileNet, cpuType, device = value
            with self.subTest(key=key):
                self.extractor(version, refGS, useMobileNet, cpuType, device)

    def extractorBatch(self, version, useMobileNet, cpuType, device):
        configPath = os.path.join(self.dataPath, "faceengine.conf")
        runtimeConfigPath = os.path.join(self.dataPath, "runtime.conf")

        faceEngine = fe.createFaceEngine(self.dataPath)
        self.assertTrue(make_activation(faceEngine))
        config = fe.createSettingsProvider(configPath)
        runtimeConf = fe.createSettingsProvider(runtimeConfigPath)

        config.setValue("DescriptorFactory::Settings", "model", fe.SettingsProviderValue(version))
        config.setValue("DescriptorFactory::Settings", "useMobileNet", fe.SettingsProviderValue(useMobileNet))
        config.setValue("system", "verboseLogging", fe.SettingsProviderValue(5))
        runtimeConf.setValue("Runtime", "deviceClass", fe.SettingsProviderValue(device))
        runtimeConf.setValue("Runtime", "cpuClass", fe.SettingsProviderValue(cpuType))

        faceEngine.setSettingsProvider(config)
        faceEngine.setRuntimeSettingsProvider(runtimeConf)

        warps = [fe.Image(), fe.Image()]
        err1 = warps[0].load(os.path.join(self.test_data_path, "warp1.ppm"))
        self.assertTrue(err1.isOk)
        err2 = warps[1].load(os.path.join(self.test_data_path, "warp2.ppm"))
        self.assertTrue(err2.isOk)
        extractor = self.faceEngine.createExtractor()
        batch = self.faceEngine.createDescriptorBatch(2)
        descriptor = self.faceEngine.createDescriptor()

        res_batch, _, garbage_scores = extractor.extractFromWarpedImageBatch(warps, batch, descriptor, 2)
        self.assertTrue(res_batch.isOk)
        with open(self.test_data_path + "/batch12_" + str(version) + "_actual.bin", "wb") as out_file:
            for i in range(2):
                descriptor_from_batch = batch.getDescriptorFast(i)
                out_file.write(descriptor_from_batch.getData())
        for i_desc in range(2):
            res, value = extractor.extractFromWarpedImage(warps[i_desc], descriptor)
            self.assertTrue(res.isOk)
            self.assertEqual(descriptor.getModelVersion(), batch.getModelVersion())
            dataExpected = descriptor.getData()
            dataActual = batch.getDescriptorFast(i_desc).getData()
            descLength = descriptor.getDescriptorLength()
            for j in range(descLength):
                self.assertEqual(dataExpected[j], dataActual[j])
            self.assertAlmostEqual(value, garbage_scores[i_desc], delta=0.0001)

        # test of saving and loading from archive
        batch_loaded = self.faceEngine.createDescriptorBatch(2)

        err, full_data_default1 = batch.save()
        self.assertTrue(err.isOk)
        err_load = batch_loaded.load(full_data_default1, len(full_data_default1))
        self.assertTrue(err_load.isOk)
        self.assertBatchDescriptorsEquality(batch, batch_loaded)

        err, full_data_default2 = batch.save(fe.Save.Default)
        self.assertTrue(err.isOk)
        err_load = batch_loaded.load(full_data_default2, len(full_data_default2))
        self.assertTrue(err_load.isOk)
        self.assertBatchDescriptorsEquality(batch, batch_loaded)

        err, full_data_no_signature = batch.save(fe.Save.NoSignature)
        self.assertTrue(err.isOk)
        err_load = batch_loaded.load(full_data_no_signature, len(full_data_no_signature))
        # we cannot load batch from archive without signature
        self.assertTrue(err_load.isError)
        self.assertTrue(err_load.error, fe.SerializeError.Signature)

    def assertBatchDescriptorsEquality(self, batch1, batch2):
        self.assertEqual(batch1.getCount(), batch2.getCount())
        batch_size = batch1.getCount()
        self.assertTrue(batch_size != 0)
        for i_desc in range(batch_size):
            descriptor = batch1.getDescriptorFast(i_desc)
            descriptor_loaded = batch2.getDescriptorFast(i_desc)
            data = descriptor.getData()
            data_loaded = descriptor_loaded.getData()
            self.assertEqual(len(data), len(data_loaded))
            self.assertEqual(len(data_loaded), descriptor.getDescriptorLength())
            for j in range(len(data)):
                self.assertEqual(data[j], data_loaded[j])

    def testExtractorBatch(self):
        params = {"46_mobilenet": [46, True, "auto", "cpu"], "46_no_mobilenet": [46, False, "auto", "cpu"],
                  "52_mobilnet": [52, True, "auto", "cpu"], "52_no_mobilnet": [52, False, "auto", "cpu"],
                  "54_mobilnet": [54, True, "auto", "cpu"], "54_no_mobilnet": [54, False, "auto", "cpu"],
                  "56_no_mobilnet": [56, False, "auto", "cpu"]}
        for key, value in params.items():
            version, useMobileNet, cpuType, device = value
            with self.subTest(key=key):
                self.extractorBatch(version, useMobileNet, cpuType, device)

    def extractorAggregation(self, version, useMobileNet, cpuType, device):

        configPath = os.path.join(self.dataPath, "faceengine.conf")
        runtimeConfigPath = os.path.join(self.dataPath, "runtime.conf")

        faceEngine = fe.createFaceEngine(self.dataPath)
        self.assertTrue(make_activation(faceEngine))
        config = fe.createSettingsProvider(configPath)
        runtimeConf = fe.createSettingsProvider(runtimeConfigPath)

        config.setValue("DescriptorFactory::Settings", "model", fe.SettingsProviderValue(version))
        config.setValue("DescriptorFactory::Settings", "useMobileNet", fe.SettingsProviderValue(useMobileNet))
        config.setValue("system", "verboseLogging", fe.SettingsProviderValue(5))
        runtimeConf.setValue("Runtime", "deviceClass", fe.SettingsProviderValue(device))
        runtimeConf.setValue("Runtime", "cpuClass", fe.SettingsProviderValue(cpuType))

        faceEngine.setSettingsProvider(config)
        faceEngine.setRuntimeSettingsProvider(runtimeConf)

        warps = [fe.Image(), fe.Image()]
        err1 = warps[0].load(os.path.join(self.test_data_path, "warp1.ppm"))
        self.assertTrue(err1.isOk)
        err2 = warps[1].load(os.path.join(self.test_data_path, "warp2.ppm"))
        self.assertTrue(err2.isOk)
        extractor = faceEngine.createExtractor()
        batch = faceEngine.createDescriptorBatch(2)
        descriptor = faceEngine.createDescriptor()
        aggr = faceEngine.createDescriptor()
        res = extractor.extractFromWarpedImageBatch(warps, batch, aggr, 1)
        self.assertFalse(res[0].isError)
        res, value = extractor.extractFromWarpedImage(warps[0], descriptor)
        self.assertTrue(res.isOk)
        self.assertEqual(descriptor.getModelVersion(), batch.getModelVersion())
        data_expected = descriptor.getData()
        data_actual = batch.getDescriptorFast(0).getData()
        descLength = descriptor.getDescriptorLength()
        for j in range(descLength):
            self.assertEqual(data_expected[j], data_actual[j])

    def testExtractorAggregation(self):
        params = {"46_mobilenet": [46, True, "auto", "cpu"], "46_no_mobilenet": [46, False, "auto", "cpu"],
                  "52_mobilnet": [52, True, "auto", "cpu"], "52_no_mobilnet": [52, False, "auto", "cpu"],
                  "54_mobilnet": [54, True, "auto", "cpu"], "54_no_mobilnet": [54, False, "auto", "cpu"],
                  "56_no_mobilnet": [56, False, "auto", "cpu"]}
        for key, value in params.items():
            version, useMobileNet, cpuType, device = value
            with self.subTest(key=key):
                self.extractorAggregation(version, useMobileNet, cpuType, device)

    def testNegativeTestOnInvalidImages(self):
        # disable logging for negative tests
        self.setLogging(0)
        empty_image = fe.Image()
        descriptor = self.faceEngine.createDescriptor()
        aggregation = self.faceEngine.createDescriptor()
        extractor = self.faceEngine.createExtractor()
        res, value = extractor.extractFromWarpedImage(empty_image, descriptor)
        self.assertTrue(res.isError)
        self.assertEqual(res.error, fe.FSDKError.InvalidImage)

        descriptor_batch = self.faceEngine.createDescriptorBatch(2)
        images = [empty_image, empty_image]
        res_batch, aggr_garbage_score, garbage_scores = extractor.extractFromWarpedImageBatch(images, descriptor_batch, aggregation, 2)
        self.assertTrue(res_batch.isError)
        self.assertEqual(res_batch.error, fe.FSDKError.InvalidImage)

        detection = fe.DetectionFloat()
        landmarks = fe.Landmarks5()
        extraction_res, value = extractor.extract(empty_image, detection, landmarks, descriptor)
        self.assertTrue(extraction_res.isError)
        self.assertEqual(extraction_res.error, fe.FSDKError.InvalidImage)

    def testCompareMatchingResult(self):
        image_list = [fe.Image(), fe.Image()]
        err = image_list[0].load(os.path.join(self.test_data_path, "3_Riot_Riot_3_413_small.jpg"))
        self.assertTrue(err.isOk)
        err = image_list[1].load(os.path.join(self.test_data_path, "image_480.jpg"))
        self.assertTrue(err.isOk)
        extractor = self.faceEngine.createExtractor()
        batch = self.faceEngine.createDescriptorBatch(7)
        detector = self.faceEngine.createDetector(fe.FACE_DET_V3)
        warper = self.faceEngine.createWarper()

        warps = []
        rect_list = [image.getRect() for image in image_list]
        err, face_list = detector.detect(image_list, rect_list, 10, fe.DetectionType(fe.dtBBox | fe.dt5Landmarks))
        self.assertTrue(err.isOk)

        i_image = 0
        for list in face_list:
            for face in list:
                (detection, landmarks5) = face.detection, face.landmarks5_opt.value()
                transformation = warper.createTransformation(detection, landmarks5)
                err, warp_result = warper.warp(image_list[i_image], transformation)
                self.assertTrue(err.isOk)
                warps.append(warp_result)
            i_image += 1

        err_batch_extraction, _ = extractor.extractFromWarpedImageBatch(warps, batch, 7)
        self.assertTrue(err_batch_extraction.isOk)
        descriptor = batch.getDescriptorSlow(0)

        params = OrderedDict()
        params["1_in_top_best_descriptor_is_first"] = [1, False, [0]]
        params["N_in_top_best_descriptor_is_first"] = [4, False, [0, 5, 2, 3]]
        params["1_in_top_remove_first_descriptor_from_batch"] = [1, True, [4]]
        params["N_in_top_remove_first_descriptor_from_batch"] = [4, True, [3, 0, 1, 2]]

        for key, value in params.items():
            i, remove, expected_indices = value
            with self.subTest(key=key):
                if remove:
                    batch.removeSlow(0)
                self.match(descriptor, batch, i, expected_indices)

    def match(self, descriptor, batch, n_top, expected_indices):
        matcher = self.faceEngine.createMatcher()
        err, result_closest, indices = matcher.match(descriptor, batch, n_top)
        self.assertTrue(err.isOk)
        err1, matching_list = matcher.match(descriptor, batch)
        self.assertTrue(err1.isOk)
        similarities = []
        distances = []
        for matching in matching_list:
            similarities.append(matching.similarity)
            distances.append(matching.distance)
        self.assertEqual(max(similarities), result_closest[0].similarity)
        self.assertEqual(min(distances), result_closest[0].distance)
        self.assertEqual(len(indices), n_top)
        self.assertEqual(len(indices), len(expected_indices))
        for i, _ in enumerate(indices):
            self.assertEqual(indices[i], expected_indices[i])


if __name__ == '__main__':
    unittest.main()

