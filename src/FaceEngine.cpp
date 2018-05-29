#include <iostream>
#include <pybind11/pybind11.h>
#include <fsdk/FaceEngine.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>
#include "ErrorsAdapter.hpp"
#include "FaceEngineAdapter.hpp"
#include "SettingsProviderAdapter.hpp"

namespace py = pybind11;


PyIFaceEngine createPyFaceEnginePtr(const char* dataPath = nullptr, const char* configPath = nullptr) {
	return PyIFaceEngine(dataPath, configPath);
}

PyISettingsProvider createSettingsProviderPtr(const char* path) {
	return PyISettingsProvider(path);
}


PYBIND11_MAKE_OPAQUE(fsdk::Landmarks5);
PYBIND11_MAKE_OPAQUE(fsdk::Landmarks68);

PYBIND11_MODULE(FaceEngine, f) {

	f.doc() = R"pbdoc(
        Python wrapper for LUNA SDK usings pybind11
        -------------------------------------------

        .. currentmodule:: FaceEngine

        .. autosummary::
           :toctree: _generate

           	createFaceEngine
           	createSettingsProvider
           	loadImage
           	PyIFaceEngine
			PyIFaceEngine.createAttributeEstimator
			PyIFaceEngine.createQualityEstimator
			PyIFaceEngine.createEthnicityEstimator

			PyIFaceEngine.createHeadPoseEstimator
			PyIFaceEngine.createBlackWhiteEstimator
			PyIFaceEngine.createDepthEstimator
			PyIFaceEngine.createIREstimator
			PyIFaceEngine.createSmileEstimator
			PyIFaceEngine."createFaceFlowEstimator
			PyIFaceEngine.createEyeEstimator
			PyIFaceEngine.createEmotionsEstimator
			PyIFaceEngine.createGazeEstimator

			PyIFaceEngine.createDetector
			PyIFaceEngine.createWarper
			PyIFaceEngine.createDescriptor
			PyIFaceEngine.createDescriptorBatch
			PyIFaceEngine.createExtractor
			PyIFaceEngine.createMatcher
			PyIFaceEngine.createLSHTable
			PyIFaceEngine.setSettingsProvider

    )pbdoc";

	f.def("createFaceEngine", &createPyFaceEnginePtr, py::return_value_policy::take_ownership,
		  "Create FaceEngine", py::arg("dataPath") = nullptr, py::arg("configPath") = nullptr,
	"Create the LUNA SDK root object\n"
			"    Args:\n"
			"        param1 (str): [optional] path to folder with FSDK data. Default: ./data (on windows), /opt/visionlabs/data (on linux)\n"
			"        param2 (str): [optional] path to faceengine.conf file. Default: <dataPath>/faceengine.cong\n");

	f.def("createSettingsProvider", &createSettingsProviderPtr, py::return_value_policy::take_ownership,
		  "Create object SettingsProvider\n"
			"    Args:\n"
			"        param1 (str): configuration file path\n");

	py::class_<PyIFaceEngine>(f, "PyIFaceEngine", "Root LUNA SDK object interface")
		.def("createAttributeEstimator", &PyIFaceEngine::createAttributeEstimator, "Creates Attribute estimator")
		.def("createQualityEstimator", &PyIFaceEngine::createQualityEstimator, "Creates Quality estimator")
		.def("createEthnicityEstimator", &PyIFaceEngine::createEthnicityEstimator, "Creates Ethnicity estimator")

		.def("createHeadPoseEstimator", &PyIFaceEngine::createHeadPoseEstimator, "Creates Head pose estimator")
		.def("createBlackWhiteEstimator", &PyIFaceEngine::createBlackWhiteEstimator, "Creates Black/White estimator")
		.def("createDepthEstimator", &PyIFaceEngine::createDepthEstimator, "Creates Liveness Depth estimator")
		.def("createIREstimator", &PyIFaceEngine::createIREstimator, "Creates Liveness Infrared estimator")
		.def("createSmileEstimator", &PyIFaceEngine::createSmileEstimator, "Creates Smile estimator")
		.def("createFaceFlowEstimator", &PyIFaceEngine::createFaceFlowEstimator, "Creates Liveness flow estimator. "
			"Note: this estimator is required only for liveness detection purposes.")
		.def("createEyeEstimator", &PyIFaceEngine::createEyeEstimator, "Creates Eye estimator")
		.def("createEmotionsEstimator", &PyIFaceEngine::createEmotionsEstimator, "Creates Emotions estimator")
		.def("createGazeEstimator", &PyIFaceEngine::createGazeEstimator, "Creates Gaze estimator")

		.def("createDetector", &PyIFaceEngine::createDetector,
			"Creates a detector of given type.\n"
			"    Args:\n"
			"        param1 (enum ObjectDetectorClassType): fixed or random order of algorithm types\n")
		.def("createWarper", &PyIFaceEngine::createWarper, "Creates warper")
		.def("createDescriptor", &PyIFaceEngine::createDescriptor, "Creates Descriptor")
		.def("createDescriptorBatch", &PyIFaceEngine::createDescriptorBatch, py::arg("size"), py::arg("version") = 0,
			"Creates Batch of descriptors\n"
			"    Args:\n"
			"        param1 (int): amount of descriptors in batch\n"
			"        param2 (str): descriptor version in batch. If 0 - use dafault version from config\n")

		.def("createExtractor", &PyIFaceEngine::createExtractor, "Creates descriptor extractor")
		.def("createMatcher", &PyIFaceEngine::createMatcher, "Creates descriptor matcher")
		.def("createLSHTable", &PyIFaceEngine::createLSHTable,
			"Creates Local Sensitive Hash table (Descriptor index).\n"
			"    Args:\n"
			"        param1 (int): batch of descriptors to build index with\n"
			"        note: Index is unmutable, you cant add or remove descriptors from already created index\n")
		.def("setSettingsProvider", &PyIFaceEngine::setSettingsProvider,
			"Sets settings provider\n"
			"    Args:\n"
			"        param1 (PyISettingsProvider): setting provider\n")
			;

