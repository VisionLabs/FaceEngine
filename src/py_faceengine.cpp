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
#include "helpers.hpp"


namespace py = pybind11;

void estimators_module(py::module& f);
void index_module(py::module& f);

PyIFaceEngine createPyFaceEnginePtr(const char* dataPath = nullptr, const char* configPath = nullptr) {
	return PyIFaceEngine(dataPath, configPath);
}


PyISettingsProvider createSettingsProviderPtr(const char* path) {
	return PyISettingsProvider(path);
}


PYBIND11_MAKE_OPAQUE(fsdk::Landmarks5);
PYBIND11_MAKE_OPAQUE(fsdk::Landmarks68);

PYBIND11_MODULE(FaceEngine, f) {
	
	estimators_module(f);
	index_module(f);
	
	enum class FaceEngineEdition {
		FrontEndEdition,
		CompleteEdition
	};
	
	py::enum_<fsdk::FaceEngineEdition>(f, "FaceEngineEdition", "Complete or FrontEdition version.\n")
		.value("FrontEndEdition", fsdk::FaceEngineEdition::FrontEndEdition)
		.value("CompleteEdition", fsdk::FaceEngineEdition::CompleteEdition)
		.export_values();
			;
	
	f.def("createFaceEngine", &createPyFaceEnginePtr, py::return_value_policy::take_ownership,
		 "Create FaceEngine", py::arg("dataPath") = nullptr, py::arg("configPath") = nullptr,
		 "Create the LUNA SDK root object\n"
		 "\tArgs:\n"
		 "\t\tparam1 (str): [optional] path to folder with FSDK data. Default: ./data (on windows), /opt/visionlabs/data (on linux)\n"
		 "\t\tparam2 (str): [optional] path to faceengine.conf file. Default: <dataPath>/faceengine.cong\n");
	
	f.def("createSettingsProvider", &createSettingsProviderPtr, py::return_value_policy::take_ownership,
		 "Create object SettingsProvider\n"
		 "\tArgs:\n"
		 "\t\tparam1 (str): configuration file path\n");
	
	py::class_<PyIFaceEngine>(f, "PyIFaceEngine", "Root LUNA SDK object interface\n")
		.def("getFaceEngineEdition", &PyIFaceEngine::getFaceEngineEdition,
			"Get LUNA SDK ditribution edition.\n"
			"\tReturns:\n"
			"\t\t(enum FaceEngineEdition) Edition of LUNA SDK instance. Posible values:\n"
			"\t\t\tCompleteEdition - full edition with all function.\n"
			"\t\t\tFrontEndEdition - short edition with excluded descriptor functions.\n"
			"\t\t\tExtractor, matcher, descriptor, batch and index cannot be created.")
		.def("createAttributeEstimator", &PyIFaceEngine::createAttributeEstimator, "Creates Attribute estimator\n")
		.def("createQualityEstimator", &PyIFaceEngine::createQualityEstimator, "Creates Quality estimator\n")
		.def("createEthnicityEstimator", &PyIFaceEngine::createEthnicityEstimator, "Creates Ethnicity estimator\n")
		
		.def("createHeadPoseEstimator", &PyIFaceEngine::createHeadPoseEstimator, "Creates Head pose estimator\n")
		.def("createBlackWhiteEstimator", &PyIFaceEngine::createBlackWhiteEstimator, "Creates Black/White estimator\n")
		.def("createDepthEstimator", &PyIFaceEngine::createDepthEstimator, "Creates Liveness Depth estimator\n")
		.def("createIREstimator", &PyIFaceEngine::createIREstimator, "Creates Liveness Infrared estimator\n")
		.def("createSmileEstimator", &PyIFaceEngine::createSmileEstimator, "Creates Smile estimator\n")
		.def("createFaceFlowEstimator", &PyIFaceEngine::createFaceFlowEstimator, "Creates Liveness flow estimator. \n"
			"Note: this estimator is required only for liveness detection purposes.\n")
		.def("createEyeEstimator", &PyIFaceEngine::createEyeEstimator, "Creates Eye estimator\n")
		.def("createEmotionsEstimator", &PyIFaceEngine::createEmotionsEstimator, "Creates Emotions estimator\n")
		.def("createGazeEstimator", &PyIFaceEngine::createGazeEstimator, "Creates Gaze estimator\n")

		.def("createAGSEstimator", &PyIFaceEngine::createAGSEstimator, "Creates AGS estimator\n")

		.def("createDetector", &PyIFaceEngine::createDetector,
			"Creates a detector of given type.\n"
			"\tArgs:\n"
			"\t\tparam1 (enum ObjectDetectorClassType): Object detector type enumeration.\n")
		.def("createWarper", &PyIFaceEngine::createWarper, "Creates warper\n")
		.def("createDescriptor", &PyIFaceEngine::createDescriptor, "Creates Descriptor\n")
		.def("createDescriptorBatch", &PyIFaceEngine::createDescriptorBatch, py::arg("size"), py::arg("version") = 0,
			"Creates Batch of descriptors\n"
			"\tArgs:\n"
			"\t\tparam1 (int): amount of descriptors in batch\n"
			"\t\tparam2 (str): descriptor version in batch. If 0 - use default version from config\n")
		
		.def("createExtractor", &PyIFaceEngine::createExtractor, "Creates descriptor extractor\n")
		.def("createMatcher", &PyIFaceEngine::createMatcher, "Creates descriptor matcher\n")
		
		// Index
		.def("createIndexBuilder", &PyIFaceEngine::createIndexBuilder, "Creates index builder.\n")
		
		.def("loadDenseIndex", [](
			PyIFaceEngine& faceEngine,
			const char* indexPath) {
				fsdk::ResultValue<fsdk::FSDKError, fsdk::IDenseIndex*> res = faceEngine.loadDenseIndex(indexPath);
				if (res.isOk())
					return std::make_tuple(FSDKErrorResult(fsdk::makeResult(res.getError())), fsdk::acquire(res.getValue()));
				else
					return std::make_tuple(FSDKErrorResult(fsdk::makeResult(res.getError())), fsdk::IDenseIndexPtr());
			},
			"Loads dense index.\n"
			"\t\t Only indexes saved as dense are to be loaded as dense.\n"
			"\tArgs:\n"
			"\t\t param1 (str): indexPath Path to index to be loaded.\n"
			"\tReturns:\n"
			"\t\t(tuple with FSDKErrorResult and dense index): error code FSDKErrorResult and output dense index.\n")
		
		.def("loadDynamicIndex", [](
			PyIFaceEngine& faceEngine,
			const char* indexPath) {
				fsdk::ResultValue<fsdk::FSDKError, fsdk::IDynamicIndex*> res = faceEngine.loadDynamicIndex(indexPath);
				if (res.isOk())
					return std::make_tuple(FSDKErrorResult(fsdk::makeResult(res.getError())), fsdk::acquire(res.getValue()));
				else
					return std::make_tuple(FSDKErrorResult(fsdk::makeResult(res.getError())), fsdk::IDynamicIndexPtr());
			},
			"Loads dynamic index.\n"
			"\t\t Only indexes saved as dynamic are to be loaded as dynamic.\n"
			"\tArgs:\n"
			"\t\t param1 (str): indexPath Path to index to be loaded.\n"
			"\tReturns:\n"
			"\t\t(tuple with FSDKErrorResult and dense index): error code FSDKErrorResult and output dynamic index.\n")
		
		.def("setSettingsProvider", &PyIFaceEngine::setSettingsProvider,
			"Sets settings provider\n"
			"\tArgs:\n"
			"\t\tparam1 (PyISettingsProvider): setting provider\n")
				; // FaceEngine
		
		// ISettingsProvider
		py::class_<PyISettingsProvider>(f, "PyISettingsProvider")
		.def("getDefaultPath", &PyISettingsProvider::getDefaultPath,
			"Get settings path this provider is bound to.\n"
			"\tThis is the same path that was given to load().\n"
			"\tReturns path string.\n")
		
		.def("load", &PyISettingsProvider::load,
			"Load settings from given path.\n"
			"if `path` is null, load from the default path; @see getDefaultPath().\n"
			"\tReturns result with error code specified by ISettingsProvider\n")
		
		.def("save", &PyISettingsProvider::save,
			"Save settings values using the default path.\n"
			"Path may be null, in this case a path from getDefaultPath() will be used.\n"
			"\tReturns true if succeded, false otherwise.\n")
		
		.def("clear", &PyISettingsProvider::clear, "Clear settings.\n")
		
		.def("isEmpty", &PyISettingsProvider::isEmpty,
			"Check if there are loaded settings.\n"
			"Returns true if provider is empty.\n")
		
		.def("setValue", &PyISettingsProvider::setValue, "Set parameter value.\n"
		"Lookup parameter by key. Creates a parameter if it does not already exist.\n"
		"Sets settings provider\n"
		"\tArgs:\n"
		"\t\tparam1 (str): parameter section\n"
		"\t\tparam2 (str): parameter name\n"
		"\t\tparam3 (value): parameter value\n")
		
		.def("getValue", &PyISettingsProvider::getValue,
			"Get parameter value. Lookup parameter by key. Return empty value if the parameters does not exist.\n"
			"\tArgs:\n"
			"\t\tparam1 (str): parameter section\n"
			"\t\tparam2 (str): parameter name\n")
			; // PyISettingsProvider
	
	py::class_<fsdk::ISettingsProvider::Value>(f, "SettingsProviderValue", "Configuration parameter value.\n")
		.def(py::init<>(), "Initialize an empty value. Value type will be set to `Undefined`")
		.def(py::init<int>(), "Initialize an integer value")
		.def(py::init<int, int>(), "Initialize a 2d integer value")
		.def(py::init<int, int, int>(), "Initialize a 3d integer value")
		.def(py::init<int, int, int, int>(), "Initialize a 4d integer value")
		.def(py::init<float>(), "Initialize a float value")
		.def(py::init<float, float>(), "Initialize a 2d float value")
		.def(py::init<float, float, float>(), "Initialize a 3d float value")
		.def(py::init<float, float, float, float>(), "Initialize a 4d float value")
		.def(py::init<const char*>(), "Initialize a string value. Note: only short strings (<15 chars) are supported.\n")
		.def(py::init<const fsdk::Rect&>(), "Initialize a rect value")
		.def(py::init<bool>(), "Initialize a bool value")
		
		.def("asFloat", &fsdk::ISettingsProvider::Value::asFloat, py::arg("defaultValue") = 0.f,
			"Safely get a float. If actual value type is float, the value is returned; "
			"if not a fallback value is returned"
			"\tArgs:\n"
			"\t\tparam1 (float): [optional] fallback value\n")
		
		.def("asBool", &fsdk::ISettingsProvider::Value::asBool, py::arg("defaultValue") = false,
			"Safely get a boolean. If actual value type is convertible to bool, "
			"the value is returned; if not a fallback value is returned.\n"
			"\tArgs:\n"
			"\t\tparam1 (bool): [optional] fallback value\n")
		
		.def("asInt", &fsdk::ISettingsProvider::Value::asInt, py::arg("defaultValue") = 0,
			"Safely get an integer. If actual value type is Int, the value is returned; "
			"if not a fallback value is returned.\n"
			"\tArgs:\n"
			"\t\tparam1 (int): [optional] fallback value\n")
		
		.def("asString", &fsdk::ISettingsProvider::Value::asString, py::arg("defaultValue") = "",
			"Safely get a string. If actual value type is String, the value is returned; "
			"if not a fallback value is returned. Note: doesn't allocate or copy memory.\n"
			"\tArgs:\n"
			"\t\tparam1 (int): [optional] fallback value\n")
		
		.def("asRect", []( const fsdk::ISettingsProvider::Value& v) {
			return v.asRect(); })
		.def("asRect", [](
			const fsdk::ISettingsProvider::Value& v, const fsdk::Rect& r) {
				return v.asRect(r); },
			"Safely get a Rect. If actual value type is convertible to Rect, the value is returned; "
			"if not a fallback value is returned\n"
			"\tArgs:\n"
			"\t\tparam1 (rect): [optional] fallback value\n")
		
		.def("asPoint2f", [](
			const fsdk::ISettingsProvider::Value& v) {
			return v.asPoint2f(); })
		.def("asPoint2f", [](
			const fsdk::ISettingsProvider::Value& v, const fsdk::Point2f& r) {
				return v.asPoint2f(r); },
			"Safely get a Point2f. If actual value type is convertible to Point2f, the value is returned; \n"
			"\tif not a fallback value is returned.\n"
			"\tArgs:\n"
			"\t\tparam1 (Vector2f): [optional] fallback value\n")
		
		.def("asPoint2i", [](
			const fsdk::ISettingsProvider::Value& v) {
			return v.asPoint2i(); })
		.def("asPoint2i", [](
			const fsdk::ISettingsProvider::Value& v, const fsdk::Point2i& r) {
				return v.asPoint2i(r); },
			"Safely get a Point2i. If actual value type is convertible to Point2i, the value is returned; \n"
			"\tif not a fallback value is returned.\n"
			"\tArgs:\n"
			"\t\tparam1 (Vector2i): [optional] fallback value\n")
		
		.def("asSize", [](
			const fsdk::ISettingsProvider::Value& v) {
			return v.asSize(); })
		.def("asSize", [](
			const fsdk::ISettingsProvider::Value& v, const fsdk::Size& s) {
			
				return v.asSize(s); },
			"Safely get a Size. If actual value type is convertible to Size, the value is returned;\n"
			"\tif not a fallback value is returned.\n"
			"\tArgs:\n"
			"\t\tparam1 (Vector2i): [optional] fallback value\n")
			; // SettingsProviderValue
	
	py::class_<fsdk::IFaceEnginePtr>(f, "IFaceEnginePtr");
	
	py::class_<fsdk::IDetectorPtr>(f, "IDetectorPtr", "Face detector interface")
	
		.def("detect",[](
			const fsdk::IDetectorPtr& det,
			const fsdk::Image& image,
			const fsdk::Rect& rect,
			uint32_t maxCount) {
			   std::vector<fsdk::Detection> detections(maxCount);
				std::vector<fsdk::Landmarks5> landmarks(maxCount);
				std::vector<fsdk::Landmarks68> landmarks68(maxCount);
				fsdk::ResultValue<fsdk::FSDKError, int> err = det->detect(
				image,
				rect,
				detections.data(),
				landmarks.data(),
				landmarks68.data(),
				maxCount);
				
				if (err.isOk()) {
					auto detectionResultPyList = py::list(err.getValue());
					for (uint32_t i = 0; i < (uint32_t)err.getValue(); ++i) {
						detectionResultPyList[i] = std::make_tuple(detections[i], landmarks[i], landmarks68[i]);
					}
					return std::make_tuple(FSDKErrorValueInt(err), detectionResultPyList);
				} else {
					return std::make_tuple(FSDKErrorValueInt(err), py::list());
				}},
			"Detect faces and landmarks on the image\n"
			"\tArgs:\n"
			"\t\tparam1 (Image): input image. Format must be R8G8B8\n"
			"\t\tparam2 (Rect): rect of interest inside of the image\n"
			"\t\tparam3 (int): length of `detections` and `landmarks` arrays\n"
			"\tReturns:\n"
			"\t\t(tuple with FSDKErrorValueInt code and list of tuples): \n"
			"\t\t\ttuple with FSDKErrorValueInt code and list of tuples from\n"
			"\t\t\tDetection, Landmarks5, Landmarks68see FSDKErrorValueInt (see FSDKErrorValueInt)\n")
		
		.def("detect_light",[](
			const fsdk::IDetectorPtr& det,
			const fsdk::Image& image,
			const fsdk::Rect& rect,
			int maxCount) {
				std::vector<fsdk::Detection> detections(maxCount);
				fsdk::ResultValue<fsdk::FSDKError, int> err = det->detect(
				image,
				rect,
				detections.data() ,
				maxCount);
				
					if (err.isOk()) {
						uint32_t size = err.getValue();
						detections.resize(size);
						return std::make_tuple(FSDKErrorValueInt(err), std::move(detections));
					} else {
						return std::make_tuple(FSDKErrorValueInt(err), std::vector<fsdk::Detection>()); }},
			"Detect faces and landmarks on the image\n"
			"\tArgs:\n"
			"\t\tparam1 (Image): input image. Format must be R8G8B8\n"
			"\t\tparam2 (Rect): rect of interest inside of the image\n"
			"\t\tparam3 (int): length of `detections` array\n"
			"\tReturns:\n"
			"\t\t(tuple with FSDKErrorValueInt code and list of Detections): \n"
			"\t\t\ttuple with FSDKErrorValueInt code and list of Detections (see FSDKErrorValueInt)\n")
		
		.def("detect5",[](
			const fsdk::IDetectorPtr& det,
			const fsdk::Image& image,
			const fsdk::Rect& rect,
			int maxCount) {
				std::vector<fsdk::Detection> detections(maxCount);
				std::vector<fsdk::Landmarks5> landmarks(maxCount);
				fsdk::ResultValue<fsdk::FSDKError, int> err = det->detect(
				image,
				rect,
				detections.data(),
				landmarks.data(),
				maxCount);
				auto detectionResultPyList = py::list();
			
			if (err.isOk()) {
				auto detectionResultPyList = py::list(err.getValue());
				for (uint32_t i = 0; i < (uint32_t)err.getValue(); ++i)
					detectionResultPyList[i] = std::make_tuple(detections[i], landmarks[i]);
				
				return std::make_tuple(FSDKErrorValueInt(err), detectionResultPyList);
			} else
				return std::make_tuple(FSDKErrorValueInt(err), py::list()); },
			
			"Detect faces and landmarks on the image\n"
			"\tArgs:\n"
			"\t\tparam1 (Image): input image. Format must be R8G8B8\n"
			"\t\tparam2 (Rect): rect of interest inside of the image\n"
			"\t\tparam3 (int): length of `detections` and `landmarks` arrays\n"
			"\tReturns:\n"
			"\t\t(tuple with FSDKErrorValueInt code and list of tuples): \n"
			"\t\t\t tuple with FSDKErrorValueInt code and list of tuples from Detection, Landmarks5 (see FSDKErrorValueInt)\n")
			;
	
	py::class_<fsdk::IWarperPtr>(f, "IWarperPtr",
		"Face detection area warper interface.\n"
		"\tPerform affine transformations on an image to properly align depicted face for descriptor extraction.\n")
	
		.def("warp",[](
			const fsdk::IWarperPtr& warper,
			const fsdk::Image& image,
			const fsdk::Transformation& transformation) {
				fsdk::Image transformedImage;
				fsdk::Result<fsdk::FSDKError> error = warper->warp(image, transformation, transformedImage);
				if (error.isOk())
					return std::make_tuple(FSDKErrorResult(error), transformedImage);
				else
					return std::make_tuple(FSDKErrorResult(error), fsdk::Image()); },
			"Warp image\n"
			"\tArgs:\n"
			"\t\tparam1 (Image): input image. Format must be R8G8B8\n"
			"\t\tparam2 (Transformation): transformation data\n"
			"\tReturns:\n"
			"\t\t(tuple): tuple with FSDKErrorResult and output transformed image\n")
		
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
					return std::make_tuple(FSDKErrorResult(error), transformedLandmarks5);
				else
					return std::make_tuple(FSDKErrorResult(error), fsdk::Landmarks5()); },
			"Warp landmarks of size 5\n"
			"\tArgs:\n"
			"\t\tparam1 (Landmarks5): landmarks array of size 5\n"
			"\t\tparam2 (Transformation): transformation data\n"
			"\tReturns:\n"
			"\t\t(tuple): tuple with FSDKErrorResult and transformed landmarks5\n")
		
		.def("warp",[](
			const fsdk::IWarperPtr& warper,
			const fsdk::Landmarks68& landmarks68,
			const fsdk::Transformation& transformation) {
				fsdk::Landmarks68 transformedLandmarks68;
				fsdk::Result<fsdk::FSDKError> error = warper->warp(landmarks68, transformation, transformedLandmarks68);
				if (error.isOk())
					return std::make_tuple(FSDKErrorResult(error), transformedLandmarks68);
				else
					return std::make_tuple(FSDKErrorResult(error), fsdk::Landmarks68()); },
			"Warp landmarks of size 68\n"
			"\tArgs:\n"
			"\t\tparam1 (Landmarks68): landmarks array of size 68\n"
			"\t\tparam2 (Transformation): transformation data\n"
			"\tReturns:\n"
			"\t\t(tuple): tuple with FSDKErrorResult and transformed landmarks68\n")
		
		.def("createTransformation",[](
			const fsdk::IWarperPtr& warper,
			const fsdk::Detection& detection,
			const fsdk::Landmarks5& landmarks) {
				return warper->createTransformation(detection, landmarks); },
			"Create transformation data struct.\n"
			"\tArgs:\n"
			"\t\tparam1 (Detection): detection rect where landmarks are.\n"
			"\t\tparam2 (Landmarks5): landmarks to calculate warping angles\n"
			"\tReturns:\n"
			"\t\t(Transformation): Transformation obj,\n")
			;
	
	// descriptor
	py::class_<fsdk::IDescriptorPtr>(f, "IDescriptorPtr", "Descriptor interface. Used for matching.\n")
	
		.def("getModelVersion",[]( const fsdk::IDescriptorPtr& desc) {
				return desc->getModelVersion(); },
			"Get algorithm model version this descriptor was created with.\n"
			"\tReturns:\n"
			"\t\t(int): Version as integral number.\n")
		
		.def("getDescriptorLength",[]( const fsdk::IDescriptorPtr& desc) {
				return desc->getDescriptorLength(); },
			"Return size of descriptor in bytes.\n"
			"This method is thread safe.\n"
			"\tReturns:\n"
			"\t\t(int): size of descriptor in bytes.\n")
		
		.def("getDescriptor",[]( const fsdk::IDescriptorPtr& desc) {
				const int size = desc->getDescriptorLength();
				std::vector<uint8_t>buffer(size, 0);
				bool allocated = desc->getDescriptor(&buffer.front());
				auto l = py::list(size);
				int i = 0;
				for (auto it = buffer.rbegin(); it != buffer.rend(); ++it) {
					l[i] = *it;
					++i;
				}
				if (allocated)
					return l;
				else
					return py::list(); },
			"Copy descriptor data to python list.\n "
			"\tThis method is thread safe"
			"\tReturns:\n"
			"\t\t(list): list of uint8_t if is ok (length of list is 264), empty list if ERROR")
		.def("load",[](
			const fsdk::IDescriptorPtr& descriptor,
			const char* buffer,
			uint32_t bufferSize) {
				const char* data = reinterpret_cast<const char*>(buffer);
				VectorArchive archiveDescriptor(data, bufferSize);
				fsdk::Result<fsdk::ISerializableObject::Error> err = descriptor->load(&archiveDescriptor);
				return SerializeErrorResult(err);
			}, "Load descriptor from buffer")
				; // descriptor
	
	py::enum_<fsdk::ISerializableObject::Error>(f, "SerializeError", "Serialization error codes.\n")
		.value("Ok", fsdk::ISerializableObject::Error::Ok)
		.value("Size", fsdk::ISerializableObject::Error::Size)
		.value("Signature", fsdk::ISerializableObject::Error::Signature)
		.value("ArchiveRead", fsdk::ISerializableObject::Error::ArchiveRead)
		.value("InputArchive", fsdk::ISerializableObject::Error::InputArchive)
		.value("ArchiveWrite", fsdk::ISerializableObject::Error::ArchiveWrite)
			; // error

