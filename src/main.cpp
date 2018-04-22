#include <pybind11/pybind11.h>
#include <iostream>
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

	fsdk::IWarperPtr createWarper() {
		return fsdk::acquire(faceEnginePtr->createWarper());
	}
};

class PyISettingsProvider {
public:
	fsdk::ISettingsProviderPtr settingsProviderPtr;

	PyISettingsProvider(const char* path = nullptr) {
		settingsProviderPtr = fsdk::acquire(fsdk::createSettingsProvider(path));
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
	fsdk::Image::Error error;
	const char* what;

	ImageErrorResult(fsdk::Result<fsdk::Image::Error> err) :
		isOk(err.isOk()),
		isError(err.isError()),
		error(err.getError()),
		what(err.what())
		{};
};

struct ErrorValue {

	bool isOk;
	bool isError;
	fsdk::FSDKError fsdkError;
	const char* what;
	int value;

	ErrorValue(fsdk::ResultValue<fsdk::FSDKError, int> err) :
		isOk(err.isOk()),
		isError(err.isError()),
		fsdkError(err.getError()),
		what(err.what()),
		value(err.getValue())
		{};

};

PYBIND11_MAKE_OPAQUE(fsdk::Landmarks5);
PYBIND11_MAKE_OPAQUE(fsdk::Landmarks68);

PYBIND11_MODULE(fe, f) {

	f.def("createPyFaceEnginePtr", &createPyFaceEnginePtr, py::return_value_policy::take_ownership,
		  "Create FaceEngine", py::arg("dataPath") = nullptr, py::arg("configPath") = nullptr);
	f.def("create SettingsProviderPtr", &createSettingsProviderPtr, py::return_value_policy::take_ownership,
		  "Create object SettingsProvider");

	py::class_<PyIFaceEngine>(f, "PyIFaceEngine")
		.def("createAttributeEstimator", &PyIFaceEngine::createAttributeEstimator)
		.def("createQualityEstimator", &PyIFaceEngine::createQualityEstimator)
		.def("createEthnicityEstimator", &PyIFaceEngine::createEthnicityEstimator)
		.def("createDetector", &PyIFaceEngine::createDetector)
		.def("createWarper", &PyIFaceEngine::createWarper)
			;

	py::class_<PyISettingsProvider>(f, "PyISettingsProvider")
		;

	py::class_<fsdk::IFaceEnginePtr>(f, "IFaceEnginePtr");
	py::class_<fsdk::IQualityEstimatorPtr>(f, "IQualityEstimatorPtr")
		.def("estimate",[](
		fsdk::IQualityEstimatorPtr est,
		const fsdk::Image &warp) {
			fsdk::Quality out;
			fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, out);
			auto estResultPy = py::dict();
			estResultPy["FSDKErrorResult"] = FSDKErrorResult(err);
			estResultPy["Quality"] = out;
			return estResultPy; })
		;

