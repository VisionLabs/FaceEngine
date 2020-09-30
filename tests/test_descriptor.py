import unittest
import argparse
import sys
import os
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
        config.setValue("FaceDetV3::Settings", "minFaceSize", 20)
        self.faceEngine.setSettingsProvider(config)

    def testDifferentBatchVersion(self):
        warps = [fe.Image(), fe.Image()]
        err1 = warps[0].load(os.path.join(self.test_data_path, "warp1.ppm"))
        self.assertTrue(err1.isOk)
        err2 = warps[1].load(os.path.join(self.test_data_path, "warp2.ppm"))
        self.assertTrue(err2.isOk)

        extractor = self.faceEngine.createExtractor(46)
        batch = self.faceEngine.createDescriptorBatch(2, 46)

        extractor.extractFromWarpedImageBatch(warps, batch)

        batch_loaded = self.faceEngine.createDescriptorBatch(2, 54)

        err, full_data_default1 = batch.save()
        self.assertTrue(err.isOk)
        err_load = batch_loaded.load(full_data_default1, len(full_data_default1))
        self.assertEqual(err_load.error, fe.SerializeError.ArchiveRead)

        del extractor
        del batch
        del batch_loaded

    def testVersion(self):
        extractor = self.faceEngine.createExtractor()
        matcher = self.faceEngine.createMatcher()
        descriptor = self.faceEngine.createDescriptor()
        aggregation = self.faceEngine.createDescriptor()
        self.matchDescriptor(extractor, descriptor, aggregation, matcher)

        test_cases = (46, 52, 54, 56)
        for model in test_cases:
            with self.subTest(model=model):
                extractor = self.faceEngine.createExtractor(model)
                matcher = self.faceEngine.createMatcher(model)
                descriptor = self.faceEngine.createDescriptor(model)
                aggregation = self.faceEngine.createDescriptor(model)

                self.assertEqual(model, extractor.getModelVersion())
                self.assertEqual(model, matcher.getModelVersion())
                self.assertEqual(model, descriptor.getModelVersion())
                self.assertEqual(model, aggregation.getModelVersion())
                self.assertEqual(fe.DT_FACE, extractor.getDescriptorType())
                self.assertEqual(fe.DT_FACE, descriptor.getDescriptorType())
                self.matchDescriptor(extractor, descriptor, aggregation, matcher)

        del extractor
        del matcher
        del descriptor
        del aggregation

    def matchDescriptor(self, extractor, descriptor, aggregation, matcher):
        batch_size = 2
        # the version of descriptorBatch should be the same as extractor version
        descriptorBatch = self.faceEngine.createDescriptorBatch(2, extractor.getModelVersion())

        images = []
        for i in range(batch_size):
            image = fe.Image()
            err = image.load(os.path.join(self.test_data_path, "warp1.ppm"))
            self.assertTrue(err.isOk)
            images.append(image)
        self.assertTrue(extractor.extractFromWarpedImage(images[0], descriptor))
        self.assertTrue(extractor.extractFromWarpedImageBatch(images, descriptorBatch, aggregation))

        def assertMatchingResult(result):
            self.assertEqual(result.similarity, 1.0)
            self.assertEqual(result.distance, 0.0)

        def checkDescriptorsEquality(desc1, desc2):
            result, value = matcher.match(desc1, desc2)
            self.assertTrue(result.isOk)
            assertMatchingResult(value)

        checkDescriptorsEquality(aggregation, descriptor)
        checkDescriptorsEquality(aggregation, aggregation)
        result = matcher.match(aggregation, descriptorBatch)
        self.assertTrue(result[0].isOk)
        assertMatchingResult(result[1][0])
        assertMatchingResult(result[1][1])

        del descriptorBatch

    def testMatchDifferentVersion(self):
        test_cases = {52: 46, 54: 46, 56: 46}
        for desc1, desc2 in test_cases.items():
            with self.subTest(match_with=desc1):
                matcher = self.faceEngine.createMatcher(desc1)
                descriptor1 = self.faceEngine.createDescriptor(desc1)
                descriptor2 = self.faceEngine.createDescriptor(desc2)
                err_descriptor, value = matcher.match(descriptor1, descriptor2)
                self.assertEqual(err_descriptor.error, fe.FSDKError.IncompatibleDescriptors)

    def testMatchDifferentVersionBatch(self):
        test_cases = (52, 56)
        batch = self.faceEngine.createDescriptorBatch(2, 46)
        for model in test_cases:
            with self.subTest(match_with=model):
                descriptor = self.faceEngine.createDescriptor(model)
                err_batch = batch.add(descriptor)
                self.assertEqual(err_batch.error, fe.DescriptorBatchError.Incompatible)

        del batch
        del descriptor

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
        config.setValue("FaceDetV3::Settings", "minFaceSize", 20)
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

            descriptor_cases = {descriptor: dataActual, descriptorExpected: dataExpected}
            for i, data in descriptor_cases.items():
                with self.subTest(descriptor=i):
                    self.assertDescriptors(i, data)

        del extractor
        del descriptor

    def assertDescriptors(self, descriptor, data):
        err, full_data_default1 = descriptor.save()
        self.assertTrue(err.isOk)
        err, full_data_default2 = descriptor.save(fe.Save.Default)
        self.assertTrue(err.isOk)
        err, full_data_no_signature = descriptor.save(fe.Save.NoSignature)
        self.assertTrue(err.isOk)

        diff_actual_default1 = len(full_data_default1) - len(data)
        diff_actual_default2 = len(full_data_default2) - len(data)
        diff_actual_no_signature = len(full_data_no_signature) - len(data)

        for i in range(descriptor.getDescriptorLength()):
            self.assertEqual(data[i], full_data_default1[i + diff_actual_default1])
            self.assertEqual(data[i], full_data_default2[i + diff_actual_default2])
            self.assertEqual(data[i], full_data_no_signature[i + diff_actual_no_signature])

    def testExtractor(self):
        test_cases = {"46_mobilenet": [46, 0.9718, True, "auto", "cpu"],
                      "46_no_mobilnet": [46, 0.9718, False, "auto", "cpu"],
                      "52_mobilnet": [52, 1.0, True, "auto", "cpu"],
                      "52_no_mobilnet": [52, 0.8926, False, "auto", "cpu"],
                      "54_mobilnet": [54, 0.9094, True, "auto", "cpu"],
                      "54_no_mobilnet": [54, 0.9411, False, "auto", "cpu"],
                      "56_no_mobilnet": [56, 0.7673, False, "auto", "cpu"]}
        for key, value in test_cases.items():
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
        config.setValue("FaceDetV3::Settings", "minFaceSize", 20)
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

        res_batch, _, garbage_scores = extractor.extractFromWarpedImageBatch(warps, batch, descriptor)
        self.assertTrue(res_batch.isOk)
        with open(self.test_data_path + "/batch12_" + str(version) + "_actual.bin", "wb") as out_file:
            for i in range(2):
                getErr, descriptor_from_batch = batch.getDescriptorFast(i)
                self.assertTrue(getErr.isOk)
                out_file.write(descriptor_from_batch.getData())
        for i_desc in range(2):
            res, value = extractor.extractFromWarpedImage(warps[i_desc], descriptor)
            self.assertTrue(res.isOk)
            self.assertEqual(descriptor.getModelVersion(), batch.getModelVersion())
            dataExpected = descriptor.getData()
            getErr, tmpDescriptor = batch.getDescriptorFast(i_desc)
            self.assertTrue(getErr.isOk)
            dataActual = tmpDescriptor.getData()
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

        del extractor
        del batch
        del descriptor
        del batch_loaded

    def assertBatchDescriptorsEquality(self, batch1, batch2):
        self.assertEqual(batch1.getCount(), batch2.getCount())
        batch_size = batch1.getCount()
        self.assertTrue(batch_size != 0)
        for i_desc in range(batch_size):
            getErr1, descriptor = batch1.getDescriptorFast(i_desc)
            self.assertTrue(getErr1.isOk)
            getErr2, descriptor_loaded = batch2.getDescriptorFast(i_desc)
            self.assertTrue(getErr2.isOk)
            data = descriptor.getData()
            data_loaded = descriptor_loaded.getData()
            self.assertEqual(len(data), len(data_loaded))
            self.assertEqual(len(data_loaded), descriptor.getDescriptorLength())
            for j in range(len(data)):
                self.assertEqual(data[j], data_loaded[j])

    def testExtractorBatch(self):
        test_cases = {"46_mobilenet": [46, True, "auto", "cpu"],
                      "46_no_mobilenet": [46, False, "auto", "cpu"],
                      "52_mobilnet": [52, True, "auto", "cpu"],
                      "52_no_mobilnet": [52, False, "auto", "cpu"],
                      "54_mobilnet": [54, True, "auto", "cpu"],
                      "54_no_mobilnet": [54, False, "auto", "cpu"],
                      "56_no_mobilnet": [56, False, "auto", "cpu"]}
        for key, value in test_cases.items():
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
        config.setValue("FaceDetV3::Settings", "minFaceSize", 20)
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
        res = extractor.extractFromWarpedImageBatch(warps, batch, aggr)
        self.assertFalse(res[0].isError)
        res, value = extractor.extractFromWarpedImage(warps[0], descriptor)
        self.assertTrue(res.isOk)
        self.assertEqual(descriptor.getModelVersion(), batch.getModelVersion())
        data_expected = descriptor.getData()
        getErr, tmpDescriptorActual = batch.getDescriptorFast(0)
        self.assertTrue(getErr.isOk)
        data_actual = tmpDescriptorActual.getData()
        descLength = descriptor.getDescriptorLength()
        for j in range(descLength):
            self.assertEqual(data_expected[j], data_actual[j])

        del extractor
        del batch
        del descriptor
        del aggr

    def testExtractorAggregation(self):
        test_cases = {"46_mobilenet": [46, True, "auto", "cpu"],
                      "46_no_mobilenet": [46, False, "auto", "cpu"],
                      "52_mobilnet": [52, True, "auto", "cpu"],
                      "52_no_mobilnet": [52, False, "auto", "cpu"],
                      "54_mobilnet": [54, True, "auto", "cpu"],
                      "54_no_mobilnet": [54, False, "auto", "cpu"],
                      "56_no_mobilnet": [56, False, "auto", "cpu"]}
        for key, value in test_cases.items():
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
        res_batch, aggr_garbage_score, garbage_scores = extractor.extractFromWarpedImageBatch(images, descriptor_batch, aggregation)
        self.assertTrue(res_batch.isError)
        self.assertEqual(res_batch.error, fe.FSDKError.InvalidImage)

        del descriptor
        del descriptor_batch
        del aggregation
        del extractor

    def testCompareMatchingResult(self):
        image_list = [fe.Image(), fe.Image()]
        err = image_list[0].load(os.path.join(self.test_data_path, "3_Riot_Riot_3_413_small.jpg"))
        self.assertTrue(err.isOk)
        err = image_list[1].load(os.path.join(self.test_data_path, "image_480.jpg"))
        self.assertTrue(err.isOk)

        config = fe.createSettingsProvider("data/faceengine.conf")
        config.setValue("FaceDetV3::Settings", "minFaceSize", 20)
        self.faceEngine.setSettingsProvider(config)

        extractor = self.faceEngine.createExtractor()
        batch = self.faceEngine.createDescriptorBatch(7)
        detector = self.faceEngine.createDetector(fe.FACE_DET_V3)
        warper = self.faceEngine.createWarper()

        warps = []
        rect_list = [image.getRect() for image in image_list]
        err, face_list = detector.detect(image_list, rect_list, 10, fe.DetectionType(fe.DT_BBOX | fe.DT_LANDMARKS5))
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

        err_batch_extraction, _ = extractor.extractFromWarpedImageBatch(warps, batch)
        self.assertTrue(err_batch_extraction.isOk)
        getErr, descriptor = batch.getDescriptorSlow(0)
        self.assertTrue(getErr.isOk)
        params = OrderedDict()
        params["1_in_top_best_descriptor_is_first"] = [1, False, [0]]

        for key, value in params.items():
            i, remove, expected_indices = value
            with self.subTest(key=key):
                if remove:
                    batch.removeSlow(0)
                self.match(descriptor, batch, i, expected_indices)

        del extractor
        del batch

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

    def testDifferentBatchVersion(self):
        warps = [fe.Image(), fe.Image()]
        err1 = warps[0].load(os.path.join(self.test_data_path, "warp1.ppm"))
        self.assertTrue(err1.isOk and warps[0].isValid())
        err2 = warps[1].load(os.path.join(self.test_data_path, "warp2.ppm"))
        self.assertTrue(err2.isOk and warps[1].isValid())

        version = 46
        extractor = self.faceEngine.createExtractor(version)
        batch = self.faceEngine.createDescriptorBatch(2, version)
        descriptor = self.faceEngine.createDescriptor(version)

        res_batch, _, garbage_scores = extractor.extractFromWarpedImageBatch(warps, batch, descriptor)
        batch_version = 54
        batch_loaded = self.faceEngine.createDescriptorBatch(2, batch_version)

        err, full_data_default1 = batch.save()
        self.assertTrue(err.isOk)
        err_load = batch_loaded.load(full_data_default1, len(full_data_default1))
        # it is failed only for descriptors with different length
        self.assertTrue(err_load.isError)
        self.assertTrue(err_load.error, fe.SerializeError.ArchiveRead)

        del extractor
        del batch_loaded
        del batch

    def testOutOfRangeIndexForDescriptorFromBatch(self):
        warps = [fe.Image(), fe.Image()]
        err1 = warps[0].load(os.path.join(self.test_data_path, "warp1.ppm"))
        self.assertTrue(err1.isOk and warps[0].isValid())
        err2 = warps[1].load(os.path.join(self.test_data_path, "warp2.ppm"))
        self.assertTrue(err2.isOk and warps[1].isValid())

        version = 46
        extractor = self.faceEngine.createExtractor(version)
        batchSize = 5
        empty_batch = self.faceEngine.createDescriptorBatch(batchSize, version)

        partly_filled_batch = self.faceEngine.createDescriptorBatch(batchSize, version)
        res_batch, _ = extractor.extractFromWarpedImageBatch(warps, partly_filled_batch)
        self.assertTrue(res_batch.isOk)

        def check(batch, index, isOk, referenceError):
            get_fast_err, _ = batch.getDescriptorFast(index)
            self.assertEqual(get_fast_err.isOk, isOk)
            self.assertEqual(get_fast_err.error, referenceError)

            get_slow_err, _ = batch.getDescriptorSlow(index)
            self.assertEqual(get_fast_err.isOk, isOk)
            self.assertEqual(get_slow_err.error, referenceError)

        params = OrderedDict()
        params["out_of_range_befor_initialization"] = [empty_batch, 0, False, fe.DescriptorBatchError.OutOfRange]
        params["succeed_descriptor_case"] = [partly_filled_batch, 0, True, fe.DescriptorBatchError.Ok]
        params["out_of_range_index_is_greater_than_count"] = [partly_filled_batch, 3, False, fe.DescriptorBatchError.OutOfRange]
        params["out_of_range_index_is_greater_than_max_size"] = [partly_filled_batch, 6, False, fe.DescriptorBatchError.OutOfRange]

        for key, value in params.items():
            batch, index, isOk, typeOfError = value
            with self.subTest(key=key):
                check(batch, index, isOk, typeOfError)

        del extractor
        del empty_batch

    def testClearBatch(self):
        warps = [fe.Image(), fe.Image()]
        err1 = warps[0].load(os.path.join(self.test_data_path, "warp1.ppm"))
        self.assertTrue(err1.isOk and warps[0].isValid())
        err2 = warps[1].load(os.path.join(self.test_data_path, "warp1.ppm"))
        self.assertTrue(err2.isOk and warps[1].isValid())

        version = 52
        extractor = self.faceEngine.createExtractor(version)
        batchCount = 2
        batch = self.faceEngine.createDescriptorBatch(batchCount, version)
        # batch size is 2, batch is empty
        self.assertEqual(batch.getCount(), 0)
        # batch is filled, batch count is 2
        res_batch, _ = extractor.extractFromWarpedImageBatch(warps, batch)
        self.assertEqual(batch.getCount(), batchCount)
        # batch is cleared, batch count must be 0
        batch.clear()
        self.assertEqual(batch.getCount(), 0)

        del extractor
        del batch

    @unittest.expectedFailure
    def testDescriptorWithoutConfig(self):
        emptyFaceEngine = fe.createFaceEngine("")
        descriptor = emptyFaceEngine.createDescriptor()

        del emptyFaceEngine
        del descriptor

if __name__ == '__main__':
    unittest.main()