//	Errors
	py::class_<SerializeErrorResult>(f, "SerializeErrorResult",
		"Wrapper for ISerializableObject::Error that encapsulates an action result enumeration.\n"
		"\tAn enum should specify a result code.\n")
		.def_readonly("isOk", &SerializeErrorResult::isOk)
		.def_readonly("isError", &SerializeErrorResult::isError)
		.def_readonly("serializeError", &SerializeErrorResult::serializeError)
		.def_readonly("what", &SerializeErrorResult::what)
		.def("__repr__",
			[](const SerializeErrorResult &err) {
				return "SerializeErrorResult: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", serializeError = " +
						fsdk::ErrorTraits<fsdk::ISerializableObject::Error>::toString(err.serializeError)
						+ ", what = " + err.what; })
			;

// DescriptorBatch
	py::class_<fsdk::IDescriptorBatchPtr>(f, "IDescriptorBatchPtr", "Descriptor batch interface. "
	"Used for matching large continuous sets of descriptors")
	
		.def("add",[] (
			const fsdk::IDescriptorBatchPtr& descriptorBatchPtr,
			const fsdk::IDescriptorPtr& desc) {
				fsdk::Result<fsdk::IDescriptorBatch::Error> error = descriptorBatchPtr->add(desc);
				return DescriptorBatchResult(error);
			},
			"Add a descriptor to the batch.\n"
			"\tWhen adding the first descriptor to an empty batch, the initial internal parameters (like version,\n"
			"\tetc) of the batch are set to correspond ones of the descriptor. This means that incompatible descriptors\n"
			"\tadded later will be rejected. See getModelVersion() to check whether a descriptor can be added to the batch.\n"
			"\tArgs:\n"
			"\t\tparam1 (IDescriptorPtr): descriptor to add. Descriptor data is copied and to internal reference is held,\n"
			"\t\tthus it is safe to release the source descriptor object later.\n"
			"\tReturns:\n"
			"\t\t(DescriptorBatchResult): One of the error codes specified by DescriptorBatchError.\n")
		
		.def("removeFast",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr, const int index) {
				fsdk::Result<fsdk::IDescriptorBatch::Error> error = descriptorBatchPtr->removeFast(index);
				return DescriptorBatchResult(error); },
			"Remove a descriptor from batch. \nRemove descriptor by swapping it with the last descriptor in batch.\n"
			"\tThis preserves descriptor order.\n"
			"\tArgs:\n"
			"\t\tparam1 (int): descriptor index\n"
			"\tReturns:\n"
			"\t\t(DescriptorBatchResult): One of the error codes specified by DescriptorBatchError.\n")
		
		.def("removeSlow",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr, int index) {
				fsdk::Result<fsdk::IDescriptorBatch::Error> error = descriptorBatchPtr->removeSlow(index);
				return  DescriptorBatchResult(error);},
			"Remove a descriptor from batch.\n"
			"\tRemove descriptor by shifting all the following descriptors back. This preserves descriptor order.\n"
			"\tArgs:\n"
			"\t\tparam1 (int): descriptor index\n"
			"\tReturns:\n"
			"\t\t(DescriptorBatchResult): One of the error codes specified by DescriptorBatchError.\n" )
		
		.def("getMaxCount",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr) {
				return descriptorBatchPtr->getMaxCount(); },
			"Get maximum number of descriptors in this batch.\n"
			"\tReturns:\n"
			"\t\t(int): maximum number of descriptors in this batch.\n")
		
		.def("getCount",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr) {
				return descriptorBatchPtr->getCount(); },
			"Get actual number of descriptors in this batch.\n"
			"\tReturns:\n"
			"\t\t(int):actual number of descriptors in this batch.\n")
		
		.def("getModelVersion",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr) {
				return descriptorBatchPtr->getModelVersion(); },
			"Get algorithm model version the descriptors in this batch were created with.\n"
			"This function only makes sense when there is at least one descriptor in the batch. It will return 0 if\n"
			"the batch is empty.\n"
			"\tReturns:\n"
			"\t\t(int): Version as integral number.\n")
		
		.def("getDescriptorSize",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr) {
				return descriptorBatchPtr->getDescriptorSize(); },
			"Get length of one descriptor. Specified by version of descriptors in batch.\n"
			"\tReturns:\n"
			"\t\t(int): Length of one descriptor in batch.\n")
		
		.def("getDescriptorSlow",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr, int index) {
				return fsdk::acquire(descriptorBatchPtr->getDescriptorSlow(index)); },
			"Create descriptor from batch by index with copying\n"
			"\tArgs:\n"
			"\t\tparam1 (int):  index required descriptor in batch\n"
			"\tReturns:\n"
			"\t\t(IDescriptorPtr): valid object if succeeded.\n")
		
		.def("getDescriptorFast",[]( const fsdk::IDescriptorBatchPtr& descriptorBatchPtr, int index) {
				if (index < 0 || index >= int(descriptorBatchPtr->getCount())) throw py::index_error();
				return fsdk::acquire(descriptorBatchPtr->getDescriptorFast(index)); },
			"Create descriptor from batch by index without copying\n"
			"\tArgs:\n"
			"\t\tparam1 (int):  index required descriptor in batch\n"
			"\tReturns:\n"
			"\t\t(IDescriptorPtr): valid object if succeeded.\n")
		
		.def("load",[](
			const fsdk::IDescriptorBatchPtr& descriptorBatchPtr,
			const char* buffer,
			uint32_t bufferSize) {
				VectorArchive archiveDescriptor(buffer, bufferSize);
				fsdk::Result<fsdk::ISerializableObject::Error> err =
					descriptorBatchPtr->load(&archiveDescriptor, bufferSize);
				return SerializeErrorResult(err);
			
			}, "Load descriptor from buffer")
				;
	
	py::enum_<fsdk::IDescriptorBatch::Error>(f, "DescriptorBatchError",
		"Descriptor batch error enumeration.\n"
		"\tUsed for descriptor batch related errors indication.\n")
		.value("Ok", fsdk::IDescriptorBatch::Error::Ok)
		.value("InvalidInput", fsdk::IDescriptorBatch::Error::InvalidInput)
		.value("BatchFull", fsdk::IDescriptorBatch::Error::BatchFull)
		.value("Incompatible", fsdk::IDescriptorBatch::Error::Incompatible)
		.value("Internal", fsdk::IDescriptorBatch::Error::Internal)
		.value("IoError", fsdk::IDescriptorBatch::Error::IoError)
		.value("OutOfRange", fsdk::IDescriptorBatch::Error::OutOfRange)
			;
	
	py::class_<fsdk::IDescriptorExtractorPtr>(f, "IDescriptorExtractorPtr",
		"Descriptor extractor interface.\n"
		"\tExtracts face descriptors from images. The descriptors can be later used for face matching.\n")
	
		.def("extract",[](
			const fsdk::IDescriptorExtractorPtr& extractor,
			fsdk::Image& image,
			const fsdk::Detection& detection,
			const fsdk::Landmarks5& landmarks,
			const fsdk::IDescriptorPtr& descriptor) {
				fsdk::ResultValue<fsdk::FSDKError, float> err = extractor->extract(image, detection,
																					landmarks, descriptor);
				return FSDKErrorValueFloat(err); },
			"Extract a face descriptor from an image.\n"
			"\tThis method accepts arbitrary images that have size at least 250x250 pixels and R8G8B8 pixel format.\n"
			"\tThe input image is warped internally using an assigned warper (@see IWarper). The descriptor extractor is\n"
			"\tcreated with a proper warped assigned by default so no additional setup is required, unless this behaviour\n"
			"\tis overriden with `defaultWarper` flag upon extractor creation.\n"
			"\tArgs:\n"
			"\t\tparam1 (Image):  source image. Format must be R8G8B8\n"
			"\t\tparam2 (Detection): face detection\n"
			"\t\tparam3 (Landmarks5): face feature set\n"
			"\t\tparam4 (IDescriptorPtr) [out]: descriptor to fill with data.\n"
			"\tReturns:\n"
			"\t\t(FSDKErrorValueFloat): ResultValue with error code specified by FSDKError and score of descriptor normalized in range [0, 1]\n"
			"\t\t\t1 - face on the input warp; 0 - garbage on the input detection.\n"
			"\t\t\tScore is fake if extractor uses mobile net version of extraction model.\n"
			"\t\tSee FSDKErrorValueFloat.\n")
		
		.def("extractFromWarpedImage",[](
			const fsdk::IDescriptorExtractorPtr& extractor,
			const fsdk::Image& image,
			const fsdk::IDescriptorPtr& descriptor) {
				fsdk::ResultValue<fsdk::FSDKError, float> err = extractor->extractFromWarpedImage(image, descriptor);
				return FSDKErrorValueFloat(err); },
			"Extract descriptor from a warped image.\n"
			"\tThe input image should be warped; see IWarper.\n"
			"\tArgs:\n"
			"\t\tparam1 (Image): image source warped image, should be a valid 250x250 image in R8G8B8 format.\n"
			"\t\tparam2 (IDescriptorPtr) [out]: descriptor to fill with data\n"
			"\tReturns:\n"
			"\t\t(FSDKErrorValueFloat): ResultValue with error code specified by FSDKError and score of descriptor normalized in range [0, 1]\n"
			"\t\t\t1 - face on the input warp; 0 - garbage on the input detection.\n"
			"\t\t\tScore is fake if extractor uses mobile net version of extraction model.\n"
			"\t\tSee FSDKErrorValueFloat.\n")
		
		.def("extractFromWarpedImageBatch",[](
			const fsdk::IDescriptorExtractorPtr& extractor,
			std::vector<fsdk::Image> warpsBatch,
			const fsdk::IDescriptorBatchPtr& descriptorBatch,
			const fsdk::IDescriptorPtr& aggregation,
			uint32_t batchSize) {
				std::vector<float> garbageScoreBatch(batchSize);
				fsdk::Result<fsdk::FSDKError> err = extractor->extractFromWarpedImageBatch(
				warpsBatch.data(),
				descriptorBatch,
				aggregation,
				garbageScoreBatch.data(),
				batchSize);
				auto garbagePyList = py::list();
				if (err.isOk()) {
					for (uint32_t i = 0; i < batchSize; ++i) {
						garbagePyList.append(garbageScoreBatch[i]);
					}
					return garbagePyList;
				}
				else {
					garbagePyList.append(FSDKErrorResult(err));
					return garbagePyList;
				} },
			"Extract batch of descriptors from a batch of images and perform aggregation.\n"
			"\tThe input images should be warped; see IWarper.\n"
			"\tArgs:\n"
			"\t\tparam1 (list):  input array of warped images, images should be in R8G8B8 format, "
			"with size 250x250\n"
			"\t\tparam2 (IDescriptorBatchPtr) [out]:  descriptorBatch descriptor batch to fill with data.\n"
			"\t\tparam3 (IDescriptorPtr) [out]: descriptor with aggregation based on descriptor batch.\n"
			"\t\tparam4 (int): size of the batch.\n"
			"\tReturns:\n"
			"\t\t(list): if OK - list of descriptor scores normalized in range [0, 1]\n"
			"\t\t\t1 - face on the input warp; 0 - garbage on the input warp.\n"
			"\t\t\tScore is fake if extractor uses mobile net version of extraction model.\n"
			"\t\t(FSDKErrorResult): else - result with error code specified by FSDKError\n See FSDKErrorResult.\n")
		
		.def("extractFromWarpedImageBatch",[](
			const fsdk::IDescriptorExtractorPtr& extractor,
			py::list warpsBatchList,
			const fsdk::IDescriptorBatchPtr& descriptorBatch,
			uint32_t batchSize) {
				std::vector<float> garbageScoreBatch(batchSize);
				std::vector<fsdk::Image> warpsBatch(batchSize);
				for (uint32_t i = 0; i < batchSize; ++i) {
					warpsBatch[i] = warpsBatchList[i].cast<fsdk::Image>();
				}
				fsdk::Result<fsdk::FSDKError> err = extractor->extractFromWarpedImageBatch(
				warpsBatch.data(),
				descriptorBatch,
				garbageScoreBatch.data(),
				batchSize);
				py::list garbagePyList;
				if (err.isOk()) {
					for (uint32_t i = 0; i < batchSize; ++i) {
						garbagePyList.append(garbageScoreBatch[i]);
					}
					return garbagePyList;
				}
				else {
					garbagePyList.append(FSDKErrorResult(err));
					return garbagePyList; } },
			"Extract batch of descriptors from a batch of images.\n"
			"\tThe input images should be warped; see IWarperPtr.\n"
			"\tArgs:\n"
			"\t\tparam1 (list): input list of warped images.Images should be in R8G8B8 format , with size 250x250\n"
			"\t\tparam2 (IDescriptorBatchPtr) [out]: \n"
			"\t\tparam3 (int): size of the batch\n"
			"\tReturns:\n"
			"\t\t(list): if OK - list of descriptor scores normalized in range [0, 1]\n"
			"\t\t\t1 - face on the input warp; 0 - garbage on the input warp.\n"
			"\t\t\t Score is fake if extractor uses mobile net version of extraction model.\n"
			"\t\t(FSDKErrorResult): else - result with error code specified by FSDKError\n See FSDKErrorResult.\n")
			;
	
	py::class_<fsdk::IDescriptorMatcherPtr>(f, "IDescriptorMatcherPtr",
		"Descriptor matcher interface.\n"
		"\tMatches descriptors 1:1 and 1:M (@see IDescriptor and IDescriptorBatch interfaces).\n"
		"\tAs a result of the matching process the calling site gets a MatchingResult "
		"(or several of them in case of 1:M\n"
		"\tmatching). The MatchingResult structure contains distance and similarity metrics.\n"
		"\t\n"
		"\tDistance is measured in abstract units and tends to 0 for similar descriptors "
		"and to infinity for different ones.\n"
		"\tSimilarity is the opposite metric and shows probability of two "
		"descriptors belonging to the same person; therfore\n"
		"\tit is normalized to [0..1] range")
		
		.def("match",[](
			const fsdk::IDescriptorMatcherPtr& matcherPtr,
			const fsdk::IDescriptorPtr& first,
			const fsdk::IDescriptorPtr& second) {
				fsdk::ResultValue<fsdk::FSDKError, fsdk::MatchingResult> err =
				matcherPtr->match(first, second);
				return FSDKErrorValueMatching(err); },
			"Match descriptors 1:1.\n"
			"\tArgs\n"
			"\t\tparam1 (IDescriptorPtr): first descriptor\n"
			"\t\tparam2 (IDescriptorPtr): second descriptor\n"
			"\tReturns:\n"
			"\t\t(FSDKErrorValueMatching): Value with error code specified by FSDKError and matching result. "
			"See FSDKErrorValueMatching.\n")
		
		.def("match",[](
			const fsdk::IDescriptorMatcherPtr& matcherPtr,
			const fsdk::IDescriptorPtr& reference,
			const fsdk::IDescriptorBatchPtr& candidates) {
				std::vector<fsdk::MatchingResult> results(candidates->getCount());
				fsdk::Result<fsdk::FSDKError> err =
				matcherPtr->match(reference, candidates, results.data());
				auto resultsPyList = py::list();
				if (err.isOk()) {
					for (const auto& it: results) {
						resultsPyList.append(it);
					}
					return resultsPyList;
				}
				else {
					resultsPyList.append(FSDKErrorResult(err));
					return resultsPyList; } },
			"Match descriptors 1:M.\n"
			"\tMatches a reference descriptor to a batch of candidate descriptors. "
			"The results are layed out in the\n"
			"\tsame order as the candidate descriptors in the batch.\n"
			"\tArgs\n"
			"\t\tparam1 (IDescriptorPtr): the reference descriptor\n"
			"\t\tparam2 (IDescriptorPtr): the candidate descriptor batch to match with the reference\n"
			"\tReturns:\n"
			"\t\t(list): if OK - matchig result list.\n"
			"\t\t\tLength of `results` must be at least the same as the length of the candidates batch.\n"
			"\t\t\tIDescriptorBatchPtr::getMaxCount()\n"
			"\t\t(FSDKErrorResult wrapped in list): else - result with error specified by FSDKErrorResult.\n")
			;
	
	py::class_<fsdk::Landmarks5>(f, "Landmarks5",
		"Face landmarks, length is fixed and equal to5.\n"
		"\tMTCNN face detector is capable of face landmarks detection. "
		"\tLandmarks are special classes binded to python. \n"
		"\tThey are similar on python lists. It is possible to use some standard python built-in "
		"functions for them: \n"
		"\t`__len__`, `__getitem__`. The method `__setitem__` is used only for test and research "
		"purposes with class Vector2f. \n"
		"\tMore detailed description see in FaceEngineSDK_Handbook.pdf or source C++ interface.\n")
	
		.def(py::init<>())
		.def("__len__", [](const fsdk::Landmarks5 &v) { return v.landmarkCount; },
			"Called to implement the built-in function len(). Should return the length of the object, an integer >= 0.\n"
			"\tExample: len(landmarks)")
		.def("__getitem__", [](const fsdk::Landmarks5 &s, size_t i) {
				if (i >= s.landmarkCount) throw py::index_error();
				return s.landmarks[i]; },
			"Called to implement evaluation of self[key]. The accepted keys should be integers.\n "
			"\tExample: lanmarks[3]")
		
		.def("__setitem__", [](fsdk::Landmarks5 &s, size_t i, fsdk::Vector2<float> v) {
				if (i >= s.landmarkCount) throw py::index_error();
				s.landmarks[i].x = v.x;
				s.landmarks[i].y = v.y;
				return s.landmarks[i];
			},
			"Called to implement assignment to self[key]. \n"
			"\tThe method `__setitem__` is used only for test and research purposes with class Vector2f.\n "
			"\tExample: lanmarks[i] = FaceEngine.Vector2f(10.0, 20.0)")
			; //Landmarks5
	
	py::class_<fsdk::Landmarks68>(f, "Landmarks68",
		"Face landmarks, length is fixed and equal to 68.\n"
		"\tMTCNN face detector is capable of face landmarks detection. "
		"\tLandmarks are special classes binded to python. \n"
		"\tThey are similar on python lists. It is possible to use some standard python "
		"built-in functions for them: \n"
		"\t`__len__`, `__getitem__`. The method `__setitem__` is used only for test and "
		"research purposes with class Vector2f. \n"
		"\tMore detailed description see in FaceEngineSDK_Handbook.pdf or source C++ interface.\n")
	
		.def(py::init<>())
		.def("__len__", [](const fsdk::Landmarks68 &v) { return v.landmarkCount; },
			"Called to implement the built-in function len(). Should return the length of the object, an integer >= 0.\n"
			"\tExample: len(landmarks)")
		
		.def("__getitem__", [](const fsdk::Landmarks68 &s, size_t i) {
				if (i >= s.landmarkCount) throw py::index_error();
				return s.landmarks[i]; },
			"Called to implement evaluation of self[key]. The accepted keys should be integers.\n "
			"\tExample:lanmarks[3]")
		
		.def("__setitem__", [](fsdk::Landmarks68 &s, size_t i, fsdk::Vector2<float> v) {
			if (i >= s.landmarkCount) throw py::index_error();
			s.landmarks[i].x = v.x;
			s.landmarks[i].y = v.y;
			return s.landmarks[i];
		}, "Called to implement assignment to self[key]. \n"
			"\tThe method `__setitem__` is used only for test and research purposes with class Vector2f.\n "
			"\tExample: lanmarks[i] = FaceEngine.Vector2f(10.0, 20.0)")
		; //Landmarks68

