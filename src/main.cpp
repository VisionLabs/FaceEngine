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
//PYBIND11_DECLARE_HOLDER_TYPE(fsdk::IFaceEngine, );

class PyIFaceEngine : public fsdk::IFaceEngine {
public:


	// Create ethnicity estimator.
	fsdk::IAttributeEstimator* createAttributeEstimator() noexcept override {
		PYBIND11_OVERLOAD_PURE(
			fsdk::IAttributeEstimator*,         /* Return type */
			fsdk::IFaceEngine,                   /* Parent class */
			createAttributeEstimator,     /* Name of function */
		/* This function has no arguments. The trailing comma
		   in the previous line is needed for some compilers */
		);
	}

	// Create quality estimator.
	fsdk::IQualityEstimator* createQualityEstimator() noexcept override {

		PYBIND11_OVERLOAD_PURE(
			fsdk::IQualityEstimator*,         /* Return type */
			fsdk::IFaceEngine,                   /* Parent class */
			createQualityEstimator,     /* Name of function */
		/* This function has no arguments. The trailing comma
		   in the previous line is needed for some compilers */
		);
	}
	static fsdk::IAttributeEstimatorPtr pt1;
	static fsdk::IQualityEstimatorPtr pt2;
	fsdk::IAttributeEstimatorPtr getAttributeEstimator() {
		return fsdk::acquire(createAttributeEstimator());
	}

	fsdk::IQualityEstimatorPtr getQualityEstimator() {
		return fsdk::acquire(createQualityEstimator());
	}
};

class PyISettingsProvider: public fsdk::ISettingsProvider {
public:
};

class PyIAttributeEstimator : public fsdk::IAttributeEstimator{
public:


	 fsdk::Result<fsdk::FSDKError> estimate(
			const fsdk::Image &warp,
			fsdk::AttributeEstimation &out) noexcept {
		PYBIND11_OVERLOAD_PURE(
			fsdk::Result<fsdk::FSDKError>,
			fsdk::IAttributeEstimator,
			estimate,
			warp,
			out
		);
	}
};

class PyQualityEstimator: public fsdk::IQualityEstimator {
public:
};

py::object createFaceEnginePy (const char* dataPath = nullptr, const char* configPath = nullptr) {
	return py::cast(fsdk::createFaceEngine(dataPath, configPath));
}

py::object createSettingsProviderPy(const char* path) {
	return py::cast(fsdk::createSettingsProvider(path));
}

class IFaceEnginePtr : fsdk::IFaceEngine {
public:
	py::object createAttributeEstimatorPtr() {
		return py::cast(createAttributeEstimator());
	}

	py::object createQualityEstimatorPtr() {
		return py::cast(createQualityEstimator());
	}
};

class PyIFaceEnginePtr : fsdk::IFaceEnginePtr {

//	// Create quality estimator.
//	py::object createQualityEstimatorPtr() noexcept {
//
//	}

};

class PyIRefCounted : public fsdk::IRefCounted {

	int32_t retain() noexcept override {
		PYBIND11_OVERLOAD_PURE(
			int32_t ,               /* Return type */
			fsdk::IRefCounted,      /* Parent class */
			retain,               /* Name of function */
		/* This function has no arguments. The trailing comma
		   in the previous line is needed for some compilers */
		);
	}

	int32_t release() noexcept override {
		PYBIND11_OVERLOAD_PURE(
			int32_t,                /* Return type */
			fsdk::IRefCounted,      /* Parent class */
			release,              /* Name of function */
		/* This function has no arguments. The trailing comma
		   in the previous line is needed for some compilers */
		);
	}

	int32_t getRefCount() noexcept {
		PYBIND11_OVERLOAD_PURE(
			int32_t,                /* Return type */
			fsdk::IRefCounted,      /* Parent class */
			getRefCount,          /* Name of function */
		/* This function has no arguments. The trailing comma
		   in the previous line is needed for some compilers */
		);
	}
};

class PyImage {

public:

	int load(const std::string &path) {
		return int(image.load(path.c_str()));
	}

	fsdk::Image getImage() {
		return image;
	}

	py::object getPythonImage() {
		 return py::cast(image);
	}

	int getWidth() {
		return image.getWidth();
	}

	int getHeight() {
		return image.getHeight();
	}

	bool isValid() {
		return image.isValid();
	}

	void setImage(const fsdk::Image image) {
		this->image = image;
	}
//	py::str load(const char*) { return "load(const char*)"; }
//	py::str load(const char*, const fsdk::Format) { return "load(const char*, const fsdk::Format)"; }
private:
	fsdk::Image image = fsdk::Image();
};

class PyVector2: public fsdk::Vector2<float> {

};

//PYBIND11_MAKE_OPAQUE(std::map<std::string, double>);
//
//// ...

PYBIND11_MAKE_OPAQUE(fsdk::Landmarks5);
PYBIND11_MAKE_OPAQUE(fsdk::Landmarks68);