	py::class_<fsdk::IAttributeEstimatorPtr>(f, "IAttributeEstimatorPtr")
		.def("estimate", [](
			const fsdk::IAttributeEstimatorPtr& est,
			const fsdk::Image &warp) {
				fsdk::AttributeEstimation out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, out);
				auto estResultPy = py::dict();
				estResultPy["FSDKErrorResult"] = FSDKErrorResult(err);
				estResultPy["AttributeEstimation"] = out;
				return estResultPy; })
				;
	py::class_<fsdk::IEthnicityEstimatorPtr>(f, "IEthnicityEstimatorPtr")
		.def("estimate",[](
			const fsdk::IEthnicityEstimatorPtr& est,
			const fsdk::Image &warp) {
				fsdk::EthnicityEstimation out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, out);
				auto estResultPy = py::dict();
				estResultPy["FSDKErrorResult"] = FSDKErrorResult(err);
				estResultPy["EthnicityEstimation"] = out;
				return estResultPy; })
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
			fsdk::ResultValue<fsdk::FSDKError, int> err = det->detect(image, rect, detections, landmarks, landmarks68, maxCount);
			auto detResultPy = py::list();
			for (size_t i = 0; i < maxCount; ++i) {
				auto tempDict = py::dict();
				tempDict["errorValue"] = ErrorValue(err);
				tempDict["Detection"] = detections[i];
				tempDict["Landmarks5"] = landmarks[i];
				tempDict["Landmarks68"] = landmarks68[i];
				detResultPy.append(tempDict);
			}
			return detResultPy; })
		;

	;
	py::class_<fsdk::IWarperPtr>(f, "IWarperPtr")
		.def("warp",[](
			const fsdk::IWarperPtr& warper,
			const fsdk::Image& image,
			const fsdk::Transformation& transformation) {
				fsdk::Image transformedImage;
				fsdk::Result<fsdk::FSDKError> error = warper->warp(image, transformation, transformedImage);
				auto warpResultPy = py::dict();
				warpResultPy["FSDKErrorResult"] = FSDKErrorResult(error);
				warpResultPy["transformedImage"] = transformedImage;
				return warpResultPy; })
		.def("warp",[](
			const fsdk::IWarperPtr& warper,
			const fsdk::Landmarks5& landmarks,
			const fsdk::Transformation& transformation) {
				fsdk::Landmarks5 transformedLandmarks;
				fsdk::Result<fsdk::FSDKError> error = warper->warp(
					landmarks,
					transformation,
					transformedLandmarks);
				auto warpResultPy = py::dict();
				warpResultPy["FSDKErrorResult"] = FSDKErrorResult(error);
				warpResultPy["transformedLandmarks"] = transformedLandmarks;
				return warpResultPy; })
		.def("warp",[](
			const fsdk::IWarperPtr& warper,
			const fsdk::Landmarks68& landmarks68,
			const fsdk::Transformation& transformation) {
				fsdk::Landmarks68 transformedLandmarks68;
				fsdk::Result<fsdk::FSDKError> error = warper->warp(landmarks68,
																	transformation, transformedLandmarks68);
				auto warpResultPy = py::dict();
				warpResultPy["FSDKErrorResult"] = FSDKErrorResult(error);
				warpResultPy["transformedLandmarks68"] = transformedLandmarks68;
				return warpResultPy; })
		.def("createTransformation",[](
			const fsdk::IWarperPtr& warper,
			const fsdk::Detection& detection,
			const fsdk::Landmarks5& landmarks) {
				return warper->createTransformation(detection, landmarks); })
					;


	py::class_<fsdk::Landmarks5>(f, "Landmarks5")
		.def(py::init<>())
		.def("__len__", [](const fsdk::Landmarks5 &v) { return v.landmarkCount; })
//		.def("__getitem__", [](const fsdk::Landmarks5 &s, size_t i) {
//			if (i >= s.landmarkCount) throw py::index_error();
//			return s.landmarks[i];
//		})
//		.def("__setitem__", [](fsdk::Landmarks68 &s, size_t i, float value) {
//			if (i >= s.landmarkCount) throw py::index_error();
//				s.landmarks[i] = value;
//		})

		.def("getItem", [](fsdk::Landmarks5 &s, size_t i)  {
			if (i >= s.landmarkCount) throw py::index_error();
			return s.landmarks[i];
		})

		.def("setX", [](fsdk::Landmarks5 &s, size_t i, float value) {
			if (i >= s.landmarkCount) throw py::index_error();
			s.landmarks[i].x = value;
		})

		.def("setY", [](fsdk::Landmarks5 &s, size_t i, float value) {
			if (i >= s.landmarkCount) throw py::index_error();
			s.landmarks[i].y = value;
		})
			;

	py::class_<fsdk::Landmarks68>(f, "Landmarks68")
		.def(py::init<>())
		.def("__len__", [](const fsdk::Landmarks68 &v) { return v.landmarkCount; })