// doc Template
//		"Te.\n"
//		"\tTe.\n"
//		"\tArgs\n"
//		"\t\tparam1 (t): te\n"
//		"\t\tparam2 (t): te\n"
//		"\tReturns:\n"
//		"\t\t(t): te"
// Vector2
	
	py::class_<fsdk::Vector2<float>>(f, "Vector2f", "Vector with 2 float values, x and y")
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
	
	py::class_<fsdk::Vector2<int>>(f, "Vector2i", "Vector with 2 integer values, x and y")
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
	py::class_<FSDKErrorResult>(f, "FSDKErrorResult",
		"Wrapper for FSDK::Error that encapsulates an action result enumeration.\n"
		"\tAn enum should specify a result code.\n")
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
	
	py::class_<DescriptorBatchResult>(f, "DescriptorBatchResult",
		"Wrapper for DescriptorBatch::Error that encapsulates an action result enumeration.\n"
		"\tAn enum should specify a result code.\n")
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
	
	py::class_<ImageErrorResult>(f, "ImageErrorResult",
		"Wrapper for Image::Error that encapsulates an action result enumeration.\n"
		"\tAn enum should specify a result code.\n")
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
	
	py::class_<SettingsProviderResult>(f, "SettingsProviderErrorResult",
		"Wrapper for SettingsProvider::Error that encapsulates an action result enumeration.\n"
		"\ttAn enum should specify a result code.\n")
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
	
	py::class_<FSDKErrorValueInt>(f, "FSDKErrorValueInt", "Wrapper for result to output some integer "
		"value aside the result.\n")
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
	
	py::class_<FSDKErrorValueFloat>(f, "FSDKErrorValueFloat",
		"Wrapper for result to output some "
		"float value aside the result.\n")
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
	
	py::class_<FSDKErrorValueMatching>(f, "FSDKErrorValueMatching", "Wrapper for result to output "
		"some Matching value (distance, similarity) aside the result.\n")
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
						+ ", value: (distance = " + std::to_string(err.value.distance) +
						", similarity = " + std::to_string(err.value.similarity) + ")"
						+ ", what = " + err.what;
			})
		;