PYBIND11_MODULE(fe, f) {


	f.def("createFaceEngine", &createFaceEnginePy, py::return_value_policy::take_ownership,
		  "Create static FaceEngine", py::arg("dataPath") = nullptr, py::arg("configPath") = nullptr);
	f.def("createSettingsProvider", &createSettingsProviderPy, py::return_value_policy::take_ownership,
		  "Create static object SettingsProvider");

//	py::class_<A>(m, "A")
//	.def(py::init<>())
//	.def("set", &A::set)
//	.def("get", &A::get)
//	// Alternative for property access in Python
//	.def_property("x", &A::get, &A::set);
//
//	    fsdk::Landmarks5 landmarks5[MaxDetections];
//    fsdk::Landmarks68 landmarks68[MaxDetections];
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

		.def("getitem", [](fsdk::Landmarks5 &s, size_t i)  {
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
		.def("getitem", [](fsdk::Landmarks68 &s, size_t i)  {
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
		 });

	py::class_<fsdk::Vector2<int>>(f, "Vector2i")
	.def(py::init<>())
	.def(py::init<int, int>())
		.def(py::init<const fsdk::Vector2<int>&>())
	.def_readwrite("x", &fsdk::Vector2<int>::x)
	.def_readwrite("y", &fsdk::Vector2<int>::y)
	.def("__repr__",
		 [](const fsdk::Vector2<int> &v) {
			 return "<Vector2i: x = " + std::to_string(v.x) + ", y = " + std::to_string(v.y) + ">";
		 });


	py::class_<fsdk::IFaceEngine, PyIFaceEngine>(f, "IFaceEngine")
		/* Reference original class in function definitions */
//		.def("createEthnicityEstimator", &fsdk::IFaceEngine::createEthnicityEstimator)
		.def("createAttributeEstimator", &fsdk::IFaceEngine::createAttributeEstimator,
			 py::return_value_policy::reference)
		.def("createQualityEstimator", &fsdk::IFaceEngine::createQualityEstimator,
			 py::return_value_policy::reference);

//	py::class_<fsdk::IRefCounted, PyIRefCounted>(f, "IRefCounted")
//		.def("retain", &fsdk::IRefCounted::retain)
//		.def("release", &fsdk::IRefCounted::release)
//		.def("getRefCount", &fsdk::IRefCounted::getRefCount);

	py::class_<fsdk::IAttributeEstimator, PyIAttributeEstimator>(f, "IAttributeEstimator")
		.def("estimate", &fsdk::IAttributeEstimator::estimate);


	py::class_<fsdk::IQualityEstimator, PyQualityEstimator>(f, "IQualityEstimator");
//		.def("estimate", &fsdk::IAttributeEstimator::estimate);

	py::class_<fsdk::ISettingsProvider, PyISettingsProvider>(f, "ISettingsProvider");
//		.def("estimate", &fsdk::IAttributeEstimator::estimate);

	py::class_<fsdk::AttributeEstimation>(f, "AttributeEstimation")
		.def(py::init<>())
		.def_readwrite("gender", &fsdk::AttributeEstimation::gender)
		.def_readwrite("glasses", &fsdk::AttributeEstimation::glasses)
		.def_readwrite("age", &fsdk::AttributeEstimation::age)
		.def("__repr__",
		 [](const fsdk::AttributeEstimation &a) {
			 return "<example.AttributeEstimation \ngender = "
					+ std::to_string(a.gender) + "\nglasses = "
					+ std::to_string(a.glasses) + "\nage = "
					+ std::to_string(a.age)  + "'>";
		 });

	py::class_<fsdk::Quality>(f, "Quality")
		.def(py::init<>())
		.def_readwrite("light", &fsdk::Quality::light)
		.def_readwrite("dark", &fsdk::Quality::dark)
		.def_readwrite("gray", &fsdk::Quality::gray)
		.def_readwrite("blur", &fsdk::Quality::blur)
		.def("__repr__",
		 [](const fsdk::Quality &a) {
			 return "<example.Quality "
					", light = " + std::to_string(a.light)
					+ ", dark = " + std::to_string(a.dark)
					+ ", gray = " + std::to_string(a.gray)
					+ "blur = " + std::to_string(a.blur) +  "'>";
		 })
	.def("getQuality", &fsdk::Quality::getQuality)
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

	enum Type {
		Unknown,		//!< unknown format.
		B8G8R8X8,		//!< 3 channel 8, bit per channel, B-G-R color order format with 8 bit padding before next pixel.
		R8G8B8X8,		//!< 3 channel 8, bit per channel, R-G-B color order format with 8 bit padding before next pixel.
		B8G8R8,			//!< 3 channel 8, bit per channel, B-G-R color order format.
		R8G8B8,			//!< 3 channel 8, bit per channel, R-G-B color order format.
		R8,				//!< 1 channel 8, bit per channel format.
		R16				//!< 1 channel 16, bit per channel format;
	};



	py::enum_<fsdk::Format::Type>(f, "Type")
		.value("Unknown", fsdk::Format::Unknown)
		.value("B8G8R8X8", fsdk::Format::B8G8R8X8)
		.value("R8G8B8X8", fsdk::Format::R8G8B8X8)
		.value("B8G8R8", fsdk::Format::B8G8R8)
		.value("R8G8B8", fsdk::Format::R8G8B8)
		.value("R8", fsdk::Format::R8)
		.value("R16", fsdk::Format::R16)
		.export_values();
			;


	py::class_<PyImage> image(f, "Image");
		image.def(py::init<>());
		image.def("getWidth", &PyImage::getWidth);
		image.def("getHeight", &PyImage::getHeight);
		image.def("isValid", &PyImage::isValid);
#if defined(PYBIND11_OVERLOAD_CAST)
		.def("load", py::overload_cast<const char*>(&fsdk::Image::load))
		.def("load", py::overload_cast<const char*, const fsdk::Format>(&fsdk::Image::load));

#else
		image.def("load", &PyImage::load);
		image.def("setImage", &PyImage::setImage);
		image.def("getImage", &PyImage::getImage);
		image.def("getPythonImage", &PyImage::getPythonImage, py::return_value_policy::reference_internal);
//        image.def("load", static_cast<py::str (fsdk::Image::*)(const char*, const fsdk::Format)>(&PyImage::load));
#endif


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
