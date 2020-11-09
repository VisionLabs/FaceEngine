#include <iostream>
#include <pybind11/pybind11.h>
#include <fsdk/FaceEngine.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/operators.h>
#include <pybind11/numpy.h>
#include "ErrorsAdapter.hpp"
#include "FaceEngineAdapter.hpp"
#include "LivenessEngineAdapter.hpp"
#include "SettingsProviderAdapter.hpp"
#include "helpers.hpp"
#include <fsdk/Version.h>
#include <fsdk/Types/HumanLandmarks.h>

namespace py = pybind11;

void estimators_module(py::module& f);
void index_module(py::module& f);
void image_rect_module(py::module &f);
void settings_provider_module(py::module& f);
void detector_module(py::module& f);
void descriptor_module(py::module& f);
void warper_module(py::module& f);
void liveness_module(py::module& f);

PyIFaceEngine createPyFaceEnginePtr(
	const char* dataPath = nullptr,
	const char* configPath = nullptr,
	const char* runtimeConfigPath = nullptr) {
	return PyIFaceEngine(dataPath, configPath, runtimeConfigPath);
}

PyILivenessEngine createPyLivenessEnginePtr(
	const PyIFaceEngine& pyIFaceEngine,
	const char* dataPath = nullptr) {
	return PyILivenessEngine(pyIFaceEngine, dataPath);
}

PyISettingsProvider createSettingsProviderPtr(const char* path) {
	return PyISettingsProvider(path);
}

template<class T>
py::class_<fsdk::Optional<T>> optional_class(py::module& this_module, const char* name)
{
	py::class_<fsdk::Optional<T>>class_instance(this_module, name);
	
	class_instance.def(py::init<>());
	class_instance.def(py::init<T>());
	
	class_instance.def("value", [](const fsdk::Optional<T>& self) {
		return self.value();
	});
	class_instance.def("isValid", [](const fsdk::Optional<T>& self) {
		return self.valid();
	});
	
	class_instance.def("set", [](fsdk::Optional<T>& self, const T& param) {
		self = param;
		return self;
	});
	
	return class_instance;
}

void set_optional_class(py::module& f)
{
	auto optionalLandmarks5 = optional_class<fsdk::Landmarks5>(f, "OptionalLandmarks5");
	auto optionalLandmarks68 = optional_class<fsdk::Landmarks68>(f, "OptionalLandmarks68");
	auto optionalfloat = optional_class<float>(f, "Optionalfloat");
	auto optionalEthnicityEstimation = optional_class<fsdk::EthnicityEstimation>(f, "OptionalEthnicityEstimation");
	auto optionalLandmarks17 = optional_class<fsdk::HumanLandmarks17>(f, "OptionalLandmarks17");
}