// ISettingsProvider
	py::class_<PyISettingsProvider>(f, "PyISettingsProvider")
		.def("getDefaultPath", &PyISettingsProvider::getDefaultPath)
		.def("load", &PyISettingsProvider::load)
		.def("save", &PyISettingsProvider::save)
		.def("clear", &PyISettingsProvider::clear)
		.def("isEmpty", &PyISettingsProvider::isEmpty)
		.def("setValue", &PyISettingsProvider::setValue)
		.def("getValue", &PyISettingsProvider::getValue)
			;
	py::class_<fsdk::ISettingsProvider::Value>(f, "SettingsProviderValue")
		.def(py::init<>(), "Constructor")
		.def(py::init<int>(), "Constructor")
		.def(py::init<int, int>(), "Constructor")
		.def(py::init<int, int, int>(), "Constructor")
		.def(py::init<int, int, int, int>(), "Constructor")
		.def(py::init<float>(), "Constructor")
		.def(py::init<float, float>(), "Constructor")
		.def(py::init<float, float, float>(), "Constructor")
		.def(py::init<float, float, float, float>(), "Constructor")
		.def(py::init<const char*>(), "Constructor")
		.def(py::init<const fsdk::Rect&>(), "Constructor")
		.def(py::init<bool>(), "Constructor")
		.def("asFloat", &fsdk::ISettingsProvider::Value::asFloat, py::arg("defaultValue") = 0.f)
		.def("asBool", &fsdk::ISettingsProvider::Value::asBool, py::arg("defaultValue") = false)
		.def("asInt", &fsdk::ISettingsProvider::Value::asInt, py::arg("defaultValue") = 0)
		.def("asString", &fsdk::ISettingsProvider::Value::asString, py::arg("defaultValue") = "")
		.def("asRect", [](
			const fsdk::ISettingsProvider::Value& v) {
					return v.asRect(); })
		.def("asRect", [](
			const fsdk::ISettingsProvider::Value& v, const fsdk::Rect& r) {
				return v.asRect(r); })
		.def("asPoint2f", [](
			const fsdk::ISettingsProvider::Value& v) {
				return v.asPoint2f(); })
		.def("asPoint2f", [](
			const fsdk::ISettingsProvider::Value& v, const fsdk::Point2f& r) {
				return v.asPoint2f(r); })
		.def("asPoint2i", [](
			const fsdk::ISettingsProvider::Value& v) {
				return v.asPoint2i(); })
		.def("asPoint2i", [](
			const fsdk::ISettingsProvider::Value& v, const fsdk::Point2i& r) {
				return v.asPoint2i(r); })
		.def("asSize", [](
			const fsdk::ISettingsProvider::Value& v) {
				return v.asSize(); })
		.def("asSize", [](
			const fsdk::ISettingsProvider::Value& v, const fsdk::Size& s) {
				return v.asSize(s); })
			;

	py::class_<fsdk::IFaceEnginePtr>(f, "IFaceEnginePtr");

	py::class_<fsdk::IQualityEstimatorPtr>(f, "IQualityEstimatorPtr")
		.def("estimate",[](
			const fsdk::IQualityEstimatorPtr& est,
			const fsdk::Image &warp) {
				fsdk::Quality out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, out);
				if (err.isOk())
					return py::cast(out);
				else
					return py::cast(FSDKErrorResult(err)); })
				;

	py::class_<fsdk::IAttributeEstimatorPtr>(f, "IAttributeEstimatorPtr")
		.def("estimate", [](
			const fsdk::IAttributeEstimatorPtr& est,
			const fsdk::Image &warp) {
				fsdk::AttributeEstimation out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, out);
				if (err.isOk())
					return py::cast(out);
				else
					return py::cast(FSDKErrorResult(err)); })
					;
	py::class_<fsdk::IEthnicityEstimatorPtr>(f, "IEthnicityEstimatorPtr")
		.def("estimate",[](
			const fsdk::IEthnicityEstimatorPtr& est,
			const fsdk::Image &warp) {
				fsdk::EthnicityEstimation out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, out);
				if (err.isOk())
					return py::cast(out);
				else
					return py::cast(FSDKErrorResult(err)); })
				;

	py::class_<fsdk::IDetectorPtr>(f, "IDetectorPtr")
		.def("detect",[](
			const fsdk::IDetectorPtr& det,
			const fsdk::Image& image,
			const fsdk::Rect& rect,
			int maxCount) {
				fsdk::Detection detections[maxCount];
				fsdk::Landmarks5 landmarks[maxCount];
				fsdk::Landmarks68 landmarks68[maxCount];
				fsdk::ResultValue<fsdk::FSDKError, int> err = det->detect(
					image,
					rect,
					detections,
					landmarks,
					landmarks68,
					maxCount);
				auto detectionResultPyList = py::list();
				if (err.isOk()) {
					for (size_t i = 0; i < maxCount; ++i) {
						detectionResultPyList.append(std::make_tuple(detections[i], landmarks[i], landmarks68[i]));
					}
					return detectionResultPyList;
				}
				else {
					detectionResultPyList.append(py::cast(FSDKErrorValueInt(err)));
					return detectionResultPyList; } })
					;

	py::class_<DetectionResult>(f, "DetectionResult")
		.def_readwrite("detection", &DetectionResult::detection)
		.def_readwrite("landmarks5", &DetectionResult::landmarks5)
		.def_readwrite("landmarks68", &DetectionResult::landmarks68)
			;

	py::class_<fsdk::IWarperPtr>(f, "IWarperPtr")
		.def("warp",[](
			const fsdk::IWarperPtr& warper,
			const fsdk::Image& image,
			const fsdk::Transformation& transformation) {
				fsdk::Image transformedImage;
				fsdk::Result<fsdk::FSDKError> error = warper->warp(image, transformation, transformedImage);
				if (error.isOk())
					return py::cast(transformedImage);
				else
					return py::cast(FSDKErrorResult(error)); })
		.def("warp",[](
			const fsdk::IWarperPtr& warper,
			const fsdk::Landmarks5& landmarks,
			const fsdk::Transformation& transformation) {
				fsdk::Landmarks5 transformedLandmarks5;
				fsdk::Result<fsdk::FSDKError> error = warper->warp(
					landmarks,
					transformation,
					transformedLandmarks5);
				if (error.isOk())
					return py::cast(transformedLandmarks5);
				else
					return py::cast(FSDKErrorResult(error)); })
		.def("warp",[](
			const fsdk::IWarperPtr& warper,
			const fsdk::Landmarks68& landmarks68,
			const fsdk::Transformation& transformation) {
				fsdk::Landmarks68 transformedLandmarks68;
				fsdk::Result<fsdk::FSDKError> error = warper->warp(landmarks68,
																	transformation,
																   transformedLandmarks68);
				if (error.isOk())
					return py::cast(transformedLandmarks68);
				else
					return py::cast(FSDKErrorResult(error)); })
		.def("createTransformation",[](
			const fsdk::IWarperPtr& warper,
			const fsdk::Detection& detection,
			const fsdk::Landmarks5& landmarks) {
				return warper->createTransformation(detection, landmarks); })
					;
	// descriptor
	py::class_<fsdk::IDescriptorPtr>(f, "IDescriptorPtr")
		.def("getModelVersion",[]( const fsdk::IDescriptorPtr& desc) {
				return desc->getModelVersion(); })
		.def("getDescriptorLength",[]( const fsdk::IDescriptorPtr& desc) {
			return desc->getDescriptorLength(); })
		.def("getDescriptor",[]( const fsdk::IDescriptorPtr& desc) {
			std::vector<uint8_t>buffer(264, 0);
			bool allocated = desc->getDescriptor(&buffer.front());
			auto l = py::list();
			for (auto it = buffer.rbegin(); it != buffer.rend(); ++it) {
				l.append(*it);
			}
			if (allocated)
				return l;
			else
				return py::list(); })
				;
	py::class_<fsdk::IDescriptorBatchPtr>(f, "IDescriptorBatchPtr")
		.def("add",[] (
			const fsdk::IDescriptorBatchPtr& descriptorBatchPtr,
			const fsdk::IDescriptorPtr& desc) {
				fsdk::Result<fsdk::IDescriptorBatch::Error> error = descriptorBatchPtr->add(desc);
			if (error.isOk())
				return py::cast(desc);
			else
				return py::cast(DescriptorBatchResult(error)); })
		.def("removeFast",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr, int index) {
				return descriptorBatchPtr->removeFast(index); })
		.def("removeSlow",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr, int index) {
				return descriptorBatchPtr->removeSlow(index); })
		.def("getMaxCount",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr) {
			return descriptorBatchPtr->getMaxCount(); })
		.def("getCount",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr) {
			return descriptorBatchPtr->getCount(); })
		.def("getModelVersion",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr) {
			return descriptorBatchPtr->getModelVersion(); })
		.def("getDescriptorSize",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr) {
			return descriptorBatchPtr->getDescriptorSize(); })
		.def("getDescriptorSlow",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr, int index) {
			if (index < 0 || index >= descriptorBatchPtr->getCount()) throw py::index_error();
			return fsdk::acquire(descriptorBatchPtr->getDescriptorSlow(index)); })
		.def("getDescriptorFast",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr, int index) {
			return fsdk::acquire(descriptorBatchPtr->getDescriptorFast(index)); })
				;

	py::enum_<fsdk::IDescriptorBatch::Error>(f, "DescriptorBatchError")
		.value("Ok", fsdk::IDescriptorBatch::Error::Ok)
		.value("InvalidInput", fsdk::IDescriptorBatch::Error::InvalidInput)
		.value("BatchFull", fsdk::IDescriptorBatch::Error::BatchFull)
		.value("Incompatible", fsdk::IDescriptorBatch::Error::Incompatible)
		.value("Internal", fsdk::IDescriptorBatch::Error::Internal)
		.value("IoError", fsdk::IDescriptorBatch::Error::IoError)
		.value("OutOfRange", fsdk::IDescriptorBatch::Error::OutOfRange)
			;

	py::class_<fsdk::IDescriptorExtractorPtr>(f, "IDescriptorExtractorPtr")
		.def("extract",[](
			const fsdk::IDescriptorExtractorPtr& extractor,
			fsdk::Image& image,
			const fsdk::Detection& detection,
			const fsdk::Landmarks5& landmarks,
			const fsdk::IDescriptorPtr& descriptor) {
			fsdk::ResultValue<fsdk::FSDKError, float> err = extractor->extract(image, detection,
																			   landmarks, descriptor);
				if (err.isOk())
					return py::cast(image);
				else
					return py::cast(FSDKErrorValueFloat(err)); })
		.def("extractFromWarpedImage",[](
			const fsdk::IDescriptorExtractorPtr& extractor,
			const fsdk::Image& image,
			const fsdk::IDescriptorPtr& descriptor) {
			fsdk::ResultValue<fsdk::FSDKError, float> err = extractor->extractFromWarpedImage(image, descriptor);
			if (err.isOk())
				return py::cast(descriptor);
			else
				return py::cast(FSDKErrorValueFloat(err)); })
		.def("extractFromWarpedImageBatch",[](
			const fsdk::IDescriptorExtractorPtr& extractor,
			py::list warpsBatchList,
			const fsdk::IDescriptorBatchPtr& descriptorBatch,
			const fsdk::IDescriptorPtr& aggregation,
			int batchSize) {
				float garbageScoreBatch[batchSize];
				fsdk::Image warpsBatch [batchSize];
				for (size_t i = 0; i < batchSize; ++i) {
					warpsBatch[i] = warpsBatchList[i].cast<fsdk::Image>();
				}
				fsdk::Result<fsdk::FSDKError> err = extractor->extractFromWarpedImageBatch(
						warpsBatch,
						descriptorBatch,
						aggregation,
						garbageScoreBatch,
						batchSize);
				auto garbagePyList = py::list();
				if (err.isOk()) {
					for (size_t i = 0; i < batchSize; ++i) {
						garbagePyList.append(garbageScoreBatch[i]);
					}
					return garbagePyList;
				}
				else {
					garbagePyList.append(FSDKErrorResult(err));
				} })
		.def("extractFromWarpedImageBatch",[](
			const fsdk::IDescriptorExtractorPtr& extractor,
			py::list warpsBatchList,
			const fsdk::IDescriptorBatchPtr& descriptorBatch,
			int batchSize) {
				float garbageScoreBatch[batchSize];
				fsdk::Image warpsBatch [batchSize];
				for (size_t i = 0; i < batchSize; ++i) {
					warpsBatch[i] = warpsBatchList[i].cast<fsdk::Image>();
				}
				fsdk::Result<fsdk::FSDKError> err = extractor->extractFromWarpedImageBatch(
					warpsBatch,
					descriptorBatch,
					garbageScoreBatch,
					batchSize);
				auto garbagePyList = py::list();
				if (err.isOk()) {
					for (size_t i = 0; i < batchSize; ++i) {
						garbagePyList.append(garbageScoreBatch[i]);
					}
					return garbagePyList;
				}
				else {
					garbagePyList.append(FSDKErrorResult(err));
					return garbagePyList; } })
					;

	py::class_<fsdk::IDescriptorMatcherPtr>(f, "IDescriptorMatcherPtr")
		.def("match",[](
			const fsdk::IDescriptorMatcherPtr& matcherPtr,
			const fsdk::IDescriptorPtr& first,
			const fsdk::IDescriptorPtr& second) {
			fsdk::ResultValue<fsdk::FSDKError, fsdk::MatchingResult> err =
				matcherPtr->match(first, second);
				return FSDKErrorValueMatching(err); })
		.def("match",[](
			const fsdk::IDescriptorMatcherPtr& matcherPtr,
			const fsdk::IDescriptorPtr& reference,
			const fsdk::IDescriptorBatchPtr& candidates) {
			fsdk::MatchingResult results[candidates->getCount()];
			fsdk::Result<fsdk::FSDKError> err =
				matcherPtr->match(reference, candidates, results);
				auto resultsPyList = py::list();
				if (err.isOk()) {
					for (const auto& it: results) {
						resultsPyList.append(it);
					}
					return resultsPyList;
				}
				else {
					resultsPyList.append(FSDKErrorResult(err));
					return resultsPyList; } })
		.def("match",[](
			const fsdk::IDescriptorMatcherPtr& matcherPtr,
			const fsdk::IDescriptorPtr reference,
			const fsdk::IDescriptorBatchPtr& candidates,
			py::list indicesPyList) {
				const int indicesCount = py::len(indicesPyList);
				int indices[indicesCount];
				for (size_t i = 0; i < indicesCount; ++i) {
					indices[i] = indicesPyList[i].cast<int>();
				}
				fsdk::MatchingResult results[candidates->getCount()];
				fsdk::Result<fsdk::FSDKError> err =
					matcherPtr->match(reference, candidates, indices, indicesCount, results);
				auto resultsPyList = py::list();
				if (err.isOk()) {
					for (const auto& it: results) {
						resultsPyList.append(it);
					}
					return resultsPyList;
				}
				else {
					resultsPyList.append(FSDKErrorResult(err));
						return resultsPyList; } })
		.def("matchCompact",[](
			const fsdk::IDescriptorMatcherPtr& matcherPtr,
			const fsdk::IDescriptorPtr reference,
			const fsdk::IDescriptorBatchPtr& candidates,
			py::list indicesPyList) {
				const int indicesCount = py::len(indicesPyList);
				int indices[indicesCount];
				for (size_t i = 0; i < indicesCount; ++i) {
					indices[i] = indicesPyList[i].cast<int>();
				}
				fsdk::MatchingResult results[indicesCount];
				fsdk::Result<fsdk::FSDKError> err =
					matcherPtr->matchCompact(reference, candidates, indices, indicesCount, results);
				auto resultsPyList = py::list();
				if (err.isOk()) {
					for (const auto& it: results) {
						resultsPyList.append(it);
					}
					return resultsPyList;
				}
				else {
					resultsPyList.append(FSDKErrorResult(err));
					return resultsPyList; } })
				;