//	Transformation
	py::class_<fsdk::Transformation>(f, "Transformation",
		"Transformation data structure, used for warping.\n"
		"\tUse this structure to perform image and landmarks transformations.\n"
		"\tIf no circumstances should you create OR edit this structure by yourself")
		.def(py::init<>())
		.def_readwrite("angleDeg", &fsdk::Transformation::angleDeg)
		.def_readwrite("scale", &fsdk::Transformation::scale)
		.def_readwrite("centerP", &fsdk::Transformation::centerP)
		.def_readwrite("detectionTopLeft", &fsdk::Transformation::detectionTopLeft)
		.def("__repr__", [](const fsdk::Transformation &t) {
			return "Transformation: "
				  " angleDeg = " + std::to_string(t.angleDeg)
				  + ", scale = " + std::to_string(t.scale)
				  + ", centerP: x = " + std::to_string(t.centerP.x) + " y = " + std::to_string(t.centerP.y)
				  + ", detectionTopLeft: x = " + std::to_string(t.detectionTopLeft.x)
				  + " y = " + std::to_string(t.detectionTopLeft.y);
		})
			;
// Image type and format
	py::enum_<fsdk::Format::Type>(f, "FormatType", "Format type enumeration.\n")
		.value("Unknown", fsdk::Format::Unknown)
		.value("B8G8R8X8", fsdk::Format::B8G8R8X8)
		.value("R8G8B8X8", fsdk::Format::R8G8B8X8)
		.value("B8G8R8", fsdk::Format::B8G8R8)
		.value("R8G8B8", fsdk::Format::R8G8B8)
		.value("R8", fsdk::Format::R8)
	//		R16 is used for internal transformations for depth map and cannot be saved or downloaded
		.value("R16", fsdk::Format::R16,
			  "\tUsed for internal transformations for depth map and "
			  "cannot be saved or downloaded")
		;
	
	py::class_<fsdk::Image>(f, "Image",
		"Image objects\n"
		"More detailed description see in FaceEngineSDK_Handbook.pdf or source C++ interface.\n")
		.def(py::init<>())
		.def("getWidth", &fsdk::Image::getWidth)
		.def("getHeight", &fsdk::Image::getHeight)
		.def("isValid", &fsdk::Image::isValid)
		.def("getFormat", [](const fsdk::Image& image){
			fsdk::Format::Type type = fsdk::Format::Type(image.getFormat());
			return type;
		})
		
		.def("getRect", &fsdk::Image::getRect,
			"Image rectangle.\n"
			"\tResulting rectangle top left corner is always at (0, 0).\n")
		
		.def("getData", [](const fsdk::Image& image) {
			fsdk::Format type = fsdk::Format::Type(image.getFormat());
			int c = getChannelCount(type);
			const auto* const data_uint = image.getDataAs<uint8_t>();
			std::vector<uint8_t> data(data_uint, data_uint + image.getDataSize());
			std::vector<ssize_t> shape { image.getHeight(), image.getWidth(), c };
			auto ptr = data.data();
			return py::array(shape, ptr);
		}, "\tReturns image as numpy array.\n")
		
		.def("getChannelCount", [](const fsdk::Image& image) {
			fsdk::Format type = fsdk::Format::Type(image.getFormat());
			return getChannelCount(type);
		}, "\tReturns channel count.\n")
		.def("getChannelStep", [](const fsdk::Image& image) {
			return image.getFormat().getChannelStep();
		}, "\tGet channel step.Padding bytes are considered spare channels.\n")
		
		.def("getChannelSize", [](const fsdk::Image& image) {
			return image.getFormat().getChannelSize();
		})
		
		.def("getBitDepth", [](const fsdk::Image& image) {
			return image.getFormat().getBitDepth();
		})
		
		.def("getByteDepth", [](const fsdk::Image& image) {
			return image.getFormat().getByteDepth();
		})
		
		.def("computePitch", [](const fsdk::Image& image, int rowWidth) {
			return image.getFormat().computePitch(rowWidth);
		})
		
		.def("isPadded", [](const fsdk::Image& image) {
			return image.getFormat().isPadded();
		})
		
		.def("isBGR", [](const fsdk::Image& image) {
			return image.getFormat().isBGR();
		})
		
		.def("isValidFormat", [](const fsdk::Image& image) {
			return image.getFormat().isValid();
		}, "\tReturns true if image format is one of valid types, i.e. not Unknown.\n")
		
		.def("setData", [](fsdk::Image& image, py::array npImage) {
			auto size = npImage.shape();
			fsdk::Format type;
			if (size[2] == 3)
				type = fsdk::Format::R8G8B8;
			else if (size[2] == 1)
				type = fsdk::Format::R8;
			else
				throw py::cast_error("\nUnsupported types of image! Convert it to R8G8B8 or R8, or "
									"point exact format as second parameter, example: "
									"image.setData(numpy_array, FaceEngine.FormatType.R8G8B8X8)");
			image.set(size[1], size[0], type, npImage.data());
		}, "Set image by numpy array. Convert it to R8G8B8 or R8.\n")
		
		.def("setData", [](fsdk::Image& image, py::array npImage, fsdk::Format::Type type) {
			auto size = npImage.shape();
			image.set(size[1], size[0], fsdk::Format(type), npImage.data());
		}, "\n\tSet image by numpy array. Please point format. example: \n"
			"\t\timage.setData(numpy_array, FaceEngine.FormatType.R8G8B8X8)")
		
		.def("save", [](const fsdk::Image& image, const char* path) {
			fsdk::Result<fsdk::Image::Error> error = image.save(path);
			return ImageErrorResult(error);
		})
		
		.def("save", [](const fsdk::Image& image,
						const char* path,
						const fsdk::Format::Type type) {
			fsdk::Result<fsdk::Image::Error> error = image.save(path, fsdk::Format(type));
			return ImageErrorResult(error);
		})
		.def("load",[](fsdk::Image& image, const char* path) {
			fsdk::Result<fsdk::Image::Error> error = image.load(path);
			return ImageErrorResult(error);
		})
		
		.def("load", [](
			fsdk::Image& image,
			const char* path,
			const fsdk::Format::Type type) {
				fsdk::Result<fsdk::Image::Error> error = image.load(path, fsdk::Format(type));
				return ImageErrorResult(error);
			})
		
		.def("loadFromMemory", [](fsdk::Image& image, const char* bytes, int sizeInBytes) {
			fsdk::Result<fsdk::Image::Error> error = image.loadFromMemory(bytes, sizeInBytes);
			return ImageErrorResult(error);
		})
		
		.def("loadFromMemory", [](
		fsdk::Image& image,
		const char* bytes,
		int sizeInBytes,
		const fsdk::Format::Type type) {
			fsdk::Result<fsdk::Image::Error> error = image.loadFromMemory(
			bytes,
			sizeInBytes,
			fsdk::Format(type));
			return ImageErrorResult(error);
		})
		;
	
	// first of all for test
	f.def("loadImage", &loadImage, "used only for depth test");
	
	py::enum_<fsdk::Image::Type>(f, "ImageType",
		"Supported image types.\n")
		.value("BMP", fsdk::Image::Type::BMP)
		.value("JPG", fsdk::Image::Type::JPG)
		.value("PNG", fsdk::Image::Type::PNG)
		.value("PPM", fsdk::Image::Type::PPM)
		.value("TIFF", fsdk::Image::Type::TIFF)
		.value("Unknown", fsdk::Image::Type::Unknown)
		;
	
	py::enum_<fsdk::Image::Error>(f, "ImageError", "Image error codes.\n")
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
	
	py::enum_<fsdk::ISettingsProvider::Error>(f, "SettingsProviderError",
		"Config parsing error codes.\n")
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
	
	py::class_<fsdk::Detection>(f, "Detection",
		"Face detection.\n"
		"\tStores a detected face bounding box within a source image frame as well as detection confidence score.\n")
		.def(py::init<>())
		.def_readwrite("rect", &fsdk::Detection::rect, "Object bounding box")
		.def_readwrite("score", &fsdk::Detection::score, "Object detection score)")
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
	
	py::class_<fsdk::Rect>(f, "Rect", "Rectangle")
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
	
	py::enum_<fsdk::ObjectDetectorClassType>(f, "ObjectDetectorClassType", py::arithmetic(), "Object detector type enumeration.\n")
		.value("ODT_MTCNN", fsdk::ODT_MTCNN, "MTCNN detector type")
		.value("ODT_MTCNN_MINI", fsdk::ODT_MTCNN_MINI, "ODT_MTCNN_MINI detector type")
		.value("ODT_COUNT", fsdk::ODT_COUNT, "Detector type count")
		.export_values();
			;
	
	py::enum_<fsdk::FSDKError>(f, "FSDKError", "Common SDK error codes.\n")
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
			PyIFaceEngine.createFaceFlowEstimator
			PyIFaceEngine.createEyeEstimator
			PyIFaceEngine.createEmotionsEstimator
			PyIFaceEngine.createGazeEstimator

			PyIFaceEngine.createDetector
			PyIFaceEngine.createWarper
			PyIFaceEngine.createDescriptor
			PyIFaceEngine.createDescriptorBatch
			PyIFaceEngine.createExtractor
			PyIFaceEngine.createMatcher
			PyIFaceEngine.setSettingsProvider
			PyIFaceEngine.createIndexBuilder
			PyIFaceEngine.loadDenseIndex
			PyIFaceEngine.loadDynamicIndex

			SettingsProviderValue
			SettingsProviderValue.__init__

			PyISettingsProvider.getDefaultPath
			PyISettingsProvider.load
			PyISettingsProvider.save
			PyISettingsProvider.clear
			PyISettingsProvider.isEmpty
			PyISettingsProvider.setValue
			PyISettingsProvider.getValue

			IQualityEstimatorPtr
			IQualityEstimatorPtr.estimate

			IAttributeEstimatorPtr
			IAttributeEstimatorPtr.estimate

			IEthnicityEstimator
			IEthnicityEstimator.estimate

			IDetector
			IDetector.detect

			IWarperPtr
			IWarperPtr.warp
			IWarperPtr.createTransformation

			IDescriptorPtr
			IDescriptorPtr.getModelVersion
			IDescriptorPtr.getDescriptorLength
			IDescriptorPtr.getDescriptor

			IDescriptorBatchPtr
			IDescriptorBatchPtr.add
			IDescriptorBatchPtr.removeFast
			IDescriptorBatchPtr.removeSlow
			IDescriptorBatchPtr.getMaxCount
			IDescriptorBatchPtr.getCount
			IDescriptorBatchPtr.getModelVersion
			IDescriptorBatchPtr.getDescriptorSize
			IDescriptorBatchPtr.getDescriptorSlow
			IDescriptorBatchPtr.getDescriptorFast

			DescriptorBatchError

			IDescriptorExtractorPtr
			IDescriptorExtractorPtr.extract
			IDescriptorExtractorPtr.extractFromWarpedImage
			IDescriptorExtractorPtr.extractFromWarpedImageBatch


			IDescriptorMatcherPtr
			IDescriptorMatcherPtr.match

			IHeadPoseEstimatorPtr
			IHeadPoseEstimatorPtr.estimate

			IBlackWhiteEstimatorPtr
			IBlackWhiteEstimatorPtr.estimate

			ILivenessDepthEstimatorPtr
			ILivenessDepthEstimatorPtr.estimate
			ILivenessDepthEstimatorPtr.setRange

			ILivenessIREstimatorPtr
			ILivenessIREstimatorPtr.estimate

			ISmileEstimatorPtr
			ISmileEstimatorPtr.estimate

			ILivenessFlowEstimatorPtr
			ILivenessFlowEstimatorPtr.estimate

			IEyeEstimatorPtr
			IEyeEstimatorPtr.estimate

			IEmotionsEstimatorPtr
			IEmotionsEstimatorPtr.estimate

			IGazeEstimatorPtr
			IGazeEstimatorPtr.estimate

			IAGSEstimatorPtr
			IAGSEstimatorPtr.estimate

			IIndexPtr
			IIndexPtr.search

			IDenseIndexPtr
			IDenseIndexPtr.search
			IDenseIndexPtr.size
			IDenseIndexPtr.descriptorByIndex
			IDenseIndexPtr.search

			IDynamicIndexPtr
			IDynamicIndexPtr.saveToDenseIndex
			IDynamicIndexPtr.saveToDynamicIndex
			IDynamicIndexPtr.search
			IDynamicIndexPtr.size
			IDynamicIndexPtr.descriptorByIndex
			IDynamicIndexPtr.appendDescriptor
			IDynamicIndexPtr.appendBatch
			IDynamicIndexPtr.removeDescriptor

			MatchingResult

			Landmarks5
			Landmarks5.__len__
			Landmarks5.__getitem__
			Landmarks5.__setitem__

			Landmarks68
			Landmarks68.__len__
			Landmarks68.__getitem__
			Landmarks68.__setitem__

			IrisLandmarks
			IrisLandmarks.__len__
			IrisLandmarks.__getitem__
			IrisLandmarks.__setitem__

			EyelidLandmarks
			EyelidLandmarks.__len__
			EyelidLandmarks.__getitem__
			EyelidLandmarks.__setitem__

			Vector2f
			Vector2f.__init__
			Vector2f.__repr__

			Vector2i
			Vector2i.__init__
			Vector2i.__repr__

			FSDKErrorResult
			DescriptorBatchResult
			ImageErrorResult
			SettingsProviderErrorResult
			FSDKErrorValueInt
			FSDKErrorValueFloat
			FSDKErrorValueMatching


			AttributeEstimation
			AttributeEstimation.__init__
			AttributeEstimation.__repr__
			Quality
			Quality.__init__
			Quality.getQuality

			EthnicityEstimation
			EthnicityEstimation.__init__
			EthnicityEstimation.__repr__
			EthnicityEstimation.getEthnicityScore
			EthnicityEstimation.getPredominantEthnicity

			HeadPoseEstimation
			HeadPoseEstimation.__init__
			HeadPoseEstimation.__repr__
			HeadPoseEstimation.getFrontalFaceType

			SmileEstimation
			SmileEstimation.__init__
			SmileEstimation.__repr__

			EyesEstimation
			EyesEstimation.__init__
			EyesEstimation.__repr__
			State

			EyeAttributes
			EyeAttributes.state
			EyeAttributes.iris
			EyeAttributes.eyelid

			EmotionsEstimation
			EmotionsEstimation.__init__
			EmotionsEstimation.__repr__
			EmotionsEstimation.getPredominantEmotion
			EmotionsEstimation.getEmotionScore

			Emotions

			GazeEstimation
			GazeEstimation.__init__
			GazeEstimation.__repr__
			GazeEstimation

			EyeAngles

			Ethnicity

			Transformation

			Detection

			FormatType

			Image
			Image.__init__
			Image.getWidth
			Image.getHeight
			Image.isValid
			Image.getFormat
			Image.getRect
			Image.getData
			Image.getChannelCount
			Image.getChannelStep
			Image.getBitDepth
			Image.getByteDepth
			Image.computePitch
			Image.isPadded
			Image.isBGR
			Image.isValidFormat
			Image.setData
			Image.save
			Image.load
			Image.getRect
			Image.loadFromMemory

			ImageType

			ImageError

			SettingsProviderError

			Rect

			ObjectDetectorClassType
			ObjectDetectorClassType.ODT_MTCNN
			ObjectDetectorClassType.ODT_COUNT

			FSDKError
			FrontalFaceType

			DepthRange
			DepthRange.__repr__
			loadImage
			EyeAngles

			IIndexPtr
			IIndexPtr.search
			
			IDenseIndexPtr
			IDenseIndexPtr.search
			IDenseIndexPtr.size
			IDenseIndexPtr.descriptorByIndex
			
			IDynamicIndexPtr
			IDynamicIndexPtr.saveToDenseIndex
			IDynamicIndexPtr.saveToDynamicIndex
			IDynamicIndexPtr.countOfIndexedDescriptors
			IDynamicIndexPtr.search
			IDynamicIndexPtr.size
			IDynamicIndexPtr.descriptorByIndex
			IDynamicIndexPtr.search
			IDynamicIndexPtr.appendDescriptor
			IDynamicIndexPtr.appendBatch
			IDynamicIndexPtr.removeDescriptor
			
			IIndexBuilderPtr
			IIndexBuilderPtr.buildIndex
			IIndexBuilderPtr.appendDescriptor
			IIndexBuilderPtr.appendBatch
			IIndexBuilderPtr.appendDescriptor
			IIndexBuilderPtr.removeDescriptor
			IIndexBuilderPtr.descriptorByIndex
			
    )pbdoc";
}


