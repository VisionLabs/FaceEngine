#include <pybind11/pybind11.h>
#include <iostream>
#include <fstream>
#include <fsdk/FaceEngine.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

//class overload_caster_t;
//
//int add(int i, int j) {
//    return i + j;
//}
//
//namespace py = pybind11;
//
//PYBIND11_MODULE(cmake_example, m) {
//    m.doc() = R"pbdoc(
//        Pybind11 example plugin
//        -----------------------
//
//        .. currentmodule:: cmake_example
//
//        .. autosummary::
//           :toctree: _generate
//
//           add
//           subtract
//    )pbdoc";
//
//    m.def("add", &add, R"pbdoc(
//        Add two numbers
//
//        Some other explanation about the add function.
//    )pbdoc");
//
//    m.def("subtract", [](int i, int j) { return i - j; }, R"pbdoc(
//        Subtract two numbers
//
//        Some other explanation about the subtract function.
//    )pbdoc");
//
//#ifdef VERSION_INFO
//    m.attr("__version__") = VERSION_INFO;
//#else
//    m.attr("__version__") = "dev";
//#endif
//}



namespace py = pybind11;

struct SettingsProviderResult {
	bool isOk;
	bool isError;
	fsdk::ISettingsProvider::Error settingsProviderError;
	const char* what;

	SettingsProviderResult(fsdk::Result<fsdk::ISettingsProvider::Error> err) :
		isOk(err.isOk()),
		isError(err.isError()),
		settingsProviderError(err.getError()),
		what(err.what())
	{};
};

class PyISettingsProvider {
public:
	fsdk::ISettingsProviderPtr settingsProviderPtr;

	PyISettingsProvider(const char* path = nullptr) {
		settingsProviderPtr = fsdk::acquire(fsdk::createSettingsProvider(path));
	}

	const char* getDefaultPath() {
		settingsProviderPtr->getDefaultPath();
	}

	SettingsProviderResult load(const char* path) {
		return SettingsProviderResult(settingsProviderPtr->load(path));
	}

	bool save(const char* path) {
		return settingsProviderPtr->save(path);
	}

	void clear() {
		settingsProviderPtr->clear();
	}

	void isEmpty() {
		settingsProviderPtr->isEmpty();
	}

	void setValue(
		const char* section,
		const char* parameter,
		const fsdk::ISettingsProvider::Value& value) {
		settingsProviderPtr->setValue(section, parameter, value);
	}

	fsdk::ISettingsProvider::Value getValue(const char* section, const char* parameter) {
		return settingsProviderPtr->getValue(section, parameter);
	}
};

class PyIFaceEngine {
public:
	fsdk::IFaceEnginePtr faceEnginePtr;

	PyIFaceEngine(const char* dataPath = nullptr, const char* configPath = nullptr) {
		faceEnginePtr = fsdk::acquire(fsdk::createFaceEngine(dataPath, configPath));
	}

	fsdk::IDetectorPtr createDetector(fsdk::ObjectDetectorClassType type) {
		return fsdk::acquire(faceEnginePtr->createDetector(type));
	}

	fsdk::IAttributeEstimatorPtr createAttributeEstimator() {
		return fsdk::acquire(faceEnginePtr->createAttributeEstimator());
	}

	fsdk::IQualityEstimatorPtr createQualityEstimator() {
		return fsdk::acquire(faceEnginePtr->createQualityEstimator());

	}

	fsdk::IEthnicityEstimatorPtr createEthnicityEstimator() {
		return fsdk::acquire(faceEnginePtr->createEthnicityEstimator());
	}

//	warper
	fsdk::IWarperPtr createWarper() {
		return fsdk::acquire(faceEnginePtr->createWarper());
	}
//	descriptor
	fsdk::IDescriptorPtr createDescriptor() {
		return fsdk::acquire(faceEnginePtr->createDescriptor());
	}

	fsdk::IDescriptorBatchPtr createDescriptorBatch(int32_t size, int32_t version = 0) {
		return fsdk::acquire(faceEnginePtr->createDescriptorBatch(size, version));
	}

	fsdk::IDescriptorExtractorPtr createExtractor() {
		return fsdk::acquire(faceEnginePtr->createExtractor());
	}

	fsdk::IDescriptorMatcherPtr createMatcher() {
		return fsdk::acquire(faceEnginePtr->createMatcher());
	}

	fsdk::ILSHTablePtr createLSHTable(const fsdk::IDescriptorBatchPtr& batch) {
		return fsdk::acquire(faceEnginePtr->createLSHTable(batch));
	}

//	second part of estimators
	fsdk::IHeadPoseEstimatorPtr createHeadPoseEstimator() {
		return fsdk::acquire(faceEnginePtr->createHeadPoseEstimator());
	}

	fsdk::Ref<fsdk::IBlackWhiteEstimator> createBlackWhiteEstimator() {
		return fsdk::acquire(faceEnginePtr->createBlackWhiteEstimator());
	}