//	second part of estimators
	py::class_<fsdk::IHeadPoseEstimatorPtr>(f, "IHeadPoseEstimatorPtr")
		.def("estimate",[](
			const fsdk::IHeadPoseEstimatorPtr& est,
			const fsdk::Landmarks68& landmarks68) {
				fsdk::HeadPoseEstimation out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(landmarks68, out);
				if (err.isOk())
					return py::cast(out);
				else
					return py::cast(FSDKErrorResult(err)); })
		.def("estimate",[](
			const fsdk::IHeadPoseEstimatorPtr& est,
			const fsdk::Image& image,
			const fsdk::Detection& detection) {
				fsdk::HeadPoseEstimation out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(image, detection, out);
				if (err.isOk())
					return py::cast(out);
				else
					return py::cast(FSDKErrorResult(err)); })
					;

	py::class_<fsdk::Ref<fsdk::IBlackWhiteEstimator>>(f, "IBlackWhiteEstimatorPtr")
		.def("estimate",[](
			const fsdk::Ref<fsdk::IBlackWhiteEstimator>& est,
			const fsdk::Image& image) {
				bool outIsGrayscale;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(image, outIsGrayscale);
				if (err.isOk())
					return py::cast(outIsGrayscale);
				else
					return py::cast(FSDKErrorResult(err)); })
					;

	py::class_<fsdk::ILivenessDepthEstimatorPtr>(f, "ILivenessDepthEstimatorPtr")
		.def("estimate",[](
			const fsdk::ILivenessDepthEstimatorPtr& est,
			const fsdk::Image& image) {
			fsdk::ResultValue<fsdk::FSDKError, float> err = est->estimate(image);

				return FSDKErrorValueFloat(err); })
		.def("setRange",[](
			const fsdk::ILivenessDepthEstimatorPtr& est,
			const fsdk::DepthRange& range) {
				return est->setRange(range); })
					;

	py::class_<fsdk::ILivenessIREstimatorPtr>(f, "ILivenessIREstimatorPtr")
		.def("estimate",[](
			const fsdk::ILivenessIREstimatorPtr& est,
			const fsdk::Image& irWarp) {
			fsdk::ResultValue<fsdk::FSDKError,float> err = est->estimate(irWarp);
				return FSDKErrorValueFloat(err); })
					;

	py::class_<fsdk::ISmileEstimatorPtr>(f, "ISmileEstimatorPtr")
		.def("estimate",[](
			const fsdk::ISmileEstimatorPtr& est,
			const fsdk::Image& image) {
				fsdk::SmileEstimation out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(image, out);
				if (err.isOk())
					return py::cast(out);
				else
					return py::cast(FSDKErrorResult(err)); })
					;

	py::class_<fsdk::ILivenessFlowEstimatorPtr>(f, "ILivenessFlowEstimatorPtr")
		.def("estimate",[](
			const fsdk::ILivenessFlowEstimatorPtr& est,
			const fsdk::Image& small,
			py::list framesPyList,
			int length) {
				fsdk::Image frames [length];
				for (size_t i = 0; i < length; ++i) {
					frames[i] = framesPyList[i].cast<fsdk::Image>();
				}
				double score = 0.0;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(small, frames, length, score);
				if (err.isOk())
					return py::cast(score);
				else
					return py::cast(FSDKErrorResult(err)); })
					;

	py::class_<fsdk::IEyeEstimatorPtr>(f, "IEyeEstimatorPtr")
		.def("estimate",[](
			const fsdk::IEyeEstimatorPtr& est,
			const fsdk::Image& warp,
			const fsdk::Landmarks5& landmarks5) {
				fsdk::EyesEstimation out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, landmarks5, out);
				if (err.isOk())
					return py::cast(out);
				else
					return py::cast(FSDKErrorResult(err)); })
		.def("estimate",[](
			const fsdk::IEyeEstimatorPtr& est,
			const fsdk::Image& warp,
			const fsdk::Landmarks68& landmarks68) {
				fsdk::EyesEstimation out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, landmarks68, out);
				if (err.isOk())
					return py::cast(out);
				else
					return py::cast(FSDKErrorResult(err)); })
					;

	py::class_<fsdk::IEmotionsEstimatorPtr>(f, "IEmotionsEstimatorPtr")
		.def("estimate",[](
			const fsdk::IEmotionsEstimatorPtr& est,
			const fsdk::Image& warp) {
				fsdk::EmotionsEstimation out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, out);
				if (err.isOk())
					return py::cast(out);
				else
					return py::cast(FSDKErrorResult(err)); })
					;

	py::class_<fsdk::IGazeEstimatorPtr>(f, "IGazeEstimatorPtr")
		.def("estimate",[](
			const fsdk::IGazeEstimatorPtr& est,
			const fsdk::HeadPoseEstimation& angles,
			const fsdk::EyesEstimation& eyesEstimation) {
				fsdk::GazeEstimation out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(angles, eyesEstimation, out);
				if (err.isOk())
					return py::cast(out);
				else
					return py::cast(FSDKErrorResult(err)); })
					;


	py::class_<fsdk::ILSHTablePtr>(f, "ILSHTablePtr");

	py::class_<fsdk::MatchingResult>(f, "MatchingResult")
		.def(py::init<>())
		.def(py::init<float, float>())
		.def_readwrite("distance", &fsdk::MatchingResult::distance)
		.def_readwrite("similarity", &fsdk::MatchingResult::similarity)
		.def("__repr__",
			 [](const fsdk::MatchingResult &result) {
				 return "distance = " + std::to_string(result.distance)
						+ ", similarity = " + std::to_string(result.similarity); })
			;

	py::class_<fsdk::Landmarks5>(f, "Landmarks5")
		.def(py::init<>())
		.def("__len__", [](const fsdk::Landmarks5 &v) { return v.landmarkCount; })
		.def("__getitem__", [](const fsdk::Landmarks5 &s, size_t i) {
			if (i >= s.landmarkCount) throw py::index_error();
			return s.landmarks[i];
		})
		.def("__setitem__", [](fsdk::Landmarks5 &s, size_t i, fsdk::Vector2<float> v) {
			if (i >= s.landmarkCount) throw py::index_error();
			s.landmarks[i].x = v.x;
			s.landmarks[i].y = v.y;
			return s.landmarks[i];
		})
			;

	py::class_<fsdk::Landmarks68>(f, "Landmarks68")
		.def(py::init<>())
		.def("__len__", [](const fsdk::Landmarks68 &v) { return v.landmarkCount; })
		.def("__getitem__", [](const fsdk::Landmarks68 &s, size_t i) {
			if (i >= s.landmarkCount) throw py::index_error();
			return s.landmarks[i];
		})
		.def("__setitem__", [](fsdk::Landmarks68 &s, size_t i, fsdk::Vector2<float> v) {
			if (i >= s.landmarkCount) throw py::index_error();
			s.landmarks[i].x = v.x;
			s.landmarks[i].y = v.y;
			return s.landmarks[i];
		})
			;

	py::class_<fsdk::EyesEstimation::EyeAttributes::IrisLandmarks>(f, "IrisLandmarks")
		.def(py::init<>())
		.def("__len__", [](const fsdk::EyesEstimation::EyeAttributes::IrisLandmarks &v)
			{ return fsdk::EyesEstimation::EyeAttributes::irisLandmarksCount; })
		.def("__getitem__", [](const fsdk::EyesEstimation::EyeAttributes::IrisLandmarks &l, size_t i) {
			if (i >= fsdk::EyesEstimation::EyeAttributes::irisLandmarksCount) throw py::index_error();
			return l.landmarks[i];
		})
		.def("__setitem__", [](fsdk::EyesEstimation::EyeAttributes::IrisLandmarks &s,
							   size_t i,
							   fsdk::Vector2<float> v) {
			if (i >= fsdk::EyesEstimation::EyeAttributes::irisLandmarksCount) throw py::index_error();
			s.landmarks[i].x = v.x;
			s.landmarks[i].y = v.y;
			return s.landmarks[i];
		})
			;

	py::class_<fsdk::EyesEstimation::EyeAttributes::EyelidLandmarks>(f, "EyelidLandmarks")
		.def(py::init<>())
		.def("__len__", [](const fsdk::EyesEstimation::EyeAttributes::EyelidLandmarks &v)
			{ return fsdk::EyesEstimation::EyeAttributes::eyelidLandmarksCount; })
		.def("__getitem__", [](const fsdk::EyesEstimation::EyeAttributes::EyelidLandmarks &l, size_t i) {
			if (i >= fsdk::EyesEstimation::EyeAttributes::eyelidLandmarksCount) throw py::index_error();
			return l.landmarks[i];
			})
		.def("__setitem__", [](fsdk::EyesEstimation::EyeAttributes::EyelidLandmarks &s,
							   size_t i,
							   fsdk::Vector2<float> v) {
			if (i >= fsdk::EyesEstimation::EyeAttributes::eyelidLandmarksCount) throw py::index_error();
			s.landmarks[i].x = v.x;
			s.landmarks[i].y = v.y;
			return s.landmarks[i];
		})
			;