PYBIND11_MODULE(FaceEngine, f) {
	
	estimators_module(f);
	index_module(f);
	image_rect_module(f);
	settings_provider_module(f);
	detector_module(f);
	descriptor_module(f);
	warper_module(f);
	liveness_module(f);
	set_optional_class(f);
	
	enum class FaceEngineEdition {
		FrontEndEdition,
		CompleteEdition
	};

	py::enum_<fsdk::SensorType>(f, "SensorType", py::arithmetic(), "Camera sensor type.\n")
		.value("Visible", fsdk::SensorType::Visible, "Visible sensor type (default mode)")
		.value("NIR", fsdk::SensorType::NIR, "NIR sensor type")
		.export_values();
			;

	py::enum_<fsdk::ObjectDetectorClassType>(f, "ObjectDetectorClassType", py::arithmetic(), "Object detector type enumeration.\n")
		.value("FACE_DET_DEFAULT", fsdk::FACE_DET_DEFAULT, "Default detector cpecified in config file")
		.value("FACE_DET_V1", fsdk::FACE_DET_V1, "First detector type")
		.value("FACE_DET_V2", fsdk::FACE_DET_V2, "Light detector type")
		.value("FACE_DET_V3", fsdk::FACE_DET_V3, "Third detector type")
		.export_values();
			;

	py::class_<fsdk::Face>(f, "Face", "Container for detection and landmakrs\n")
		.def(py::init<>())
		.def(py::init<fsdk::Image>())
		.def(py::init<fsdk::Image, fsdk::Detection>())
		.def(py::init<fsdk::Image, fsdk::BaseDetection<float>>())
		.def_readwrite("img", &fsdk::Face::img, "Image\n")
		.def_readwrite("detection", &fsdk::Face::detection, "Detection\n")
		.def_readwrite("landmarks5_opt", &fsdk::Face::landmarks5, "Landmarks5 optinal\n")
		.def_readwrite("landmarks68_opt", &fsdk::Face::landmarks68, "Landmarks68 optinal\n")
		.def("isValid", &fsdk::Face::isValid, "Valid or not\n")
			;
	
	py::enum_<fsdk::FaceEngineEdition>(f, "FaceEngineEdition", "Complete or FrontEdition version.\n")
		.value("FrontEndEdition", fsdk::FaceEngineEdition::FrontEndEdition)
		.value("CompleteEdition", fsdk::FaceEngineEdition::CompleteEdition)
		.export_values();
			;
	
	f.def("getVersionHash", []() -> std::string {
		return std::string("fsdk_hash: ") + fsdk::getVersionHash();
	});
	f.def("getVersionString", []() -> std::string {
		return std::string("fsdk_version: ") + fsdk::getVersionString();
	});
	f.def("getBuildInfo", []() -> std::string {
		return std::string("fsdk_build_info: ") + fsdk::getBuildInfo();
	});
	
	f.def("createFaceEngine", &createPyFaceEnginePtr, py::return_value_policy::take_ownership,
		"Create FaceEngine", py::arg("dataPath") = nullptr, py::arg("configPath") = nullptr, py::arg("runtimeConfigPath") = nullptr,
		"Create the LUNA SDK root object\n"
		"\tArgs:\n"
		"\t\tparam1 (str): [optional] path to folder with FSDK data.\n"
		"\t\tparam2 (str): [optional] path to faceengine.conf file. Default: <dataPath>/faceengine.cong\n"
		"\t\tparam3 (str): [optional] path to runtime.conf file. Default: <dataPath>/runtime.cong\n");

	f.def("createSettingsProvider", &createSettingsProviderPtr, py::return_value_policy::take_ownership,
		"Create object SettingsProvider\n"
		"\tArgs:\n"
		"\t\tparam1 (str): configuration file path\n");
	
	f.def("createLivenessEngine", &createPyLivenessEnginePtr, py::return_value_policy::take_ownership,
		  "Create the Liveness object\n"
		"\tArgs:\n"
		"\t\tparam1 (FaceEngine obj): the LUNA SDK root object.\n"
		"\t\tparam2 (str): [optional] path to folder with FSDK data.\n");
	
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
		
		.def("createHeadPoseEstimator", &PyIFaceEngine::createHeadPoseEstimator, "Creates Head pose estimator\n")
		.def("createBlackWhiteEstimator", &PyIFaceEngine::createBlackWhiteEstimator, "Creates Black/White estimator\n")
		.def("createDepthEstimator", &PyIFaceEngine::createDepthEstimator, "Creates Liveness Depth estimator\n")
		.def("createIREstimator", &PyIFaceEngine::createIREstimator, "Creates Liveness Infrared estimator\n")
		.def("createLivenessFlyingFacesEstimator", &PyIFaceEngine::createLivenessFlyingFacesEstimator, "Creates Liveness Flying Faces estimator\n")
		.def("createFaceFlowEstimator", &PyIFaceEngine::createFaceFlowEstimator, "Creates Liveness flow estimator. \n"
			"Note: this estimator is required only for liveness detection purposes.\n")
		.def("createLivenessRGBMEstimator", &PyIFaceEngine::createLivenessRGBMEstimator, "Creates Liveness RGBM estimator. \n")

		.def("createEyeEstimator", &PyIFaceEngine::createEyeEstimator, 
			"Creates Eye estimator of given recognition mode\n",
			py::arg("mode") = fsdk::SensorType::Visible,
			"\tArgs:\n"
			"\t\tparam2 (enum SensorType): Recognition mode type enumeration.\n"
			"\tReturns:\n"
			"\t\tEye estimator instance.")

		.def("createEmotionsEstimator", &PyIFaceEngine::createEmotionsEstimator, "Creates Emotions estimator\n")
		.def("createGazeEstimator", &PyIFaceEngine::createGazeEstimator, "Creates Gaze estimator\n", py::arg("mode") = fsdk::SensorType::Visible)
		.def("createGlassesEstimator", &PyIFaceEngine::createGlassesEstimator, "Creates Glasses estimator\n")
		.def("createAGSEstimator", &PyIFaceEngine::createAGSEstimator, "Creates AGS estimator\n")
		.def("createMouthEstimator", &PyIFaceEngine::createMouthEstimator, "Creates Mouth estimator\n")
		.def("createMedicalMaskEstimator", &PyIFaceEngine::createMedicalMaskEstimator, "Creates Medical Mask estimator\n")
		.def("createOverlapEstimator", &PyIFaceEngine::createOverlapEstimator, "Creates Overlap estimator\n")
		.def("createOrientationEstimator", &PyIFaceEngine::createOrientationEstimator, "Creates Orientation estimator\n")
		.def("createDetector", &PyIFaceEngine::createDetector,
			"Creates a detector of given type.\n", 
			py::arg("type") = fsdk::ObjectDetectorClassType::FACE_DET_DEFAULT,
			py::arg("mode") = fsdk::SensorType::Visible,
			"\tArgs:\n"
			"\t\tparam1 (enum ObjectDetectorClassType): Object detector type enumeration.\n"
			"\t\tparam2 (enum SensorType): Recognition mode type enumeration.\n"
			"\tReturns:\n"
			"\t\tDetector instance.")
		
		.def("createHumanDetector", &PyIFaceEngine::createHumanDetector,
			"Creates a human detector.\n")
		
		.def("createWarper", &PyIFaceEngine::createWarper, "Creates warper\n")
		.def("createHumanWarper", &PyIFaceEngine::createHumanWarper, "Creates human warper\n")
		.def("createDescriptor", &PyIFaceEngine::createDescriptor, py::arg("version") = 0, "Creates Descriptor (version >= 101 is human descriptor)\n")
		.def("createDescriptorBatch", &PyIFaceEngine::createDescriptorBatch, py::arg("size"), py::arg("version") = 0,
			"Creates Batch of descriptors\n"
			"\tArgs:\n"
			"\t\tparam1 (int): amount of descriptors in batch\n"
			"\t\tparam2 (str): descriptor version in batch. If 0 - use default version from config\n")
		
		.def("createExtractor", &PyIFaceEngine::createExtractor, py::arg("version") = 0, "Creates descriptor extractor (version >= DV_MIN_HUMAN_DESCRIPTOR_VERSION is human descriptor)\n")
		.def("createMatcher", &PyIFaceEngine::createMatcher, py::arg("version") = 0, "Creates descriptor matcher (version >= DV_MIN_HUMAN_DESCRIPTOR_VERSION is human descriptor)\n")
		
		// Index
		.def("createIndexBuilder", &PyIFaceEngine::createIndexBuilder, "Creates index builder.\n")
		
		.def("loadDenseIndex", [](
			PyIFaceEngine& faceEngine,
			const char* indexPath) {
				fsdk::ResultValue<fsdk::FSDKError, fsdk::IDenseIndexPtr> res = faceEngine.loadDenseIndex(indexPath);
				if (res.isOk())
					return std::make_tuple(FSDKErrorResult(fsdk::makeResult(res.getError())), res.getValue());
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
				fsdk::ResultValue<fsdk::FSDKError, fsdk::IDynamicIndexPtr> res = faceEngine.loadDynamicIndex(indexPath);
				if (res.isOk())
					return std::make_tuple(FSDKErrorResult(fsdk::makeResult(res.getError())), res.getValue());
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
			"\t\tparam1 (SettingsProvider): sets setting provider\n")
		
		.def("getSettingsProvider", &PyIFaceEngine::getSettingsProvider,
			"Returns settings provider\n"
			"\tReturns:\n"
			"\t\t(SettingsProvider): settings provider.")
			
		.def("getRuntimeSettingsProvider", &PyIFaceEngine::getRuntimeSettingsProvider,
			"Returns runtime settings provider\n"
			"\tReturns:\n"
			"\t\t(RuntimeSettingsProvider): runtime settings provider.")

		.def("setRuntimeSettingsProvider", &PyIFaceEngine::setRuntimeSettingsProvider,
			"Sets settings provider\n"
			"\tArgs:\n"
			"\t\tparam1 (RuntimeSettingsProvider): sets runtime setting provider\n")

		.def("getLicense", &PyIFaceEngine::getLicense,
			"Get current License object, which was set for FaceEngine object.\n"
			"\tReturns:\n"
			"\t\t(fsdk::ILicensePtr): current license object."
			)

		.def("activateLicense", [](
			PyIFaceEngine& faceEngine,
			const fsdk::ILicensePtr& license,
			const char * licenseConfPath) {
				return FSDKErrorResult(faceEngine.activateLicense(license, licenseConfPath));
			},
			"Makes License activation with some platform specific manner. Network connection is required.\n"
			"\t\tThis method should be usend only on mobile or ARM platforms.\n"
			"\tArgs:\n"
			"\t\t param1 (fsdk::ILicensePtr): license object to activate.\n"
			"\t\t param2 (str): path to the license.conf file.\n"
			"\tReturns:\n"
			"\t\t(FSDKError): @see FSDKError for details.\n"
			)
				; // FaceEngine
	
	
	py::class_<fsdk::IFaceEnginePtr>(f, "IFaceEnginePtr");
	
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
		.def_readonly("error", &SerializeErrorResult::error)
		.def_readonly("what", &SerializeErrorResult::what)
		.def("__repr__",
			[](const SerializeErrorResult &err) {
				return "SerializeErrorResult: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", error = " +
						fsdk::ErrorTraits<fsdk::ISerializableObject::Error>::toString(err.error)
						+ ", what = " + err.what; })
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
	
	py::enum_<fsdk::DesctiptorType>(f, "DesctiptorType", "Determines which type of descriptor to use.\n")
		.value("DT_FACE", fsdk::DesctiptorType::DT_FACE)
		.value("DT_HUMAN", fsdk::DesctiptorType::DT_HUMAN)
		.export_values()
			;
	
	py::enum_<fsdk::DescriptorVersion>(f, "DescriptorVersion", py::arithmetic(),
			"Minimum descriptor model version.\n"
			"\tDetermines which minimum version of descriptor to use.\n")
		.value("DV_MIN_FACE_DESCRIPTOR_VERSION", fsdk::DescriptorVersion::DV_MIN_FACE_DESCRIPTOR_VERSION)
		.value("DV_MIN_HUMAN_DESCRIPTOR_VERSION", fsdk::DescriptorVersion::DV_MIN_HUMAN_DESCRIPTOR_VERSION)
		.export_values()
			;
	
	py::class_<fsdk::Landmarks5>(f, "Landmarks5",
		"Face landmarks, length is fixed and equal to5.\n"
		"\tFace detector is capable of face landmarks detection. "
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
			"\tExample: landmarks[3]")
		
		.def("__setitem__", [](fsdk::Landmarks5 &s, size_t i, fsdk::Vector2<float> v) {
				if (i >= s.landmarkCount) throw py::index_error();
				s.landmarks[i].x = v.x;
				s.landmarks[i].y = v.y;
				return s.landmarks[i];
			},
			"Called to implement assignment to self[key]. \n"
			"\tThe method `__setitem__` is used only for test and research purposes with class Vector2f.\n "
			"\tExample: landmarks[i] = FaceEngine.Vector2f(10.0, 20.0)")
			; //Landmarks5
	
	py::class_<fsdk::Landmarks68>(f, "Landmarks68",
		"Face landmarks, length is fixed and equal to 68.\n"
		"\tFace detector is capable of face landmarks detection. "
		"\tLandmarks are special classes binded to python. \n"
		"\tThey are similar on python lists. It is possible to use some standard python "
		"built-in functions for them: \n"
		"\t`__len__`, `__getitem__`. The method `__setitem__` is used only for test and "
		"\tresearch purposes with class Vector2f. \n"
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

	py::class_<fsdk::HumanLandmarks17>(f, "HumanLandmarks17",
		"HumanLandmarks landmarks, length is fixed and equal to 17.\n"
		"\tHuman detector is capable of face landmarks detection."
		"\tLandmarks are special classes binded to python. \n"
		"\tThey are similar on python lists. It is possible to use some standard python "
		"built-in functions for them: \n"
		"\t`__len__`, `__getitem__`. The method `__setitem__` is used only for test and "
		"\tresearch purposes with class Vector2f. \n"
		"\tMore detailed description see in FaceEngineSDK_Handbook.pdf or source C++ interface.\n")

		.def(py::init<>())

		.def("__len__", [](const fsdk::HumanLandmarks17 &v) { return v.landmarksCount; },
			"Called to implement the built-in function len(). Should return the length of the object, an integer >= 0.\n"
			"\tExample: len(landmarks)")

		.def("__getitem__", [](const fsdk::HumanLandmarks17 &s, size_t i) {
				if (i >= s.landmarksCount) throw py::index_error();
				return s.landmarks[i]; },
			"Called to implement evaluation of self[key]. The accepted keys should be integers.\n "
			"\tExample: landmarks[3]")
			
		.def("__setitem__", [](fsdk::HumanLandmarks17 &s, size_t i, fsdk::HumanLandmark v) {
			if (i >= s.landmarksCount) throw py::index_error();
			s.landmarks[i].point = v.point;
			s.landmarks[i].score = v.score;
			s.landmarks[i].visible = v.visible;
			return s.landmarks[i];
		}, "Called to implement assignment to self[key]. \n"
			"\tThe method `__setitem__` is used only for test and research purposes with class Vector2f.\n "
			"\tExample: landmarks[i] = FaceEngine.Vector2f(10.0, 20.0)")
		; //HumanLandmarks17

	py::class_<fsdk::Human>(f, "Human", "Human detection\n")
		.def(py::init<>())
		.def_readwrite("img", &fsdk::Human::img, "Image\n")
		.def_readwrite("detection", &fsdk::Human::detection, "Object bounding box")
		.def_readwrite("landmarks17_opt", &fsdk::Human::landmarks17, "HumanLandmarks17 optional\n")
		.def("isValid", &fsdk::Human::isValid)
		.def("__repr__",
			[](const fsdk::Human& d) {
				return "Human: rect: x = " + std::to_string(d.detection.rect.x) +
					", y = " + std::to_string(d.detection.rect.y) +
					", width = " + std::to_string(d.detection.rect.width) +
					", height = " + std::to_string(d.detection.rect.height) +
					"; score = " + std::to_string(d.detection.score) +
					"; isValid = " + std::to_string(d.detection.isValid());
			});
			;
	py::class_<fsdk::HumanLandmark>(f, "HumanLandmark", "HumanLandmark keypoints\n")
		.def(py::init<>())
		.def_readwrite("score", &fsdk::HumanLandmark::score)
		.def_readwrite("point", &fsdk::HumanLandmark::point)
		.def_readwrite("visible", &fsdk::HumanLandmark::visible)
		;

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
		.def_readonly("error", &FSDKErrorResult::error)
		.def_readonly("what", &FSDKErrorResult::what)
		.def("__repr__",
			[](const FSDKErrorResult &err) {
				return "FSDKErrorResult: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", FSDKError = " + std::to_string(static_cast<uint32_t>(err.error))
						+ ", what = " + err.what; })
			;
	
	//	Errors
	py::class_<LSDKErrorResult>(f, "LSDKErrorResult",
			"Wrapper for LSDK::Error that encapsulates an action result enumeration.\n"
			"\tAn enum should specify a result code.\n")
		.def_readonly("isOk", &LSDKErrorResult::isOk)
		.def_readonly("isError", &LSDKErrorResult::isError)
		.def_readonly("error", &LSDKErrorResult::error)
		.def_readonly("what", &LSDKErrorResult::what)
		.def("__repr__",
			 [](const LSDKErrorResult &err) {
				 return "LSDKErrorResult: "
							"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", error = " + std::to_string(static_cast<uint32_t>(err.error))
						+ ", what = " + err.what; })
		;
	
	py::class_<DescriptorBatchResult>(f, "DescriptorBatchResult",
		"Wrapper for DescriptorBatch::Error that encapsulates an action result enumeration.\n"
		"\tAn enum should specify a result code.\n")
		.def_readonly("isOk", &DescriptorBatchResult::isOk)
		.def_readonly("isError", &DescriptorBatchResult::isError)
		.def_readonly("error", &DescriptorBatchResult::error)
		.def_readonly("what", &DescriptorBatchResult::what)
		.def("__repr__",
			[](const DescriptorBatchResult &err) {
				return "DescriptorBatchResult: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", error = " + std::to_string(static_cast<uint32_t>(err.error))
						+ ", what = " + err.what; })
			;
	
	py::class_<ImageErrorResult>(f, "ImageErrorResult",
		"Wrapper for Image::Error that encapsulates an action result enumeration.\n"
		"\tAn enum should specify a result code.\n")
		.def_readonly("isOk", &ImageErrorResult::isOk)
		.def_readonly("isError", &ImageErrorResult::isError)
		.def_readonly("error", &ImageErrorResult::error)
		.def_readonly("what", &ImageErrorResult::what)
		.def("__repr__",
			[](const ImageErrorResult &err) {
				return "ImageErrorResult: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", error = " + std::to_string(static_cast<uint32_t>(err.error))
						+ ", what = " + err.what; })
			;
	
	py::class_<SettingsProviderResult>(f, "SettingsProviderErrorResult",
		"Wrapper for SettingsProvider::Error that encapsulates an action result enumeration.\n"
		"\ttAn enum should specify a result code.\n")
		.def_readonly("isOk", &SettingsProviderResult::isOk)
		.def_readonly("isError", &SettingsProviderResult::isError)
		.def_readonly("error", &SettingsProviderResult::error)
		.def_readonly("what", &SettingsProviderResult::what)
		.def("__repr__",
			[](const SettingsProviderResult &err) {
				return "SettingsProviderResult: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", error = " + std::to_string(static_cast<uint32_t>(err.error))
						+ ", what = " + err.what; })
		;
	
	py::class_<FSDKErrorValueInt>(f, "FSDKErrorValueInt", "Wrapper for result to output some integer "
		"value aside the result.\n")
		.def_readonly("isOk", &FSDKErrorValueInt::isOk)
		.def_readonly("isError", &FSDKErrorValueInt::isError)
		.def_readonly("error", &FSDKErrorValueInt::error)
		.def_readonly("what", &FSDKErrorValueInt::what)
		.def_readonly("value", &FSDKErrorValueInt::value)
		.def("__repr__",
			[](const FSDKErrorValueInt &err) {
				return "FSDKErrorValueInt: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", error = " + std::to_string(static_cast<uint32_t>(err.error))
						+ ", value = " + std::to_string(err.value)
						+ ", what = " + err.what; })
			;
	
	py::class_<FSDKErrorValueBool>(f, "FSDKErrorValueBool", "Wrapper for result to output some bool "
		"value aside the result.\n")
		.def_readonly("isOk", &FSDKErrorValueBool::isOk)
		.def_readonly("isError", &FSDKErrorValueBool::isError)
		.def_readonly("error", &FSDKErrorValueBool::error)
		.def_readonly("what", &FSDKErrorValueBool::what)
		.def_readonly("value", &FSDKErrorValueBool::value)
		.def("__repr__",
			 [](const FSDKErrorValueBool &err) {
				 return "FSDKErrorValueBool: "
							"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", error = " + std::to_string(static_cast<uint32_t>(err.error))
						+ ", value = " + std::to_string(err.value)
						+ ", what = " + err.what; })
			;
	
	py::class_<FSDKErrorValueFloat>(f, "FSDKErrorValueFloat",
		"Wrapper for result to output some "
		"float value aside the result.\n")
		.def_readonly("isOk", &FSDKErrorValueFloat::isOk)
		.def_readonly("isError", &FSDKErrorValueFloat::isError)
		.def_readonly("error", &FSDKErrorValueFloat::error)
		.def_readonly("what", &FSDKErrorValueFloat::what)
		.def_readonly("value", &FSDKErrorValueFloat::value)
		.def("__repr__",
			[](const FSDKErrorValueFloat &err) {
				return "FSDKErrorValueFloat: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", error = " + std::to_string(static_cast<uint32_t>(err.error))
						+ ", value = " + std::to_string(err.value)
						+ ", what = " + err.what;
			})
		;
	
	py::class_<FSDKErrorValueMatching>(f, "FSDKErrorValueMatching", "Wrapper for result to output "
		"some Matching value (distance, similarity) aside the result.\n")
		.def_readonly("isOk", &FSDKErrorValueMatching::isOk)
		.def_readonly("isError", &FSDKErrorValueMatching::isError)
		.def_readonly("error", &FSDKErrorValueMatching::error)
		.def_readonly("what", &FSDKErrorValueMatching::what)
		.def_readonly("value", &FSDKErrorValueMatching::value)
		.def("__repr__",
			[](const FSDKErrorValueMatching &err) {
				return "FSDKErrorValueMatching: "
						"isOk = " + std::to_string(err.isOk)
						+ ", isError = " + std::to_string(err.isError)
						+ ", error = " + std::to_string(static_cast<uint32_t>(err.error))
						+ ", value: (distance = " + std::to_string(err.value.distance) +
						", similarity = " + std::to_string(err.value.similarity) + ")"
						+ ", what = " + err.what;
			})
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

	py::enum_<fsdk::DetectionComparerType>(f, "DetectionComparerType", py::arithmetic(),
			"Strategy of BestDetections comparer\n")
		.value("DCT_CONFIDENCE", fsdk::DCT_CONFIDENCE,
			"BestDetection - detections with highest score\n")
		.value("DCT_CENTER", fsdk::DCT_CENTER,
			"BestDetection - most centered detection\n")
		.value("DCT_CENTER_AND_CONFIDENCE", fsdk::DCT_CENTER_AND_CONFIDENCE,
			"BestDetection - most centered with high score\n")
		.value("DCT_SIZE", fsdk::DCT_SIZE,
			"BestDetection - the largest detection\n")
		.export_values();
			;
	
	py::enum_<fsdk::DetectionType>(f, "DetectionType", py::arithmetic(), "Detection type enumeration.\n")
		.value("DT_BBOX", fsdk::DT_BBOX, "Get bounding boxes of faces\n")
		.value("DT_LANDMARKS5", fsdk::DT_LANDMARKS5, "Get 5 facial landmarks\n")
		.value("DT_LANDMAKRS68", fsdk::DT_LANDMAKRS68, "Get 68 facial landmarks\n")
		.value("DT_ALL", fsdk::DT_ALL, "Get all supported parameters.\n")
		.export_values();
			;
	
	py::enum_<fsdk::OrientationType>(f, "OrientationType", py::arithmetic(), "Orientation type enumeration.\n")
		.value("Normal", fsdk::OT_NORMAL, "Normal orientaiton of image\n")
		.value("Left", fsdk::OT_LEFT, "Image is rotated 90 degrees to the left.\n")
		.value("Right", fsdk::OT_RIGHT, "Image is rotated 90 degrees to the right.\n")
		.value("UpsideDown", fsdk::OT_UPSIDE_DOWN, "Image is flipped upside down.\n")
		.export_values();
			;
	
	py::enum_<fsdk::HumanDetectionType>(f, "HumanDetectionType", py::arithmetic(), "Human detection type enumeration.\n")
		.value("DCT_BOX", fsdk::DCT_BOX, "Get bounding boxes of human bodies\n")
		.value("DCT_POINTS", fsdk::DCT_POINTS, "Get 17 keypoints of human, with score for each one\n")
		.value("DCT_ALL", fsdk::DCT_ALL, "Get all supported parameters.\n")
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
		.value("EnableJustInBetaMode", fsdk::FSDKError::EnableJustInBetaMode)

		.value("FiltredAggregationError", fsdk::FSDKError::FiltredAggregationError)

		.value("BatchedInternalError", fsdk::FSDKError::BatchedInternalError)
		.value("UnsupportedFunctionality", fsdk::FSDKError::UnsupportedFunctionality)

		.value("HighMemoryUsage", fsdk::FSDKError::HighMemoryUsage)
		.value("IncompatibleModelVersions", fsdk::FSDKError::IncompatibleModelVersions)
		.value("ModelNotLoaded", fsdk::FSDKError::ModelNotLoaded)
		.value("InvalidConfig", fsdk::FSDKError::InvalidConfig)
			;

	py::enum_<fsdk::LicenseFeature>(f, "LicenseFeature", "License features.\n")
		.value("Detection", fsdk::LicenseFeature::Detection)
		.value("BestShot", fsdk::LicenseFeature::BestShot)
		.value("Attributes", fsdk::LicenseFeature::Attributes)
		.value("Emotions", fsdk::LicenseFeature::Emotions)
		.value("FaceFeatures", fsdk::LicenseFeature::FaceFeatures)
		.value("Liveness", fsdk::LicenseFeature::Liveness)
		.value("Descriptor", fsdk::LicenseFeature::Descriptor)
		.value("DescriptorIndex", fsdk::LicenseFeature::DescriptorIndex)
		.value("LivenessEngine", fsdk::LicenseFeature::LivenessEngine)
		.value("TrackEngine", fsdk::LicenseFeature::TrackEngine)
		.value("HumanDetection", fsdk::LicenseFeature::HumanDetection)
			;

	py::class_<fsdk::ILicensePtr>(f, "ILicensePtr",
		"License objects interface.\n"
		"Use License objects to adopt FaceEngine functionality.\n")
		.def("checkFeatureId", [](
			const fsdk::ILicensePtr& license,
			fsdk::LicenseFeature featureId
			) {
				const auto res = license->checkFeatureId(featureId);
				if (res.isOk()) {
					return std::make_tuple(FSDKErrorResult(fsdk::makeResult(res.getError())), res.getValue());
				}

				return std::make_tuple(FSDKErrorResult(fsdk::makeResult(res.getError())), false);
			},
			"Checks if the feature with featureId is available in this license.\n"
			"\t\t(see fsdk::LicenseFeature for details\n"
			"\tArgs:\n"
			"\t\tparam1 (featureId): featureId to check if it available\n"
			"\tReturns:\n"
			"\t\t(tuple with FSDKErrorResult and bool): tuple with FSDKErrorResult and True if feature is available, False if there is not such feature in this"
			"\t\tlicense or feature is expired or license was not activated.\n")

		.def("isActivated", [](
			const fsdk::ILicensePtr& license) {
				const auto res = license->isActivated();
				if (res.isOk()) {
					return std::make_tuple(FSDKErrorResult(fsdk::makeResult(res.getError())), res.getValue());
				}

				return std::make_tuple(FSDKErrorResult(fsdk::makeResult(res.getError())), false);
			},
			"Checks if current license object is activated and could be used by FaceEngine."
			"License object which was not activated could not be used because all features are disabled by default.\n"
			"\tReturns:\n"
			"\t\t(tuple with FSDKErrorResult and bool): tuple with FSDKErrorResult and and True if object is activated, False otherwise.\n")

		.def("getExpirationDate", [](
			const fsdk::ILicensePtr& license,
			fsdk::LicenseFeature feature) {
				const auto res = license->getExpirationDate(feature);
				if (res.isOk()) {
					return std::make_tuple(FSDKErrorResult(fsdk::makeResult(res.getError())), res.getValue());
				}

				return std::make_tuple(FSDKErrorResult(fsdk::makeResult(res.getError())), uint32_t{0});
			},
			"Get the expiation date for the current license object for the feature.\n"
			"\tArgs:\n"
			"\t\tparam1 (featureId): featureId to check if it available\n"
			"\tReturns:\n"
			"\t\t(tuple with FSDKErrorResult and timestamp): Tuple with FSDKErrorResult and timestamp in unixtime format. Zero in case of error.\n")

		.def("loadFromFile", [](
			const fsdk::ILicensePtr& license,
			const char * path) {
				return FSDKErrorResult(fsdk::makeResult(license->loadFromFile(path).getError()));
			},
			"Loads license from file.\n"
			"\tArgs:\n"
			"\t\tparam1 (str) path to the file.\n"
			"\tReturns:\n"
			"\t\t(FSDKError): @see FSDKError for details.\n")

		.def("saveToFile", [](
			const fsdk::ILicensePtr& license,
			const char * path) {
				return FSDKErrorResult(fsdk::makeResult(license->saveToFile(path).getError()));
			},
			"Saves license as raw format to the file. This file could be used in the next run of the application.\n"
			"\t\tparam1 (str) path to the file.\n"
			"\tReturns:\n"
			"\t\t(FSDKError): @see FSDKError for details.\n")
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
 			Face
			FaceEngineEdition
			FaceEngineEdition.FrontEndEdition
			FaceEngineEdition.CompleteEdition

			PyIFaceEngine
			PyIFaceEngine.getFaceEngineEdition
			PyIFaceEngine.createAttributeEstimator
			PyIFaceEngine.createQualityEstimator
			PyIFaceEngine.createLivenessFlyingFacesEstimator
			PyIFaceEngine.createLivenessRGBMEstimator

			PyIFaceEngine.createHeadPoseEstimator
			PyIFaceEngine.createBlackWhiteEstimator
			PyIFaceEngine.createDepthEstimator
			PyIFaceEngine.createIREstimator
			PyIFaceEngine.createFaceFlowEstimator
			PyIFaceEngine.createEyeEstimator
			PyIFaceEngine.createEmotionsEstimator
			PyIFaceEngine.createGazeEstimator
			PyIFaceEngine.createGlassesEstimator
			PyIFaceEngine.createMouthEstimator
			PyIFaceEngine.createMaskMedicalEstimator
			PyIFaceEngine.createOverlapEstimator
	
			PyIFaceEngine.createDetector
			PyIFaceEngine.createWarper
			PyIFaceEngine.createHumanWarper
			PyIFaceEngine.createDescriptor
			PyIFaceEngine.createDescriptorBatch
			PyIFaceEngine.createExtractor
			PyIFaceEngine.createMatcher
			PyIFaceEngine.setSettingsProvider
			PyIFaceEngine.setRuntimeSettingsProvider
			PyIFaceEngine.getSettingsProvider
			PyIFaceEngine.getRuntimeSettingsProvider
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
			IQualityEstimatorPtr.estimate_quality
			IQualityEstimatorPtr.estimate_subjective_quality

			IAttributeEstimatorPtr
			IAttributeEstimatorPtr.estimate

			IGlassesEstimatorPtr
			IGlassesEstimatorPtr.estimate

			IMouthEstimatorPtr
			IMouthEstimatorPtr.estimate

			IMedicalMaskEstimatorPtr
			IMedicalMaskEstimatorPtr.estimate

			IOverlapEstimatorPtr
			IOverlapEstimatorPtr.estimate

			ILivenessFlyingFacesEstimatorPtr
			ILivenessFlyingFacesEstimatorPtr.estimate

			ILivenessRGBMEstimatorPtr
			ILivenessRGBMEstimatorPtr.update
			ILivenessRGBMEstimatorPtr.estimate

			IDetectorPtr
			IDetectorPtr.detect
			IDetectorPtr.detectOne
			IDetectorPtr.setDetectionComparer
			IDetectorPtr.redetectOne
			IDetectorPtr.redetect

			Face
			Face.isValid

			IHumanDetectorPtr
			IHumanDetectorPtr.detect

			Human
			Human.isValid

			IWarperPtr
			IWarperPtr.warp
			IWarperPtr.createTransformation

			IHumanWarperPtr
			IHumanWarperPtr.warp

			IDescriptorPtr
			IDescriptorPtr.getModelVersion
			IDescriptorPtr.getDescriptorType
			IDescriptorPtr.getDescriptorLength
			IDescriptorPtr.getDescriptor
			IDescriptorPtr.getData
			IDescriptorPtr.load
			IDescriptorPtr.save

			IDescriptorBatchPtr
			IDescriptorBatchPtr.add
			IDescriptorBatchPtr.removeFast
			IDescriptorBatchPtr.removeSlow
			IDescriptorBatchPtr.getMaxCount
			IDescriptorBatchPtr.getCount
			IDescriptorBatchPtr.getModelVersion
			IDescriptorBatchPtr.getDescriptorType
			IDescriptorBatchPtr.getDescriptorSize
			IDescriptorBatchPtr.getDescriptorSlow
			IDescriptorBatchPtr.getDescriptorFast
			IDescriptorBatchPtr.clear
			IDescriptorBatchPtr.load
			IDescriptorBatchPtr.save

			DescriptorBatchError

			IDescriptorExtractorPtr
			IDescriptorExtractorPtr.getModelVersion
			IDescriptorExtractorPtr.getDescriptorType
			IDescriptorExtractorPtr.extractFromWarpedImage
			IDescriptorExtractorPtr.extractFromWarpedImageBatch


			IDescriptorMatcherPtr
			IDescriptorMatcherPtr.getModelVersion
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

			ILivenessFlowEstimatorPtr
			ILivenessFlowEstimatorPtr.estimate

			IEyeEstimatorPtr
			IEyeEstimatorPtr.estimate
			
			EyesRects
			EyesRects.leftEyeRect
			EyesRects.leftEyeRect
			EyeCropper
			EyeCropper.__init__
			EyeCropper.cropByLandmarks5
			EyeCropper.cropByLandmarks68

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

			AttributeRequest
			AttributeRequest.estimateAge
			AttributeRequest.estimateGender
			AttributeRequest.estimateEthnicity

			AttributeResult
			AttributeResult.__init__
			AttributeResult.__repr__

			Quality
			Quality.__init__
			Quality.getQuality

			SubjectiveQuality
			SubjectiveQuality.__init__
			SubjectiveQuality.isGood

			EthnicityEstimation
			EthnicityEstimation.__init__
			EthnicityEstimation.__repr__
			EthnicityEstimation.getEthnicityScore
			EthnicityEstimation.getPredominantEthnicity

			HeadPoseEstimation
			HeadPoseEstimation.__init__
			HeadPoseEstimation.__repr__
			HeadPoseEstimation.getFrontalFaceType

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

			LivenessFlyingFacesEstimation
			LivenessFlyingFacesEstimation.__init__
			LivenessFlyingFacesEstimation.__repr__

			LivenessRGBMEstimation
			LivenessRGBMEstimation.__init__
			LivenessRGBMEstimation.__repr__

			Ethnicity

			GlassesEstimation

			OverlapEstimation

			MouthEstimation

			MedicalMask
			MedicalMaskEstimation

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
			Image.convert

			ImageType
			
			ImageError
			TargetDevice
			ImageCompression

			SettingsProviderError

			Rect
			RectFloat

			ObjectDetectorClassType
			ObjectDetectorClassType.FACE_DET_DEFAULT
			ObjectDetectorClassType.FACE_DET_V1
			ObjectDetectorClassType.FACE_DET_V2
			ObjectDetectorClassType.FACE_DET_V3
			
			Detection
			Detection.isValid
			Detection.__repr__

			HumanDetection
			HumanDetection.isValid

			DetectionFloat
			DetectionFloat.isValid
			DetectionFloat.__repr__
			
			DetectionComparerType
			DetectionComparerType.DCT_CONFIDENCE
			DetectionComparerType.DCT_CENTER
			DetectionComparerType.DCT_CENTER_AND_CONFIDENCE
			DetectionComparerType.DCT_SIZE

			DetectionType
			DetectionType.DT_BBOX
			DetectionType.DT_LANDMARKS5
			DetectionType.DT_LANDMAKRS68

			OrientationType
			OrientationType.Normal
			OrientationType.Left
			OrientationType.Right
			OrientationType.UpsideDown

			HumanDetectionType
			HumanDetectionType.DCT_BOX
			HumanDetectionType.DCT_POINTS
			HumanDetectionType.DCT_ALL

			FSDKError
			FSDKError.Ok
			FSDKError.Internal
			FSDKError.InvalidInput
			FSDKError.InvalidImage
			FSDKError.InvalidRect
			FSDKError.InvalidImageFormat
			FSDKError.InvalidImageSize
			FSDKError.InvalidDetection
			FSDKError.InvalidLandmarks5
			FSDKError.InvalidLandmarks68
			FSDKError.InvalidTransformation
			FSDKError.InvalidDescriptor
			FSDKError.InvalidDescriptorBatch
			FSDKError.InvalidSettingsProvider
			FSDKError.ModuleNotInitialized
			FSDKError.ModuleNotReady
			FSDKError.LicenseError
			FSDKError.BufferIsNull
			FSDKError.BufferIsFull
			FSDKError.BufferIsEmpty
			FSDKError.InvalidBufferSize
			FrontalFaceType
			FrontalFaceType.FrontalFace0
			FrontalFaceType.FrontalFace1
			FrontalFaceType.FrontalFace2

			DepthRange
			DepthRange.__repr__
			DepthEstimation
			DepthEstimation.__repr__
			loadImage

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

			LicenseFeature
			LicenseFeature.Detection
			LicenseFeature.BestShot
			LicenseFeature.Attributes
			LicenseFeature.Emotions
			LicenseFeature.FaceFeatures
			LicenseFeature.Liveness
			LicenseFeature.Descriptor
			LicenseFeature.DescriptorIndex
			LicenseFeature.LivenessEngine
			LicenseFeature.TrackEngine
			LicenseFeature.HumanDetection

			ILicensePtr
			ILicensePtr.checkFeatureId
			ILicensePtr.getExpirationDate
			ILicensePtr.isActivated
			ILicensePtr.loadFromFile
			ILicensePtr.saveToFile

			HumanLandmarks17
			HumanLandmarks17.__len__
			HumanLandmarks17.__getitem__
			HumanLandmarks17.__setitem__

    )pbdoc";
}