	fsdk::ILivenessDepthEstimatorPtr createDepthEstimator() {
		return fsdk::acquire(faceEnginePtr->createDepthEstimator());
	}

	fsdk::ILivenessIREstimatorPtr createIREstimator() {
		return fsdk::acquire(faceEnginePtr->createIREstimator());
	}

	fsdk::ISmileEstimatorPtr createSmileEstimator() {
		return fsdk::acquire(faceEnginePtr->createSmileEstimator());
	}

	fsdk::ILivenessFlowEstimatorPtr createFaceFlowEstimator() {
		return fsdk::acquire(faceEnginePtr->createFaceFlowEstimator());
	}

	fsdk::IEyeEstimatorPtr createEyeEstimator() {
		return fsdk::acquire(faceEnginePtr->createEyeEstimator());
	}

	fsdk::IEmotionsEstimatorPtr createEmotionsEstimator() {
		return fsdk::acquire(faceEnginePtr->createEmotionsEstimator());
	}

	fsdk::IGazeEstimatorPtr createGazeEstimator() {
		return fsdk::acquire(faceEnginePtr->createGazeEstimator());
	}

	void setSettingsProvider(PyISettingsProvider& provider) {
		faceEnginePtr->setSettingsProvider(provider.settingsProviderPtr);
	}
};

PyIFaceEngine createPyFaceEnginePtr(const char* dataPath = nullptr, const char* configPath = nullptr) {
	return PyIFaceEngine(dataPath, configPath);
}

PyISettingsProvider createSettingsProviderPtr(const char* path) {
	return PyISettingsProvider(path);
}

struct FSDKErrorResult {
	
	bool isOk;
	bool isError;
	fsdk::FSDKError fsdkError;
	const char* what;

	FSDKErrorResult(fsdk::Result<fsdk::FSDKError> err) :
		isOk(err.isOk()),
		isError(err.isError()),
		fsdkError(err.getError()),
		what(err.what())
		{};
};

struct ImageErrorResult {
	bool isOk;
	bool isError;
	fsdk::Image::Error imageError;
	const char* what;

	ImageErrorResult(fsdk::Result<fsdk::Image::Error> err) :
		isOk(err.isOk()),
		isError(err.isError()),
		imageError(err.getError()),
		what(err.what())
		{};
};

struct DescriptorBatchResult {
	bool isOk;
	bool isError;
	fsdk::IDescriptorBatch::Error descriptorBatchError;
	const char* what;

	DescriptorBatchResult(fsdk::Result<fsdk::IDescriptorBatch::Error> err) :
		isOk(err.isOk()),
		isError(err.isError()),
		descriptorBatchError(err.getError()),
		what(err.what())
		{};
};

struct FSDKErrorValueInt {
	bool isOk;
	bool isError;
	fsdk::FSDKError fsdkError;
	const char* what;
	int value;

	FSDKErrorValueInt(fsdk::ResultValue<fsdk::FSDKError, int> err) :
		isOk(err.isOk()),
		isError(err.isError()),
		fsdkError(err.getError()),
		what(err.what()),
		value(err.getValue())
		{};

};

struct FSDKErrorValueFloat {
	bool isOk;
	bool isError;
	fsdk::FSDKError fsdkError;
	const char* what;
	int value;

	FSDKErrorValueFloat(fsdk::ResultValue<fsdk::FSDKError, float> err) :
		isOk(err.isOk()),
		isError(err.isError()),
		fsdkError(err.getError()),
		what(err.what()),
		value(err.getValue())
	{};

};

struct FSDKErrorValueMatching {
	bool isOk;
	bool isError;
	fsdk::FSDKError fsdkError;
	const char* what;
	fsdk::MatchingResult value;

	FSDKErrorValueMatching(fsdk::ResultValue<fsdk::FSDKError, fsdk::MatchingResult> err) :
		isOk(err.isOk()),
		isError(err.isError()),
		fsdkError(err.getError()),
		what(err.what()),
		value(err.getValue())
	{};

};

fsdk::Image loadImage(const char* name) {
	std::ifstream file(name, std::ios::in|std::ios::binary);

	int channels =0;
	int elementSize = 0;
	int rows = 0;
	int cols = 0;

	file.read((char*)&channels,sizeof(channels));
	file.read((char*)&elementSize,sizeof(elementSize));
	file.read((char*)&rows,sizeof(rows));
	file.read((char*)&cols,sizeof(cols));

	fsdk::Image image(cols,rows,fsdk::Format::R16);

	file.read((char*)image.getData(),rows*cols*channels*elementSize);

	return image;
};

struct DetectionResult {
	fsdk::Detection detection;
	fsdk::Landmarks5 landmarks5;
	fsdk::Landmarks68 landmarks68;
	DetectionResult(const fsdk::Detection &detection,
					const fsdk::Landmarks5& landmarks5,
					const fsdk::Landmarks68&) :
		detection(detection), landmarks5(landmarks5), landmarks68(landmarks68) {}
};