// Vector2
	py::class_<fsdk::Vector2<float>>(f, "Vector2f")
		.def(py::init<>())
		.def(py::init<float, float>())
		.def(py::init<const fsdk::Vector2<float>&>())
		.def(py::self != py::self)
		.def(py::self == py::self)
		.def_readwrite("x", &fsdk::Vector2<float>::x)
		.def_readwrite("y", &fsdk::Vector2<float>::y)
		.def("__repr__",
			 [](const fsdk::Vector2<float> &v) {
				 return "x = " + std::to_string(v.x) + ", y = " + std::to_string(v.y);
		 	})
				;

	py::class_<fsdk::Vector2<int>>(f, "Vector2i")
		.def(py::init<>())
		.def(py::init<int, int>())
		.def(py::init<const fsdk::Vector2<int>&>())
		.def(py::self != py::self)
		.def(py::self == py::self)
		.def_readwrite("x", &fsdk::Vector2<int>::x)
		.def_readwrite("y", &fsdk::Vector2<int>::y)
		.def("__repr__",
			 [](const fsdk::Vector2<int> &v) {
				 return "x = " + std::to_string(v.x) + ", y = " + std::to_string(v.y);
			 })
				;

//	Errors
	py::class_<FSDKErrorResult>(f, "FSDKErrorResult")
		.def_readonly("isOk", &FSDKErrorResult::isOk)
		.def_readonly("isError", &FSDKErrorResult::isError)
		.def_readonly("FSDKError", &FSDKErrorResult::fsdkError)
		.def_readonly("what", &FSDKErrorResult::what)
		.def("__repr__",
			 [](const FSDKErrorResult &err) {
				 return "FSDKErrorResult: "
					"isOk = " + std::to_string(err.isOk)
					+ ", isError = " + std::to_string(err.isError)
					+ ", FSDKError = " + fsdk::ErrorTraits<fsdk::FSDKError >::toString(err.fsdkError)
					+ ", what = " + err.what; })
			;

	py::class_<DescriptorBatchResult>(f, "DescriptorBatchResult")
		.def_readonly("isOk", &DescriptorBatchResult::isOk)
		.def_readonly("isError", &DescriptorBatchResult::isError)
		.def_readonly("DescriptorBatchError", &DescriptorBatchResult::descriptorBatchError)
		.def_readonly("what", &DescriptorBatchResult::what)
		.def("__repr__",
			 [](const DescriptorBatchResult &err) {
				 return "DescriptorBatchResult: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", DescriptorBatchError = " +
				 fsdk::ErrorTraits<fsdk::IDescriptorBatch::Error>::toString(err.descriptorBatchError)
						+ ", what = " + err.what; })
			;

	py::class_<ImageErrorResult>(f, "ImageErrorResult")
		.def_readonly("isOk", &ImageErrorResult::isOk)
		.def_readonly("isError", &ImageErrorResult::isError)
		.def_readonly("imageError", &ImageErrorResult::imageError)
		.def_readonly("what", &ImageErrorResult::what)
		.def("__repr__",
			 [](const ImageErrorResult &err) {
				 return "ImageErrorResult: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", imageError = " + fsdk::ErrorTraits<fsdk::Image::Error>::toString(err.imageError)
						+ ", what = " + err.what; })
			;

	py::class_<SettingsProviderResult>(f, "SettingsProviderResult")
		.def_readonly("isOk", &SettingsProviderResult::isOk)
		.def_readonly("isError", &SettingsProviderResult::isError)
		.def_readonly("SettingsProviderResult", &SettingsProviderResult::settingsProviderError)
		.def_readonly("what", &SettingsProviderResult::what)
		.def("__repr__",
			 [](const SettingsProviderResult &err) {
				 return "SettingsProviderResult: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", SettingsProviderError = " +
				 			fsdk::ErrorTraits<fsdk::ISettingsProvider::Error>::
							 toString(err.settingsProviderError)
						+ ", what = " + err.what; })
				;

	py::class_<FSDKErrorValueInt>(f, "FSDKErrorValueInt")
		.def_readonly("isOk", &FSDKErrorValueInt::isOk)
		.def_readonly("isError", &FSDKErrorValueInt::isError)
		.def_readonly("FSDKError", &FSDKErrorValueInt::fsdkError)
		.def_readonly("what", &FSDKErrorValueInt::what)
		.def_readonly("value", &FSDKErrorValueInt::value)
		.def("__repr__",
			 [](const FSDKErrorValueInt &err) {
				 return "FSDKErrorValueInt: "
					"isOk = " + std::to_string(err.isOk)
					+ ", isError = " + std::to_string(err.isError)
					+ ", FSDKError = " + fsdk::ErrorTraits<fsdk::FSDKError >::toString(err.fsdkError)
					+ ", value = " + std::to_string(err.value)
					+ ", what = " + err.what; })
			;

	py::class_<FSDKErrorValueFloat>(f, "FSDKErrorValueFloat")
		.def_readonly("isOk", &FSDKErrorValueFloat::isOk)
		.def_readonly("isError", &FSDKErrorValueFloat::isError)
		.def_readonly("FSDKError", &FSDKErrorValueFloat::fsdkError)
		.def_readonly("what", &FSDKErrorValueFloat::what)
		.def_readonly("value", &FSDKErrorValueFloat::value)
		.def("__repr__",
			 [](const FSDKErrorValueFloat &err) {
				 return "FSDKErrorValueFloat: "
							"isOk = " + std::to_string(err.isOk)
					+ ", isError = " + std::to_string(err.isError)
					+ ", FSDKError = " + fsdk::ErrorTraits<fsdk::FSDKError >::toString(err.fsdkError)
					+ ", value = " + std::to_string(err.value)
					+ ", what = " + err.what;
			 })
				;

	py::class_<FSDKErrorValueMatching>(f, "FSDKErrorValueMatching")
		.def_readonly("isOk", &FSDKErrorValueMatching::isOk)
		.def_readonly("isError", &FSDKErrorValueMatching::isError)
		.def_readonly("FSDKError", &FSDKErrorValueMatching::fsdkError)
		.def_readonly("what", &FSDKErrorValueMatching::what)
		.def_readonly("value", &FSDKErrorValueMatching::value)
		.def("__repr__",
			 [](const FSDKErrorValueMatching &err) {
				 return "FSDKErrorValueMatching: "
							"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", FSDKError = " + fsdk::ErrorTraits<fsdk::FSDKError >::toString(err.fsdkError)
						+ ",value: (distance = " + std::to_string(err.value.distance) +
					 		", similarity = " + std::to_string(err.value.similarity) + ")"
						+ ", what = " + err.what;
			 })
				;