//		.def("__getitem__", [](const fsdk::Landmarks68 &s, size_t i) {
//			if (i >= s.landmarkCount) throw py::index_error();
//			return s.landmarks[i];
//		})
		.def("getItem", [](fsdk::Landmarks68 &s, size_t i)  {
			if (i >= s.landmarkCount) throw py::index_error();
			return s.landmarks[i];
		})

		.def("setX", [](fsdk::Landmarks68 &s, size_t i, float value) {
			if (i >= s.landmarkCount) throw py::index_error();
			s.landmarks[i].x = value;
		})

		.def("setY", [](fsdk::Landmarks68 &s, size_t i, float value) {
			if (i >= s.landmarkCount) throw py::index_error();
			s.landmarks[i].y = value;
		})
			;

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

	py::class_<ImageErrorResult>(f, "ImageErrorResult")
	.def_readonly("isOk", &ImageErrorResult::isOk)
	.def_readonly("isError", &ImageErrorResult::isError)
	.def_readonly("ImageErrorResult", &ImageErrorResult::error)
	.def_readonly("what", &ImageErrorResult::what)
	.def("__repr__",
		 [](const ImageErrorResult &err) {
			 return "<example.ImageErrorResult: "
					"isOk = " + std::to_string(err.isOk)
					+ ", isError = " + std::to_string(err.isError)
					+ ", ImageError = " + fsdk::ErrorTraits<fsdk::Image::Error>::toString(err.error)
					+ ", what = " + err.what +  "'>"; })
	;

	py::class_<ErrorValue>(f, "ErrorValue")
		.def_readonly("isOk", &ErrorValue::isOk)
		.def_readonly("isError", &ErrorValue::isError)
		.def_readonly("FSDKError", &ErrorValue::fsdkError)
		.def_readonly("what", &ErrorValue::what)
		.def_readonly("value", &ErrorValue::value)
		.def("__repr__",
			 [](const ErrorValue &err) {
				 return "<example.ErrorValue: "
					"isOk = " + std::to_string(err.isOk)
					+ ", isError = " + std::to_string(err.isError)
					+ ", FSDKError = " + fsdk::ErrorTraits<fsdk::FSDKError >::toString(err.fsdkError)
					+ ", value = " + std::to_string(err.value)
					+ ", what = " + err.what + "'>";
			 })
			;

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
		 });

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

	py::class_<fsdk::EthnicityEstimation>(f, "EthnicityEstimation")
		.def(py::init<>())
		.def_readwrite("africanAmerican", &fsdk::EthnicityEstimation::africanAmerican)
		.def_readwrite("indian", &fsdk::EthnicityEstimation::indian)
		.def_readwrite("asian", &fsdk::EthnicityEstimation::asian)
		.def_readwrite("caucasian", &fsdk::EthnicityEstimation::caucasian)
		.def("__repr__",
			 [](const fsdk::EthnicityEstimation &a) {
				 return "<example.EthnicityEstimation: "
						", africanAmerican = " + std::to_string(a.africanAmerican)
						+ ", indian = " + std::to_string(a.indian)
						+ ", asian = " + std::to_string(a.asian)
						+ ", caucasian = " + std::to_string(a.caucasian) +  "'>";
			 })
		.def("getEthnicityScore", &fsdk::EthnicityEstimation::getEthnicityScore)
		.def("getPredominantEthnicity", &fsdk::EthnicityEstimation::getPredominantEthnicity)
			;

	py::enum_<fsdk::EthnicityEstimation::Ethnicities >(f, "Ethnicity")
		.value("AfricanAmerican", fsdk::EthnicityEstimation::AfricanAmerican)
		.value("Indian", fsdk::EthnicityEstimation::Indian)
		.value("Asian", fsdk::EthnicityEstimation::Asian)
		.value("Caucasian", fsdk::EthnicityEstimation::Caucasian)
		.value("Count", fsdk::EthnicityEstimation::Count)
			.export_values();
			;

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

	py::enum_<fsdk::Format::Type>(f, "Type")
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
			return std::make_tuple(ImageErrorResult(error), image); })

		.def("load", [](fsdk::Image& image,
						const char* path,
						const fsdk::Format format) {
			fsdk::Result<fsdk::Image::Error> error = image.load(path);
			return std::make_tuple(ImageErrorResult(error), image);
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

	py::enum_<fsdk::Image::Error>(f, "Error")
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
//            " *image - path to image\n"
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