PYBIND11_MAKE_OPAQUE(fsdk::Landmarks5);
PYBIND11_MAKE_OPAQUE(fsdk::Landmarks68);

PYBIND11_MODULE(FaceEngine, f) {

	f.def("createPyFaceEnginePtr", &createPyFaceEnginePtr, py::return_value_policy::take_ownership,
		  "Create FaceEngine", py::arg("dataPath") = nullptr, py::arg("configPath") = nullptr);
	f.def("createSettingsProviderPtr", &createSettingsProviderPtr, py::return_value_policy::take_ownership,
		  "Create object SettingsProvider");

	py::class_<PyIFaceEngine>(f, "PyIFaceEngine")
		.def("createAttributeEstimator", &PyIFaceEngine::createAttributeEstimator)
		.def("createQualityEstimator", &PyIFaceEngine::createQualityEstimator)
		.def("createEthnicityEstimator", &PyIFaceEngine::createEthnicityEstimator)
		.def("createDetector", &PyIFaceEngine::createDetector)
		.def("createWarper", &PyIFaceEngine::createWarper)
		.def("createDescriptor", &PyIFaceEngine::createDescriptor)
		.def("createDescriptorBatch", &PyIFaceEngine::createDescriptorBatch, py::arg("size"), py::arg("version") = 0)
		.def("createExtractor", &PyIFaceEngine::createExtractor)
		.def("createMatcher", &PyIFaceEngine::createMatcher)
		.def("createLSHTable", &PyIFaceEngine::createLSHTable)
		.def("setSettingsProvider", &PyIFaceEngine::setSettingsProvider)

		.def("createHeadPoseEstimator", &PyIFaceEngine::createHeadPoseEstimator)
		.def("createBlackWhiteEstimator", &PyIFaceEngine::createBlackWhiteEstimator)
		.def("createDepthEstimator", &PyIFaceEngine::createDepthEstimator)
		.def("createIREstimator", &PyIFaceEngine::createIREstimator)
		.def("createSmileEstimator", &PyIFaceEngine::createSmileEstimator)
		.def("createFaceFlowEstimator", &PyIFaceEngine::createFaceFlowEstimator)
		.def("createEyeEstimator", &PyIFaceEngine::createEyeEstimator)
		.def("createEmotionsEstimator", &PyIFaceEngine::createEmotionsEstimator)
		.def("createGazeEstimator", &PyIFaceEngine::createGazeEstimator)
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
		.def(py::init<>())
		.def(py::init<int>())
		.def(py::init<int, int>())
		.def(py::init<int, int, int>())
		.def(py::init<int, int, int, int>())
		.def(py::init<float>())
		.def(py::init<float, float>())
		.def(py::init<float, float, float>())
		.def(py::init<float, float, float, float>())
		.def(py::init<const char*>())
		.def(py::init<const fsdk::Rect&>())
		.def(py::init<bool>())
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
						detectionResultPyList.append(py::make_tuple(detections[i], landmarks[i], landmarks68[i]));
						std::cout << i << std::endl;
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
				fsdk::Result<fsdk::FSDKError> err = est->estimate(image);
				return FSDKErrorResult(err); })
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
				 return "<example.MatchingResult: "
							"distance = " + std::to_string(result.distance)
						+ ", similarity = " + std::to_string(result.similarity) + "'>"; })
			;


	py::class_<fsdk::Landmarks5>(f, "Landmarks5")
		.def(py::init<>())
		.def("__len__", [](const fsdk::Landmarks5 &v) { return v.landmarkCount; })
		.def("__getitem__", [](const fsdk::Landmarks5 &s, size_t i) {
			if (i >= s.landmarkCount) throw py::index_error();
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
			;

	py::class_<fsdk::EyesEstimation::EyeAttributes::IrisLandmarks>(f, "IrisLandmarks")
		.def(py::init<>())
		.def("__len__", [](const fsdk::EyesEstimation::EyeAttributes &v) { return v.irisLandmarksCount; })
		.def("__getitem__", [](const fsdk::EyesEstimation::EyeAttributes::IrisLandmarks &l, size_t i) {
			if (i >= fsdk::EyesEstimation::EyeAttributes::irisLandmarksCount) throw py::index_error();
			return l.landmarks[i];
		})
			;

	py::class_<fsdk::EyesEstimation::EyeAttributes::EyelidLandmarks>(f, "EyelidLandmarks")
		.def(py::init<>())
		.def("__len__", [](const fsdk::EyesEstimation::EyeAttributes &v) { return v.eyelidLandmarksCount; })
		.def("__getitem__", [](const fsdk::EyesEstimation::EyeAttributes::EyelidLandmarks &l, size_t i) {
			if (i >= fsdk::EyesEstimation::EyeAttributes::eyelidLandmarksCount) throw py::index_error();
			return l.landmarks[i];
		})
			;

// Vector2
	py::class_<fsdk::Vector2<float>>(f, "Vector2f")
		.def(py::init<>())
		.def(py::init<float, float>())
		.def(py::init<const fsdk::Vector2<float>&>())
		.def_readwrite("x", &fsdk::Vector2<float>::x)
		.def_readwrite("y", &fsdk::Vector2<float>::y)
		.def("__repr__",
			 [](const fsdk::Vector2<float> &v) {
				 return "<Vector2f: x = " + std::to_string(v.x) + ", y = " + std::to_string(v.y) + ">";
		 	})
				;

	py::class_<fsdk::Vector2<int>>(f, "Vector2i")
		.def(py::init<>())
		.def(py::init<int, int>())
		.def(py::init<const fsdk::Vector2<int>&>())
		.def_readwrite("x", &fsdk::Vector2<int>::x)
		.def_readwrite("y", &fsdk::Vector2<int>::y)
		.def("__repr__",
			 [](const fsdk::Vector2<int> &v) {
				 return "<Vector2i: x = " + std::to_string(v.x) + ", y = " + std::to_string(v.y) + ">";
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
				 return "<example.FSDKErrorResult: "
					"isOk = " + std::to_string(err.isOk)
					+ ", isError = " + std::to_string(err.isError)
					+ ", FSDKError = " + fsdk::ErrorTraits<fsdk::FSDKError >::toString(err.fsdkError)
					+ ", what = " + err.what +  "'>"; })
			;

	py::class_<DescriptorBatchResult>(f, "DescriptorBatchResult")
		.def_readonly("isOk", &DescriptorBatchResult::isOk)
		.def_readonly("isError", &DescriptorBatchResult::isError)
		.def_readonly("DescriptorBatchError", &DescriptorBatchResult::descriptorBatchError)
		.def_readonly("what", &DescriptorBatchResult::what)
		.def("__repr__",
			 [](const DescriptorBatchResult &err) {
				 return "<example.DescriptorBatchResult: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", DescriptorBatchError = " +
				 fsdk::ErrorTraits<fsdk::IDescriptorBatch::Error>::toString(err.descriptorBatchError)
						+ ", what = " + err.what +  "'>"; })
			;

	py::class_<ImageErrorResult>(f, "ImageErrorResult")
		.def_readonly("isOk", &ImageErrorResult::isOk)
		.def_readonly("isError", &ImageErrorResult::isError)
		.def_readonly("ImageErrorResult", &ImageErrorResult::imageError)
		.def_readonly("what", &ImageErrorResult::what)
		.def("__repr__",
			 [](const ImageErrorResult &err) {
				 return "<example.ImageErrorResult: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", ImageError = " + fsdk::ErrorTraits<fsdk::Image::Error>::toString(err.imageError)
						+ ", what = " + err.what +  "'>"; })
			;

	py::class_<SettingsProviderResult>(f, "SettingsProviderResult")
		.def_readonly("isOk", &SettingsProviderResult::isOk)
		.def_readonly("isError", &SettingsProviderResult::isError)
		.def_readonly("SettingsProviderResult", &SettingsProviderResult::settingsProviderError)
		.def_readonly("what", &SettingsProviderResult::what)
		.def("__repr__",
			 [](const SettingsProviderResult &err) {
				 return "<example.SettingsProviderResult: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", SettingsProviderError = " +
				 			fsdk::ErrorTraits<fsdk::ISettingsProvider::Error>::
							 toString(err.settingsProviderError)
						+ ", what = " + err.what +  "'>"; })
				;

	py::class_<FSDKErrorValueInt>(f, "FSDKErrorValueInt")
		.def_readonly("isOk", &FSDKErrorValueInt::isOk)
		.def_readonly("isError", &FSDKErrorValueInt::isError)
		.def_readonly("FSDKError", &FSDKErrorValueInt::fsdkError)
		.def_readonly("what", &FSDKErrorValueInt::what)
		.def_readonly("value", &FSDKErrorValueInt::value)
		.def("__repr__",
			 [](const FSDKErrorValueInt &err) {
				 return "<example.FSDKErrorValueInt: "
					"isOk = " + std::to_string(err.isOk)
					+ ", isError = " + std::to_string(err.isError)
					+ ", FSDKError = " + fsdk::ErrorTraits<fsdk::FSDKError >::toString(err.fsdkError)
					+ ", value = " + std::to_string(err.value)
					+ ", what = " + err.what + "'>"; })
			;

	py::class_<FSDKErrorValueFloat>(f, "FSDKErrorValueFloat")
		.def_readonly("isOk", &FSDKErrorValueFloat::isOk)
		.def_readonly("isError", &FSDKErrorValueFloat::isError)
		.def_readonly("FSDKError", &FSDKErrorValueFloat::fsdkError)
		.def_readonly("what", &FSDKErrorValueFloat::what)
		.def_readonly("value", &FSDKErrorValueFloat::value)
		.def("__repr__",
			 [](const FSDKErrorValueFloat &err) {
				 return "<example.FSDKErrorValueFloat: "
							"isOk = " + std::to_string(err.isOk)
					+ ", isError = " + std::to_string(err.isError)
					+ ", FSDKError = " + fsdk::ErrorTraits<fsdk::FSDKError >::toString(err.fsdkError)
					+ ", value = " + std::to_string(err.value)
					+ ", what = " + err.what + "'>";
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
				 return "<example.FSDKErrorValueMatching: "
							"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", FSDKError = " + fsdk::ErrorTraits<fsdk::FSDKError >::toString(err.fsdkError)
						+ ",value: (distance = " + std::to_string(err.value.distance) +
					 		", similarity = " + std::to_string(err.value.similarity) + ")"
						+ ", what = " + err.what + "'>";
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
			 return "<example.AttributeEstimation: "
				"gender = " + std::to_string(a.gender) +
			 	", glasses = " + std::to_string(a.glasses) +
			 	", age = " + std::to_string(a.age)  + "'>";
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
			 return "<example.Quality: "
					", light = " + std::to_string(q.light)
					+ ", dark = " + std::to_string(q.dark)
					+ ", gray = " + std::to_string(q.gray)
					+ ", blur = " + std::to_string(q.blur) +  "'>";
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
				 return "<example.EthnicityEstimation: "
						", africanAmerican = " + std::to_string(e.africanAmerican)
						+ ", indian = " + std::to_string(e.indian)
						+ ", asian = " + std::to_string(e.asian)
						+ ", caucasian = " + std::to_string(e.caucasian) +  "'>";
			 })
		.def("getEthnicityScore", &fsdk::EthnicityEstimation::getEthnicityScore)
		.def("getPredominantEthnicity", &fsdk::EthnicityEstimation::getPredominantEthnicity)
			;

//	HeadPose
	py::class_<fsdk::HeadPoseEstimation>(f, "HeadPoseEstimation")
		.def_readwrite("pitch", &fsdk::HeadPoseEstimation::pitch)
		.def_readwrite("yaw", &fsdk::HeadPoseEstimation::yaw)
		.def_readwrite("roll", &fsdk::HeadPoseEstimation::roll)
		.def("getFrontalFaceType", &fsdk::HeadPoseEstimation::getFrontalFaceType)
		.def("__repr__",
			 [](const fsdk::HeadPoseEstimation &h) {
				 return "<example.HeadPoseEstimation: "
						", pitch = " + std::to_string(h.pitch)
						+ ", yaw = " + std::to_string(h.yaw)
						+ ", roll = " + std::to_string(h.roll) +  "'>";
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
				 return "<example.DepthRange: "
						", min = " + std::to_string(h.min)
						+ ", max = " + std::to_string(h.max) +  "'>";
			 })
				;

	py::class_<fsdk::SmileEstimation>(f, "SmileEstimation")
		.def_readwrite("mouth", &fsdk::SmileEstimation::mouth)
		.def_readwrite("smile", &fsdk::SmileEstimation::smile)
		.def_readwrite("occlusion", &fsdk::SmileEstimation::occlusion)
		.def("__repr__",
			 [](const fsdk::SmileEstimation &s) {
				 return "<example.SmileEstimation: "
						", mouth = " + std::to_string(s.mouth)
						+ ", smile = " + std::to_string(s.smile)
						+ ", occlusion = " + std::to_string(s.occlusion) +  "'>";
			 })
				;
	f.def("loadImage", &loadImage);
//	EyeEstimation
	py::class_<fsdk::EyesEstimation>(f, "EyesEstimation")
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
				 return "<example.EmotionsEstimation: "
						" anger = " + std::to_string(e.anger)
						+ ", disgust = " + std::to_string(e.disgust)
						+ ", fear = " + std::to_string(e.fear)
						+ ", happiness = " + std::to_string(e.happiness)
						+ ", sadness = " + std::to_string(e.sadness)
						+ ", surprise = " + std::to_string(e.surprise)
						+ ", neutral = " + std::to_string(e.neutral) + "'>";
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
		.def_readwrite("leftEye", &fsdk::GazeEstimation::leftEye)
		.def_readwrite("rightEye", &fsdk::GazeEstimation::rightEye)
		.def("__repr__",
			 [](const fsdk::GazeEstimation &g) {
				 return "<example.GazeEstimation: "
						", leftEye: yaw = " + std::to_string(g.leftEye.yaw) +
						", pitch = " + std::to_string(g.leftEye.pitch) +
						", rightEye: yaw = " + std::to_string(g.rightEye.yaw) +
						", pitch = " + std::to_string(g.rightEye.pitch)  +  "'>";
			 })
				;

	py::class_<fsdk::GazeEstimation::EyeAngles>(f, "EyeAngles")
		.def_readwrite("yaw", &fsdk::GazeEstimation::EyeAngles::yaw)
		.def_readwrite("pitch", &fsdk::GazeEstimation::EyeAngles::pitch)
		.def("__repr__",
			 [](const fsdk::GazeEstimation::EyeAngles &e) {
				 return "<example.EyeAngles: "
						", yaw = " + std::to_string(e.yaw)
						+ ", pitch = " + std::to_string(e.pitch) +  "'>";
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
			 return "<example.Transformation: "
					" angleDeg= " + std::to_string(t.angleDeg)
					+ ", scale = " + std::to_string(t.scale)
					+ ", centerP: x = " + std::to_string(t.centerP.x) + " y = " + std::to_string(t.centerP.y)
					+ ", detectionTopLeft: x = " + std::to_string(t.detectionTopLeft.x)
					+ " y = " + std::to_string(t.detectionTopLeft.y) + "'>";
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
			fsdk::Result<fsdk::Image::Error> error = image.load(path);
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
				 return "<example.Detection: rect: x = " + std::to_string(d.rect.x) +
				 ", y = " + std::to_string(d.rect.y) +
				 ", width = " + std::to_string(d.rect.width) +
				 ", height = " + std::to_string(d.rect.height) +
						"; score = " + std::to_string(d.score) +
						"; isValid = " + std::to_string(d.isValid()) + ">";
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
				 return "<example.Rect: x = " + std::to_string(r.x) +
				 ", y = " + std::to_string(r.y) +
				 ", width = " + std::to_string(r.width) +
				 ", height = " + std::to_string(r.height) + ">";
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



//int main(int argc, char *argv[])
//{
//    // Facial feature detection confidence threshold.
//    const float confidenceThreshold = 0.25f;
//
//    // Parse command line arguments.
//    // Arguments:
//    // 1) path to a first image.
//    // Image should be in ppm format.
//    if (argc != 2) {
//        std::cout << "USAGE: " << argv[0] << " <image>\n"
//            " *image - path to image\n"f
//                  << std::endl;
//        return -1;
//    }
//    char *imagePath = argv[1];
//
//    // Create FaceEngine root SDK object.
//    fsdk::IFaceEnginePtr faceEngine = fsdk::acquire(fsdk::createFaceEngine("./data", "./data/faceengine.conf"));
//    if (!faceEngine) {
//        std::cerr << "Failed to create face engine instance." << std::endl;
//        return -1;
//    }
//
//    // Create MTCNN detector.
//    fsdk::IDetectorPtr faceDetector = fsdk::acquire(faceEngine->createDetector(fsdk::ODT_MTCNN));
//    if (!faceDetector) {
//        std::cerr << "Failed to create face detector instance." << std::endl;
//        return -1;
//    }
//
//    // Create warper.
//    fsdk::IWarperPtr warper = fsdk::acquire(faceEngine->createWarper());
//    if (!warper) {
//        std::cerr << "Failed to create face warper instance." << std::endl;
//        return -1;
//    }
//
//    // Create attribute estimator.
//    fsdk::IAttributeEstimatorPtr attributeEstimator = fsdk::acquire(faceEngine->createAttributeEstimator());
//    if (!attributeEstimator) {
//        std::cerr << "Failed to create attribute estimator instance." << std::endl;
//        return -1;
//    }
//
//    // Create quality estimator.
//    fsdk::IQualityEstimatorPtr qualityEstimator = fsdk::acquire(faceEngine->createQualityEstimator());
//    if (!qualityEstimator) {
//        std::cerr << "Failed to create quality estimator instance." << std::endl;
//        return -1;
//    }
//
//    // Create eye estimator.
//    fsdk::IEyeEstimatorPtr eyeEstimator = fsdk::acquire(faceEngine->createEyeEstimator());
//    if (!eyeEstimator) {
//        std::cerr << "Failed to create eye estimator instance." << std::endl;
//        return -1;
//    }
//
//    // Create head pose estimator.
//    fsdk::IHeadPoseEstimatorPtr headPoseEstimator = fsdk::acquire(faceEngine->createHeadPoseEstimator());
//    if (!headPoseEstimator) {
//        std::cerr << "Failed to create head pose estimator instance." << std::endl;
//        return -1;
//    }
//
//    // Create smile estimator.
//    fsdk::ISmileEstimatorPtr smileEstimator = fsdk::acquire(faceEngine->createSmileEstimator());
//    if (!smileEstimator) {
//        std::cerr << "Failed to create smile estimator instance." << std::endl;
//        return -1;
//    }
//
//    // Create smile estimator.
//    fsdk::IEmotionsEstimatorPtr emotionsEstimator = fsdk::acquire(faceEngine->createEmotionsEstimator());
//    if (!emotionsEstimator) {
//        std::cerr << "Failed to create emotions estimator instance." << std::endl;
//        return -1;
//    }
//
//    // Create ethnicity estimator.
//    fsdk::IEthnicityEstimatorPtr ethnicityEstimator = fsdk::acquire(faceEngine->createEthnicityEstimator());
//    if (!ethnicityEstimator) {
//        std::cerr << "Failed to create ethnicity estimator instance." << std::endl;
//        return -1;
//    }
//
//    // Load image.
//    fsdk::Image image;
//    if (!image.load(imagePath, fsdk::Format::R8G8B8)) {
//        std::cerr << "Failed to load image: \"" << imagePath << "\"" << std::endl;
//        return -1;
//    }
//
//    std::clog << "Detecting faces." << std::endl;
//
//    // Detect no more than 10 faces in the image.
//    enum { MaxDetections = 10 };
//
//    // Data used for detection.
//    fsdk::Detection detections[MaxDetections];
//    int detectionsCount(MaxDetections);
//    fsdk::Landmarks5 landmarks5[MaxDetections];
//    fsdk::Landmarks68 landmarks68[MaxDetections];
//
//    // Detect faces in the image.
//    fsdk::ResultValue<fsdk::FSDKError, int> detectorResult = faceDetector->detect(
//        image,
//        image.getRect(),
//        &detections[0],
//        &landmarks5[0],
//        &landmarks68[0],
//        detectionsCount
//    );
//    if (detectorResult.isError()) {
//        std::cerr << "Failed to detect face detection. Reason: " << detectorResult.what() << std::endl;
//        return -1;
//    }
//    detectionsCount = detectorResult.getValue();
//    if (detectionsCount == 0) {
//        std::clog << "Faces is not found." << std::endl;
//        return 0;
//    }
//    std::clog << "Found " << detectionsCount << " face(s)." << std::endl;
//
//    // Loop through all the faces.
//    for (int detectionIndex = 0; detectionIndex < detectionsCount; ++detectionIndex) {
//        std::cout << "Detection " << detectionIndex + 1 <<
//                  "\nRect: x=" << detections[detectionIndex].rect.x <<
//                  " y=" << detections[detectionIndex].rect.y <<
//                  " w=" << detections[detectionIndex].rect.width <<
//                  " h=" << detections[detectionIndex].rect.height << std::endl;
//
//        // Estimate confidence score of face detection.
//        if (detections[detectionIndex].score < confidenceThreshold) {
//            std::clog << "Face detection succeeded, but confidence score of detection is small." << std::endl;
//            continue;
//        }
//
//        // Get warped face from detection.
//        fsdk::Transformation transformation;
//        fsdk::Landmarks5 transformedLandmarks5;
//        fsdk::Landmarks68 transformedLandmarks68;
//        fsdk::Image warp;
//        transformation = warper->createTransformation(detections[detectionIndex], landmarks5[detectionIndex]);
//        fsdk::Result<fsdk::FSDKError> transformedLandmarks5Result = warper->warp(
//            landmarks5[detectionIndex],
//            transformation,
//            transformedLandmarks5
//        );
//        if (transformedLandmarks5Result.isError()) {
//            std::cerr << "Failed to create transformed landmarks5. Reason: " <<
//                      transformedLandmarks5Result.what() << std::endl;
//            return -1;
//        }
//        fsdk::Result<fsdk::FSDKError> transformedLandmarks68Result = warper->warp(
//            landmarks68[detectionIndex],
//            transformation,
//            transformedLandmarks68
//        );
//        if (transformedLandmarks68Result.isError()) {
//            std::cerr << "Failed to create transformed landmarks68. Reason: " <<
//                      transformedLandmarks68Result.what() << std::endl;
//            return -1;
//        }
//        fsdk::Result<fsdk::FSDKError> warperResult = warper->warp(image, transformation, warp);
//        if (warperResult.isError()) {
//            std::cerr << "Failed to create warped face. Reason: " << warperResult.what() << std::endl;
//            return -1;
//        }
//
//        // Save warped face.
//        warp.save(("warp_" + std::to_string(detectionIndex) + ".ppm").c_str());
//
//        //Estimate ethnicity.
//        fsdk::EthnicityEstimation ethnicityEstimation;
//        fsdk::Result<fsdk::FSDKError> ethnicityEstimationResult = ethnicityEstimator->estimate(warp, ethnicityEstimation);
//        if(ethnicityEstimationResult.isError()) {
//            std::cerr << "Failed to create ethnicity estimation. Reason: " << ethnicityEstimationResult.what() << std::endl;
//            return -1;
//        }
//
//        std::cout << "\nEthnicity estimation:" <<
//                  "\nAfricanAmerican: " << ethnicityEstimation.africanAmerican <<
//                  "\nAsian: " << ethnicityEstimation.asian <<
//                  "\nCaucasian: " << ethnicityEstimation.caucasian <<
//                  "\nIndian: " << ethnicityEstimation.indian << std::endl;
//
//        //Get emotions estimate.
//        fsdk::EmotionsEstimation emotionsEstimation;
//        fsdk::Result<fsdk::FSDKError> emotionsEstimationResult = emotionsEstimator->estimate(warp,emotionsEstimation);
//        if(emotionsEstimationResult.isError()) {
//            std::cerr << "Failed to create emotion estimation. Reason: " << emotionsEstimationResult.what() << std::endl;
//            return -1;
//        }
//
//        std::cout << "\nEmotions estimation:" <<
//                  "\nAnger: " << emotionsEstimation.anger <<
//                  "\nDisgust: " << emotionsEstimation.disgust <<
//                  "\nFear: " << emotionsEstimation.fear <<
//                  "\nHappiness: " << emotionsEstimation.happiness <<
//                  "\nSadness: " << emotionsEstimation.sadness <<
//                  "\nSuprise: " << emotionsEstimation.surprise <<
//                  "\nNeutral: " << emotionsEstimation.neutral << std::endl;
//
//        //Get smile estimate.
//        fsdk::SmileEstimation smileEstimation;
//        fsdk::Result<fsdk::FSDKError> smileEstimatorResult = smileEstimator->estimate(warp, smileEstimation);
//        if(smileEstimatorResult.isError()) {
//            std::cerr << "Failed to create smile estimation. Reason: " << smileEstimatorResult.what() << std::endl;
//            return -1;
//        }
//        std::cout << "\nSmile estimation:" <<
//                  "\nSmile: " << smileEstimation.smile <<
//                  "\nMouth: " << smileEstimation.mouth <<
//                  "\nOcclusion: " << smileEstimation.occlusion << std::endl;
//
//        // Get attribute estimate.
//        fsdk::AttributeEstimation attributeEstimation;
//        fsdk::Result<fsdk::FSDKError> attributeEstimatorResult = attributeEstimator->estimate(warp, attributeEstimation);
//        if(attributeEstimatorResult.isError()) {
//            std::cerr << "Failed to create attribute estimation. Reason: " << attributeEstimatorResult.what() << std::endl;
//            return -1;
//        }
//        std::cout << "\nAttribure estimate:" <<
//                  "\ngender: " << attributeEstimation.gender << " (1 - man, 0 - woman)"
//                      "\nglasses: " << attributeEstimation.glasses <<
//                  " (1 - person wears glasses, 0 - person doesn't wear glasses)" <<
//                  "\nage: " << attributeEstimation.age << " (in years)" << std::endl;
//
//        // Get quality estimate.
//        fsdk::Quality qualityEstimation;
//        fsdk::Result<fsdk::FSDKError> qualityEstimationResult = qualityEstimator->estimate(warp, qualityEstimation);
//        if(qualityEstimationResult.isError()) {
//            std::cerr << "Failed to create quality estimation. Reason: " << qualityEstimationResult.what() << std::endl;
//            return -1;
//        }
//        std::cout << "Quality estimate:" <<
//                  "\nlight: " << qualityEstimation.light <<
//                  "\ndark: " << qualityEstimation.dark <<
//                  "\ngray: " << qualityEstimation.gray <<
//                  "\nblur: " << qualityEstimation.blur <<
//                  "\nquality: " << qualityEstimation.getQuality() << std::endl;
//
//        // Get eye estimate.
//        fsdk::EyesEstimation eyesEstimation;
//        fsdk::Result<fsdk::FSDKError> eyeEstimationResult = eyeEstimator->estimate(
//            warp,
//            transformedLandmarks68,
//            eyesEstimation
//        );
//        if(eyeEstimationResult.isError()) {
//            std::cerr << "Failed to create eye estimation. Reason: " << eyeEstimationResult.what() << std::endl;
//            return -1;
//        }
//        std::cout << "Eye estimate:" <<
//                  "\nleft eye state: " << static_cast<int>(eyesEstimation.leftEye.state) << " (0 - close, 1 - open, 2 - noteye)" <<
//                  "\nright eye state: " << static_cast<int>(eyesEstimation.rightEye.state) << " (0 - close, 1 - open, 2 - noteye)" <<
//                  std::endl;
//        std::cout << std::endl;
//
//        // Get head pose estimate.
//        fsdk::HeadPoseEstimation headPoseEstimation;
//        fsdk::Result<fsdk::FSDKError> headPoseEstimationResult = headPoseEstimator->estimate(
//            landmarks68[detectionIndex],
//            headPoseEstimation
//        );
//        if(headPoseEstimationResult.isError()) {
//            std::cerr << "Failed to create head pose estimation. Reason: " << headPoseEstimationResult.what() << std::endl;
//            return -1;
//        }
//        std::cout << "Head pose estimate:" <<
//                  "\npitch angle estimation: " << headPoseEstimation.pitch <<
//                  "\nyaw angle estimation: " << headPoseEstimation.yaw <<
//                  "\nroll angle estimation: " << headPoseEstimation.roll <<
//                  std::endl;
//        std::cout << std::endl;
//    }
//
//    return 0;
//}