// Attribute
	py::class_<fsdk::AttributeEstimation>(f, "AttributeEstimation")
		.def(py::init<>())
		.def_readwrite("gender", &fsdk::AttributeEstimation::gender)
		.def_readwrite("glasses", &fsdk::AttributeEstimation::glasses)
		.def_readwrite("age", &fsdk::AttributeEstimation::age)
		.def("__repr__",
		 [](const fsdk::AttributeEstimation &a) {
			 return "AttributeEstimation: "
				"gender = " + std::to_string(a.gender) +
			 	", glasses = " + std::to_string(a.glasses) +
			 	", age = " + std::to_string(a.age);
		 })
			;

//	Quality
	py::class_<fsdk::Quality>(f, "Quality")
		.def(py::init<>())
		.def_readwrite("light", &fsdk::Quality::light)
		.def_readwrite("dark", &fsdk::Quality::dark)
		.def_readwrite("gray", &fsdk::Quality::gray)
		.def_readwrite("blur", &fsdk::Quality::blur)
		.def("__repr__",
		 [](const fsdk::Quality &q) {
			 return "Quality: "
					"light = " + std::to_string(q.light)
					+ ", dark = " + std::to_string(q.dark)
					+ ", gray = " + std::to_string(q.gray)
					+ ", blur = " + std::to_string(q.blur);
		 })
		.def("getQuality", &fsdk::Quality::getQuality)
			;

//	Ethnicity
	py::class_<fsdk::EthnicityEstimation>(f, "EthnicityEstimation")
		.def(py::init<>())
		.def_readwrite("africanAmerican", &fsdk::EthnicityEstimation::africanAmerican)
		.def_readwrite("indian", &fsdk::EthnicityEstimation::indian)
		.def_readwrite("asian", &fsdk::EthnicityEstimation::asian)
		.def_readwrite("caucasian", &fsdk::EthnicityEstimation::caucasian)
		.def("__repr__",
			 [](const fsdk::EthnicityEstimation &e) {
				 return "EthnicityEstimation: "
						"africanAmerican = " + std::to_string(e.africanAmerican)
						+ ", indian = " + std::to_string(e.indian)
						+ ", asian = " + std::to_string(e.asian)
						+ ", caucasian = " + std::to_string(e.caucasian);
			 })
		.def("getEthnicityScore", &fsdk::EthnicityEstimation::getEthnicityScore)
		.def("getPredominantEthnicity", &fsdk::EthnicityEstimation::getPredominantEthnicity)
			;

//	HeadPose
	py::class_<fsdk::HeadPoseEstimation>(f, "HeadPoseEstimation")
		.def(py::init<>())
		.def_readwrite("pitch", &fsdk::HeadPoseEstimation::pitch)
		.def_readwrite("yaw", &fsdk::HeadPoseEstimation::yaw)
		.def_readwrite("roll", &fsdk::HeadPoseEstimation::roll)
		.def("getFrontalFaceType", &fsdk::HeadPoseEstimation::getFrontalFaceType)
		.def("__repr__",
			 [](const fsdk::HeadPoseEstimation &h) {
				 return "HeadPoseEstimation: "
						"pitch = " + std::to_string(h.pitch)
						+ ", yaw = " + std::to_string(h.yaw)
						+ ", roll = " + std::to_string(h.roll);
			 })
				;
	py::enum_<fsdk::HeadPoseEstimation::FrontalFaceType>(f, "FrontalFaceType", py::arithmetic())
		.value("FrontalFace0", fsdk::HeadPoseEstimation::FrontalFace0)
		.value("FrontalFace1", fsdk::HeadPoseEstimation::FrontalFace1)
		.value("FrontalFace2", fsdk::HeadPoseEstimation::FrontalFace2)
			;

	py::class_<fsdk::DepthRange>(f, "DepthRange")
		.def_readwrite("min", &fsdk::DepthRange::min)
		.def_readwrite("max", &fsdk::DepthRange::max)
		.def("isOk", &fsdk::DepthRange::isOk)
		.def("__repr__",
			 [](const fsdk::DepthRange &h) {
				 return "<DepthRange: "
						", min = " + std::to_string(h.min)
						+ ", max = " + std::to_string(h.max);
			 })
				;

	py::class_<fsdk::SmileEstimation>(f, "SmileEstimation")
		.def(py::init<>())
		.def_readwrite("mouth", &fsdk::SmileEstimation::mouth)
		.def_readwrite("smile", &fsdk::SmileEstimation::smile)
		.def_readwrite("occlusion", &fsdk::SmileEstimation::occlusion)
		.def("__repr__",
			 [](const fsdk::SmileEstimation &s) {
				 std::ostringstream mouth;
				 std::ostringstream smile;
				 std::ostringstream occlusion;
				 mouth << s.mouth;
				 smile << s.smile;
				 occlusion << s.occlusion;
				 return "SmileEstimation: "
						"mouth = " + mouth.str()
						+ ", smile = " + smile.str()
						+ ", occlusion = " + occlusion.str();
			 })
				;
	f.def("loadImage", &loadImage, "used only for depth test"
		"");
//	EyeEstimation
	py::class_<fsdk::EyesEstimation>(f, "EyesEstimation")
		.def(py::init<>())
		.def_readwrite("leftEye", &fsdk::EyesEstimation::leftEye)
		.def_readwrite("rightEye", &fsdk::EyesEstimation::rightEye)
			;
	py::enum_<fsdk::EyesEstimation::EyeAttributes::State>(f, "State")
		.value("Closed", fsdk::EyesEstimation::EyeAttributes::State::Closed)
		.value("Open", fsdk::EyesEstimation::EyeAttributes::State::Open)
		.value("Occluded", fsdk::EyesEstimation::EyeAttributes::State::Occluded)
			;

	py::class_<fsdk::EyesEstimation::EyeAttributes>(f, "EyeAttributes")
		.def_property_readonly_static("irisLandmarksCount", [] (const fsdk::EyesEstimation::EyeAttributes& e)
			{return e.irisLandmarksCount; })
		.def_property_readonly_static("eyelidLandmarksCount", [] (const fsdk::EyesEstimation::EyeAttributes& e)
			{return e.eyelidLandmarksCount; })
		.def_readwrite("state", &fsdk::EyesEstimation::EyeAttributes::state)
		.def_readwrite("iris", &fsdk::EyesEstimation::EyeAttributes::iris)
		.def_readwrite("eyelid", &fsdk::EyesEstimation::EyeAttributes::eyelid)
			;

// Emotions
	py::class_<fsdk::EmotionsEstimation>(f, "EmotionsEstimation")
		.def(py::init<>())
		.def_readwrite("anger", &fsdk::EmotionsEstimation::anger)
		.def_readwrite("disgust", &fsdk::EmotionsEstimation::disgust)
		.def_readwrite("fear", &fsdk::EmotionsEstimation::fear)
		.def_readwrite("happiness", &fsdk::EmotionsEstimation::happiness)
		.def_readwrite("sadness", &fsdk::EmotionsEstimation::sadness)
		.def_readwrite("surprise", &fsdk::EmotionsEstimation::surprise)
		.def_readwrite("neutral", &fsdk::EmotionsEstimation::neutral)
		.def("getPredominantEmotion", &fsdk::EmotionsEstimation::getPredominantEmotion)
		.def("getEmotionScore", &fsdk::EmotionsEstimation::getEmotionScore)
		.def("__repr__",
			 [](const fsdk::EmotionsEstimation &e) {
				 return "EmotionsEstimation: "
						" anger = " + std::to_string(e.anger)
						+ ", disgust = " + std::to_string(e.disgust)
						+ ", fear = " + std::to_string(e.fear)
						+ ", happiness = " + std::to_string(e.happiness)
						+ ", sadness = " + std::to_string(e.sadness)
						+ ", surprise = " + std::to_string(e.surprise)
						+ ", neutral = " + std::to_string(e.neutral);
			 })
			;

	py::enum_<fsdk::EmotionsEstimation::Emotions>(f, "Emotions", py::arithmetic())
		.value("Anger", fsdk::EmotionsEstimation::Anger)
		.value("Disgust", fsdk::EmotionsEstimation::Disgust)
		.value("Fear", fsdk::EmotionsEstimation::Fear)
		.value("Happiness", fsdk::EmotionsEstimation::Happiness)
		.value("Sadness", fsdk::EmotionsEstimation::Sadness)
		.value("Surprise", fsdk::EmotionsEstimation::Surprise)
		.value("Neutral", fsdk::EmotionsEstimation::Neutral)
		.value("Count", fsdk::EmotionsEstimation::Count)
			;

// Gaze
	py::class_<fsdk::GazeEstimation>(f, "GazeEstimation")
		.def(py::init<>())
		.def_readwrite("leftEye", &fsdk::GazeEstimation::leftEye)
		.def_readwrite("rightEye", &fsdk::GazeEstimation::rightEye)
		.def("__repr__",
			 [](const fsdk::GazeEstimation &g) {
				 return "GazeEstimation: "
						"leftEye: yaw = " + std::to_string(g.leftEye.yaw) +
						", pitch = " + std::to_string(g.leftEye.pitch) +
						", rightEye: yaw = " + std::to_string(g.rightEye.yaw) +
						", pitch = " + std::to_string(g.rightEye.pitch);
			 })
				;

	py::class_<fsdk::GazeEstimation::EyeAngles>(f, "EyeAngles")
		.def_readwrite("yaw", &fsdk::GazeEstimation::EyeAngles::yaw)
		.def_readwrite("pitch", &fsdk::GazeEstimation::EyeAngles::pitch)
		.def("__repr__",
			 [](const fsdk::GazeEstimation::EyeAngles &e) {
				 return "EyeAngles: "
						"yaw = " + std::to_string(e.yaw)
						+ ", pitch = " + std::to_string(e.pitch);
			})
				;

//	Ethnicity
	py::enum_<fsdk::EthnicityEstimation::Ethnicities>(f, "Ethnicity")
		.value("AfricanAmerican", fsdk::EthnicityEstimation::AfricanAmerican)
		.value("Indian", fsdk::EthnicityEstimation::Indian)
		.value("Asian", fsdk::EthnicityEstimation::Asian)
		.value("Caucasian", fsdk::EthnicityEstimation::Caucasian)
		.value("Count", fsdk::EthnicityEstimation::Count)
			.export_values();
			;

//	Transformation
	py::class_<fsdk::Transformation>(f, "Transformation")
		.def(py::init<>())
		.def_readwrite("angleDeg", &fsdk::Transformation::angleDeg)
		.def_readwrite("scale", &fsdk::Transformation::scale)
		.def_readwrite("centerP", &fsdk::Transformation::centerP)
		.def_readwrite("detectionTopLeft", &fsdk::Transformation::detectionTopLeft)
		.def("__repr__", [](const fsdk::Transformation &t) {
			 return "Transformation: "
					" angleDeg= " + std::to_string(t.angleDeg)
					+ ", scale = " + std::to_string(t.scale)
					+ ", centerP: x = " + std::to_string(t.centerP.x) + " y = " + std::to_string(t.centerP.y)
					+ ", detectionTopLeft: x = " + std::to_string(t.detectionTopLeft.x)
					+ " y = " + std::to_string(t.detectionTopLeft.y);
			 })
			;
// Image type and format
	py::enum_<fsdk::Format::Type>(f, "FormatType")
		.value("Unknown", fsdk::Format::Unknown)
		.value("B8G8R8X8", fsdk::Format::B8G8R8X8)
		.value("R8G8B8X8", fsdk::Format::R8G8B8X8)
		.value("B8G8R8", fsdk::Format::B8G8R8)
		.value("R8G8B8", fsdk::Format::R8G8B8)
		.value("R8", fsdk::Format::R8)
//		R16 is used for internal transformations for depth map and cannot be saved or downloaded
		.value("R16", fsdk::Format::R16)
			;

	py::class_<fsdk::Format>(f, "Format")
		.def(py::init<>())
		.def(py::init<fsdk::Format::Type>())
		.def("getChannelCount", &fsdk::Format::getChannelCount)
		.def("getChannelStep", &fsdk::Format::getChannelStep)
		.def("getChannelSize", &fsdk::Format::getChannelSize)
		.def("getBitDepth", &fsdk::Format::getBitDepth)
		.def("getByteDepth", &fsdk::Format::getByteDepth)
		.def("computePitch", &fsdk::Format::computePitch)
		.def("isPadded", &fsdk::Format::isPadded)
		.def("isBGR", &fsdk::Format::isBGR)
		.def("isBlock", &fsdk::Format::isValid)
			;

	py::class_<fsdk::Image>(f, "Image")
		.def(py::init<>())
		.def("getWidth", &fsdk::Image::getWidth)
		.def("getHeight", &fsdk::Image::getHeight)
		.def("isValid", &fsdk::Image::isValid)
		.def("getRect", &fsdk::Image::getRect)
		.def("getDataAsList", [](const fsdk::Image& image) {
			py::list py_matr;
			for (int i = 0; i < image.getHeight(); ++i) {
				const auto* const data_str = image.getScanLineAs<uint8_t>(i);
				py::list py_str;
				for (int j = 0; j < image.getWidth(); ++j) {
					py_str.append(data_str[j]);
				}
				py_matr.append(py_str);
			}
			return py_matr;
		})
		.def("getData", [](const fsdk::Image& image) {
			const auto* const data_uint = image.getDataAs<uint8_t>();
			std::vector<uint8_t> data(data_uint, data_uint + image.getDataSize());
			std::vector<ssize_t> shape { image.getWidth(), image.getHeight() };
			auto ptr = data.data();
			return py::array(shape, ptr);

		})
		.def("save", [](const fsdk::Image& image, const char* path) {
			fsdk::Result<fsdk::Image::Error> error = image.save(path);
			return ImageErrorResult(error);
		})
		.def("save", [](const fsdk::Image& image,
						const char* path,
						const fsdk::Format format) {
				fsdk::Result<fsdk::Image::Error> error = image.save(path, format);
				return ImageErrorResult(error);
			})
		.def("load",[](fsdk::Image& image, const char* path) {
			fsdk::Result<fsdk::Image::Error> error = image.load(path);
			return ImageErrorResult(error); })

		.def("load", [](fsdk::Image& image,
						const char* path,
						const fsdk::Format format) {
			fsdk::Result<fsdk::Image::Error> error = image.load(path, format);
			return ImageErrorResult(error);
			})
				;

	py::enum_<fsdk::Image::Type>(f, "ImageType")
		.value("BMP", fsdk::Image::Type::BMP)
		.value("JPG", fsdk::Image::Type::JPG)
		.value("PNG", fsdk::Image::Type::PNG)
		.value("PPM", fsdk::Image::Type::PPM)
		.value("TIFF", fsdk::Image::Type::TIFF)
		.value("Unknown", fsdk::Image::Type::Unknown)
			;

	py::enum_<fsdk::Image::Error>(f, "ImageError")
		.value("Ok", fsdk::Image::Error::Ok)
		.value("InvalidWidth", fsdk::Image::Error::InvalidWidth)
		.value("InvalidHeight", fsdk::Image::Error::InvalidHeight)
		.value("InvalidDataPtr", fsdk::Image::Error::InvalidDataPtr)
		.value("InvalidDataSize", fsdk::Image::Error::InvalidDataSize)
		.value("InvalidImage", fsdk::Image::Error::InvalidImage)
		.value("InvalidArchive", fsdk::Image::Error::InvalidArchive)
		.value("InvalidPath", fsdk::Image::Error::InvalidPath)
		.value("InvalidType", fsdk::Image::Error::InvalidType)
		.value("InvalidFormat", fsdk::Image::Error::InvalidFormat)
		.value("InvalidBitmap", fsdk::Image::Error::InvalidBitmap)
		.value("InvalidMemory", fsdk::Image::Error::InvalidMemory)
		.value("InvalidConversion", fsdk::Image::Error::InvalidConversion)
		.value("FailedToSave", fsdk::Image::Error::FailedToSave)
		.value("FailedToLoad", fsdk::Image::Error::FailedToLoad)
		.value("FailedToInitialize", fsdk::Image::Error::FailedToInitialize)
			;

	py::enum_<fsdk::ISettingsProvider::Error>(f, "SettingsProviderError")
		.value("Ok", fsdk::ISettingsProvider::Error::Ok)
		.value("IOError", fsdk::ISettingsProvider::Error::IOError)
		.value("Memory", fsdk::ISettingsProvider::Error::Memory)
		.value("Internal", fsdk::ISettingsProvider::Error::Internal)
		.value("InvalidPi", fsdk::ISettingsProvider::Error::InvalidPi)
		.value("InvalidTag", fsdk::ISettingsProvider::Error::InvalidTag)
		.value("InvalidCdata", fsdk::ISettingsProvider::Error::InvalidCdata)
		.value("FileNotFound", fsdk::ISettingsProvider::Error::FileNotFound)
		.value("InvalidPcdata", fsdk::ISettingsProvider::Error::InvalidPcdata)
		.value("InvalidDocType", fsdk::ISettingsProvider::Error::InvalidDocType)
		.value("InvalidSettings", fsdk::ISettingsProvider::Error::InvalidSettings)
		.value("InvalidComment", fsdk::ISettingsProvider::Error::InvalidComment)
		.value("InvalidAttribute", fsdk::ISettingsProvider::Error::InvalidAttribute)
		.value("InvalidEndElement", fsdk::ISettingsProvider::Error::InvalidEndElement)
		.value("AppendInvalidRoot", fsdk::ISettingsProvider::Error::AppendInvalidRoot)
		.value("NoDocumentElement", fsdk::ISettingsProvider::Error::NoDocumentElement)
		.value("EndElementMismatch", fsdk::ISettingsProvider::Error::EndElementMismatch)
		.value("InvalidStartElement", fsdk::ISettingsProvider::Error::InvalidStartElement)
			;

	py::class_<fsdk::Detection>(f, "Detection")
		.def(py::init<>())
		.def_readwrite("rect", &fsdk::Detection::rect)
		.def_readwrite("score", &fsdk::Detection::score)
		.def("isValid", &fsdk::Detection::isValid)
		.def("__repr__",
			 [](const fsdk::Detection &d) {
				 return "Detection: rect: x = " + std::to_string(d.rect.x) +
				 ", y = " + std::to_string(d.rect.y) +
				 ", width = " + std::to_string(d.rect.width) +
				 ", height = " + std::to_string(d.rect.height) +
						"; score = " + std::to_string(d.score) +
						"; isValid = " + std::to_string(d.isValid());
			 	})
			;

	py::class_<fsdk::Rect>(f, "Rect")
		.def(py::init<>())
		.def(py::init<int, int, int, int>())
		.def(py::init<fsdk::Vector2<int>, fsdk::Vector2<int>>())
		.def(py::init<const fsdk::Rect&>())
		.def(py::self != py::self)
		.def(py::self == py::self)
		.def_readwrite("x", &fsdk::Rect::x)
		.def_readwrite("y", &fsdk::Rect::y)
		.def_readwrite("width", &fsdk::Rect::width)
		.def_readwrite("height", &fsdk::Rect::height)
		.def_static("coords", &fsdk::Rect::coords)
		.def("size", &fsdk::Rect::size)
		.def("topLeft", &fsdk::Rect::topLeft)
		.def("center", &fsdk::Rect::center)
		.def("bottomRight", &fsdk::Rect::bottomRight)
		.def("top", &fsdk::Rect::top)
		.def("bottom", &fsdk::Rect::bottom)
		.def("left", &fsdk::Rect::left)
		.def("right", &fsdk::Rect::right)
		.def("set", &fsdk::Rect::set)
		.def("adjust", &fsdk::Rect::adjust)
		.def("adjusted", &fsdk::Rect::adjusted)
		.def("getArea", &fsdk::Rect::getArea)
		.def("inside", &fsdk::Rect::inside)
		.def("isValid", &fsdk::Rect::isValid)
		.def("__repr__",
			 [](const fsdk::Rect &r) {
				 return "x = " + std::to_string(r.x) +
				 ", y = " + std::to_string(r.y) +
				 ", width = " + std::to_string(r.width) +
				 ", height = " + std::to_string(r.height);
			 })
				;

	py::enum_<fsdk::ObjectDetectorClassType>(f, "ObjectDetectorClassType", py::arithmetic())
		.value("ODT_MTCNN", fsdk::ODT_MTCNN)
		.value("ODT_COUNT", fsdk::ODT_COUNT)
		.export_values();
			;

	py::enum_<fsdk::FSDKError>(f, "FSDKError")
		.value("Ok", fsdk::FSDKError::Ok)
		.value("Internal", fsdk::FSDKError::Internal)
		.value("InvalidInput", fsdk::FSDKError::InvalidInput)
		.value("InvalidImage", fsdk::FSDKError::InvalidImage)

		.value("InvalidRect", fsdk::FSDKError::InvalidRect)
		.value("InvalidImageFormat", fsdk::FSDKError::InvalidImageFormat)
		.value("InvalidImageSize", fsdk::FSDKError::InvalidImageSize)
		.value("InvalidDetection", fsdk::FSDKError::InvalidDetection)
		.value("InvalidLandmarks5", fsdk::FSDKError::InvalidLandmarks5)
		.value("InvalidLandmarks68", fsdk::FSDKError::InvalidLandmarks68)
		.value("InvalidTransformation", fsdk::FSDKError::InvalidTransformation)
		.value("InvalidDescriptor", fsdk::FSDKError::InvalidDescriptor)
		.value("InvalidDescriptorBatch", fsdk::FSDKError::InvalidDescriptorBatch)
		.value("InvalidSettingsProvider", fsdk::FSDKError::InvalidSettingsProvider)

		.value("ModuleNotInitialized", fsdk::FSDKError::ModuleNotInitialized)
		.value("ModuleNotReady", fsdk::FSDKError::ModuleNotReady)
		.value("LicenseError", fsdk::FSDKError::LicenseError)

		.value("BufferIsNull", fsdk::FSDKError::BufferIsNull)
		.value("BufferIsFull", fsdk::FSDKError::BufferIsFull)
		.value("BufferIsEmpty", fsdk::FSDKError::BufferIsEmpty)
		.value("InvalidBufferSize", fsdk::FSDKError::InvalidBufferSize)

		.value("IncompatibleDescriptors", fsdk::FSDKError::IncompatibleDescriptors)
			;
}


