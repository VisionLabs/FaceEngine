#include <pybind11/pybind11.h>
#include <fsdk/FaceEngine.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>
#include "ErrorsAdapter.hpp"
#include "FaceEngineAdapter.hpp"
#include "SettingsProviderAdapter.hpp"
#include "helpers.hpp"

namespace py = pybind11;

void estimators_module(py::module& f) {
	
	py::class_<fsdk::IQualityEstimatorPtr>(f, "IQualityEstimatorPtr",
		"Image quality estimator interface.\n"
		"This estimator is designed to work with a person face image; you should pass a warped face detection image.\n"
		"Quality estimator detects the same attributes as all the other estimators:\n"
		"\tover/under exposure;\n"
		"\tblurriness;\n"
		"\tnatural/unnatural colors;\n")
	
		.def("estimate",[](
			const fsdk::IQualityEstimatorPtr& est,
			const fsdk::Image &warp) {
				fsdk::Quality out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, out);
				if (err.isOk())
					return py::cast(out);
				else
					return py::cast(FSDKErrorResult(err)); },
			"Estimate the quality. If success returns quality output structure with quality params, else error code "
			"(see FSDKErrorResult for details). \n"
			"\tArgs:\n"
			"\t\tparam1 (Image): image with warped face. Format must be R8G8B8"
			"\tReturns:\n"
			"\t\t(Quality): if success - output Quality,\n"
			"\t\t(FSDKErrorResult): else error code FSDKErrorResult\n")
		;
	
	py::class_<fsdk::IAttributeEstimatorPtr>(f, "IAttributeEstimatorPtr",
		"Face image attribute estimator interface. This estimator is designed to work with a person face image; \n"
		"you should pass a warped face detection image. Estimated attributes are: \n"
		"\tage;\n"
		"\tgender;\n"
		"\tglasses;\n")
	
		.def("estimate", [](
			const fsdk::IAttributeEstimatorPtr& est,
			const fsdk::Image &warp) {
				fsdk::AttributeEstimation out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, out);
				if (err.isOk())
					return py::cast(out);
				else
					return py::cast(FSDKErrorResult(err)); },
			"Estimate the attributes. If success returns AttributeEstimation output structure with params, else error code.\n"
			"\t\t(see FSDKErrorResult for details)\n"
			"\tArgs:\n"
			"\t\tparam1 (Image): image with warped face. Format must be R8G8B8\n"
			"\tReturns:\n"
			"\t\t(AttributeEstimation): if success - AttributeEstimation,\n"
			"\t\t(FSDKErrorResult): else error code FSDKErrorResult\n")
			;
	
	py::class_<fsdk::IEthnicityEstimatorPtr>(f, "IEthnicityEstimatorPtr",
			"Ethnicity estimator interface. This estimator is designed to work with a person face image; \n"
			"you should pass a warped face detection image obtained from IWarper. \n"
			"See EthnicityEstimation for output details")
	
		.def("estimate",[](
			const fsdk::IEthnicityEstimatorPtr& est,
			const fsdk::Image &warp) {
				fsdk::EthnicityEstimation out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, out);
				if (err.isOk())
					return py::cast(out);
				else
					return py::cast(FSDKErrorResult(err)); },
			"Ethnicity estimator interface. If success returns EthnicityEstimation structure with params, else error code. \n"
			"See FSDKErrorResult for details. This estimator is designed to work with a person face image; \n"
			"you should pass a warped face detection image obtained from IWarper\n"
			"\tArgs:\n"
			"\t\tparam1 (Image): image with warped face. Format must be R8G8B8"
			"\tReturns:\n"
			"\t\t(EthnicityEstimation): if success - EthnicityEstimation,\n"
			"\t\t(FSDKErrorResult): else error code FSDKErrorResult\n")
	;
	//	second part of estimators
	py::class_<fsdk::IHeadPoseEstimatorPtr>(f, "IHeadPoseEstimatorPtr",
		"Head pose angles estimator interface.\n"
		"\tThis estimator is designed to work with 68 landmark points extracted from photo using face detector;\n"
		"\tsee IDetector for details.\n"
		"\tdetails Estimated angles are:\n"
		"\t\tpitch;\n"
		"\t\tyaw;\n"
		"\t\troll.\n"
		"\tsee HeadPoseEstimation structure for details about how exactly the estimations are reported")
	
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
					return py::cast(FSDKErrorResult(err)); },
			"Estimate the angles.\n"
			"\tArgs\n"
			"\t\tparam1 (Image): image source image. Format must be R8G8B8.\n"
			"\t\tparam2 (Detection): detection.\n"
			"\tReturns:\n"
			"\t\t(HeadPoseEstimation): if OK - output estimation; see AngleEstimation.\n"
			"\t\t(FSDKErrorResult): else - Result with error specified by FSDKErrorResult.\n")
			;
	
	py::class_<fsdk::Ref<fsdk::IBlackWhiteEstimator>>(f, "IBlackWhiteEstimatorPtr",
		"Grayscale image estimator interface.\n"
		"\tThis estimator is indifferent to image content and dimensions; you can pass both face crops (including\n"
		"\twarped images) and full frames.\n")
	
		.def("estimate",[](
			const fsdk::Ref<fsdk::IBlackWhiteEstimator>& est,
			const fsdk::Image& image) {
				bool outIsGrayscale;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(image, outIsGrayscale);
				if (err.isOk())
					return py::cast(outIsGrayscale);
				else
					return py::cast(FSDKErrorResult(err)); },
			"Check if image is grayscale or colo.\n"
			"\tArgs\n"
			"\t\tparam1 (Image): image source image. Format must be R8G8B8.\n"
			"\tReturns:\n"
			"\t\t(FSDKErrorResult): estimation result; true if image is grayscale, false if not.\n")
			;
	
	py::class_<fsdk::ILivenessDepthEstimatorPtr>(f, "ILivenessDepthEstimatorPtr",
												"Depth estimator interface.\n"
												"\tThis estimator is designed for face analysis using depth map. It works with 16 bit depth map of face warp.\n"
												"\tSee IWarper for details")
	
		.def("estimate",[](
			const fsdk::ILivenessDepthEstimatorPtr& est,
			const fsdk::Image& image) {
				fsdk::ResultValue<fsdk::FSDKError, float> err = est->estimate(image);
			
				return FSDKErrorValueFloat(err); },
			"Check whether or not depth map corresponds to the real person.\n"
			"\tArgs\n"
			"\t\tparam1 (Image): warped depth image with R16 format.\n"
			"\tReturns:\n"
			"\t\t(FSDKErrorValueFloat): ResultValue with error code and score of estimation.\n"
			"\t\t\testimation score normalized between 0.0 and 1.0,\n"
			"\t\t\twhere 1.0 equals to 100% confidence that person on image is alive, and 0.0 equals to 0%.\n")
		
		.def("setRange",[](
			const fsdk::ILivenessDepthEstimatorPtr& est,
			const fsdk::DepthRange& range) {
				return est->setRange(range); },
			"Set depth range for estimator.\n"
			"\tArgs\n"
			"\t\tparam1 (DepthRange): range see DepthRange.\n"
			"\tReturns:\n"
			"\t\t(bool): True - if range was set, otherwise - False.\n"
			"\t\t\tif !range.isOk() range is not set.\n")
			;

// IREstimation
	py::class_<fsdk::IREstimation>(f, "IREstimation",
		"IR estimation output.\n"
		"\tThese values are produced by ILivenessIREstimator object.\n")
		.def(py::init<>())
		.def_readwrite("isReal", &fsdk::IREstimation::isReal, "\tbool answer, the real person or not")
		.def_readwrite("score", &fsdk::IREstimation::score, "\t score")
		.def("__repr__",
			[](const fsdk::IREstimation &result) {
				return "isReal = " + std::to_string(result.isReal)
						+ ", score = " + std::to_string(result.score); })
		;
	
	py::class_<fsdk::ILivenessIREstimatorPtr>(f, "ILivenessIREstimatorPtr",
		"Infra-red liveness estimator interface.\n"
		"\tThis estimator is designed for face analysis using infra red facial warp (8-bit 1 channel) image.\n"
		"\tIWarper for details.\n")
	
		.def("estimate",[](
			const fsdk::ILivenessIREstimatorPtr& est,
			const fsdk::Image& irWarp) {
				fsdk::IREstimation irEstimation;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(irWarp, irEstimation);
				if (err.isOk())
					return py::cast(irEstimation);
				else
					return py::cast(FSDKErrorResult(err)); },
			"Check whether or not infrared warp corresponds to the real person.\n"
			"\tArgs\n"
			"\t\tparam1 (Image): irWarp infra red face warp\n"
			"\tReturns:\n"
			"\t\t(IREstimation): if OK - return irEstimation\n"
			"\t\t\t(FSDKErrorResult): else - Error code.\n")
			;
	
	py::class_<fsdk::ISmileEstimatorPtr>(f, "ISmileEstimatorPtr",
		"Smile estimator interface.\n"
		"\tThis estimator is designed for smile/mouth/mouth overlap detection.\n"
		"\tIt works with warped image see IWarper for details.\n")
	
		.def("estimate",[](
			const fsdk::ISmileEstimatorPtr& est,
			const fsdk::Image& image) {
				fsdk::SmileEstimation out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(image, out);
				if (err.isOk())
					return py::cast(out);
				else
					return py::cast(FSDKErrorResult(err)); },
			"Estimate SmileEstimation probabilities.\n"
			"\tArgs\n"
			"\t\tparam1 (Image): face warped image.\n"
			"\tReturns:\n"
			"\t\t(SmileEstimation): if OK - estimation\n"
			"\t\t(FSDKErrorResult): else - Error code.\n")
			;
	
	py::class_<fsdk::ILivenessFlowEstimatorPtr>(f, "ILivenessFlowEstimatorPtr",
		"\tLiveness flow estimator interface.\n"
		"\tThis estimator is designed for liveness detection. It works with 1 close-range face crop\n"
		"\tand 10 long-range face crops estimating correctness of optical flow between theese frames.\n"
		"\tRequired input images - 96x96 face warp central crops.\n"
		"\tsee IWarper for details.\n")
	
		.def("estimate",[](
			const fsdk::ILivenessFlowEstimatorPtr& est,
			const fsdk::Image& small,
			std::vector<fsdk::Image> framesPyList) {
				double score = 0.0;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(small, framesPyList.data(), framesPyList.size(), score);
				if (err.isOk())
					return py::cast(score);
				else
					return py::cast(FSDKErrorResult(err)); },
			"Check if correct optical flow can be calculated from input images..\n"
			"\tArgs\n"
			"\t\tparam1 (Image): small face crop\n"
			"\t\tparam2 (list): list with big face crops.\n"
			"\tReturns:\n"
			"\t\t(double): if OK - score.\n"
			"\t\t(FSDKErrorResult): else - error code")
			;
	
	py::class_<fsdk::IEyeEstimatorPtr>(f, "IEyeEstimatorPtr",
		"Eye estimator interface.\n"
		"This estimator is designed to work with a person face image; you should pass a warped face detection image.\n"
		"Eye estimator detects:\n"
		"\teyes state;\n"
		"\tlandmarks describing iris.\n"
		"See EyesEstimation for output details.\n"
		"More detailed description see in FaceEngineSDK_Handbook.pdf or source C++ interface.\n")
	
		.def("estimate",[](
			const fsdk::IEyeEstimatorPtr& est,
			const fsdk::Image& warp,
			const fsdk::Landmarks5& landmarks5) {
				fsdk::EyesEstimation out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, landmarks5, out);
				if (err.isOk())
					return py::cast(out);
				else
					return py::cast(FSDKErrorResult(err)); },
			"Estimate the attributes.\n"
			"\tArgs\n"
			"\t\tparam1 (Image): warp source image. Format must be R8G8B8. Must be warped!\n"
			"\t\tparam2 (Landmarks5): landmarks5 landmark of size 5 used to warp image, "
			"must be in warped image coordinates. @see IWarper\n"
			"\tReturns:\n"
			"\t\t(EyesEstimation): if OK - return eye estimation\n"
			"\t\t(FSDKErrorResult): else - Error code")
		
		.def("estimate",[](
			const fsdk::IEyeEstimatorPtr& est,
			const fsdk::Image& warp,
			const fsdk::Landmarks68& landmarks68) {
				fsdk::EyesEstimation out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, landmarks68, out);
				if (err.isOk())
					return py::cast(out);
				else
					return py::cast(FSDKErrorResult(err)); },
			"Estimate the attributes.\n"
			"\tArgs\n"
			"\t\tparam1 (Image): warp source image. Format must be R8G8B8. Must be warped!\n"
			"\t\tparam2 (Landmarks68): landmark of size 68 used to warp image, must be "
			"in warped image coordinates.\n"
			"\tReturns:\n"
			"\t\t(EyesEstimation): if OK - return eye estimation"
			"\t\t(FSDKErrorResult): else - Error code")
			;
	
	py::class_<fsdk::IEmotionsEstimatorPtr>(f, "IEmotionsEstimatorPtr",
		"Emotions estimator interface.\n"
		"\tThis estimator is designed to work with a person face image; y"
		"ou should pass a warped face detection image obtained from IWarper.\n"
		"\tsee IWarper for details.\n"
		"\tEmotions estimator detects set of emotions depiceted on given face.\n"
		"\tsee EmotionsEstimation for output details.\n")
		.def("estimate",[](
			const fsdk::IEmotionsEstimatorPtr& est,
			const fsdk::Image& warp) {
				fsdk::EmotionsEstimation out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, out);
				if (err.isOk())
					return py::cast(out);
				else
					return py::cast(FSDKErrorResult(err)); },
			"\tEstimate the attributes.\n"
			"\tArgs\n"
			"\t\tparam1 (Image): warp source image. If format is not R8 it would be converted to R8. "
			"Must be warped!\n"
			"\tReturns:\n"
			"\t\t(EmotionsEstimation): if OK - estimation of emotions. see EmotionsEstimation for details\n"
			"\t\t(FSDKErrorResult): else - Error code")
			;
	
	py::class_<fsdk::IGazeEstimatorPtr>(f, "IGazeEstimatorPtr",
			"Gaze estimator interface.\n"
			"\tThis estimator is designed to work with 68 facial landmarks, HeadPoseEstimation\n"
			"\t(see IHeadPoseEstimator) and EyesEstimations of both eyes (@see IEyeEstimator).\n"
			"\tsee GazeEstimation structure for details about how exactly the estimations are reported.\n"
			"\tInput points should be relative to the same coordinate system. Best results are achieved\n"
			"\tif coordinate system is tied to image on which input data was retrieved.\n")
		.def("estimate",[](
			const fsdk::IGazeEstimatorPtr& est,
			const fsdk::HeadPoseEstimation& angles,
			const fsdk::EyesEstimation& eyesEstimation) {
				fsdk::GazeEstimation out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(angles, eyesEstimation, out);
				if (err.isOk())
					return py::cast(out);
				else
					return py::cast(FSDKErrorResult(err)); },
			"Estimate the eye angles.\n"
			"\tArgs\n"
			"\t\tparam1 (HeadPoseEstimation): HeadPoseEstimation calculated using landmarks68.\n"
			"\t\tparam2 (EyesEstimation): EyesEstimation of eyes.\n"
			"\tReturns:\n"
			"\t\t(GazeEstimation): if OK - output estimation; @see GazeEstimation.\n"
			"\t\t(FSDKErrorResult): else - error code")
			;
	
	py::class_<fsdk::IAGSEstimatorPtr>(f, "IAGSEstimatorPtr",
		"Approximate Garbage Score estimator interface.\n"
		"\tThis estimator is designed to work with Image and detection.\n")
			.def("estimate",[](
				const fsdk::IAGSEstimatorPtr& est,
				const fsdk::Image& image,
				const fsdk::Detection& detection) {
					fsdk::ResultValue<fsdk::FSDKError, float> err = est->estimate(image, detection);
					if (err.isOk())
						return std::make_tuple(FSDKErrorResult(err), err.getValue());
					else
						return std::make_tuple(FSDKErrorResult(err), 0.0f); },
				"Estimate the ags.\n"
				"\tArgs\n"
				"\t\tparam1 (Image): image source image in R8G8B8 format.\n"
				"\t\tparam2 (detection): detection coords in image space.\n"
				"\tReturns:\n"
				"\t\t(tuple with FSDKErrorResult and float value): Error code and float value.")
					;

	py::class_<fsdk::MatchingResult>(f, "MatchingResult", "Result of descriptor matching.")
		.def(py::init<>(), "Initializes result to default values.")

		.def(py::init<float, float>(),
			"Initializes result.\n"
			"\tArgs\n"
			"\t\tparam1 (float): distance value.\n"
			"\t\tparam2 (float): similarity value.\n")
		.def_readwrite("distance", &fsdk::MatchingResult::distance)
		.def_readwrite("similarity", &fsdk::MatchingResult::similarity)
		.def("__repr__",
			[](const fsdk::MatchingResult &result) {
				return "distance = " + std::to_string(result.distance)
						+ ", similarity = " + std::to_string(result.similarity); })
			;
	
	py::class_<fsdk::SearchResult>(f, "SearchResult", "Result of index search.\n")
		.def(py::init<>(), "Default constructor.\n")
		.def(py::init<float, float, fsdk::DescriptorId>(),
			"Construct structure with parameters.\n"
			"\tArgs\n"
			"\t\tparam1 (float): Distance between descriptors.\n"
			"\t\tparam2 (float): Similarity between descriptors.\n"
			"\t\tparam3 (int): Index of found descriptors in some storage.\n")
		.def_readwrite("distance", &fsdk::SearchResult::distance)
		.def_readwrite("similarity", &fsdk::SearchResult::similarity)
		.def_readwrite("index", &fsdk::SearchResult::index)
		.def("__repr__",
			[](const fsdk::SearchResult &result) {
				return "distance = " + std::to_string(result.distance)
						+ ", similarity = " + std::to_string(result.similarity)
						+ ", index = " + std::to_string(result.index); })
			;
	py::class_<fsdk::EyesEstimation::EyeAttributes::IrisLandmarks>(f, "IrisLandmarks",
			"Iris landmarks, length is fixed and equal to 32.\n"
			"\tMTCNN face detector is capable of face landmarks detection. "
			"\tLandmarks are special classes binded to python. \n"
			"\tThey are similar on python lists. It is possible to use some standard "
			"python built-in functions for them: \n"
			"\t`__len__`, `__getitem__`. The method `__setitem__` is used only for "
			"test and research purposes with class Vector2f. \n"
			"\tMore detailed description see in FaceEngineSDK_Handbook.pdf or source C++ interface.\n")
		
		.def(py::init<>())
		.def("__len__", [](const fsdk::EyesEstimation::EyeAttributes::IrisLandmarks &v)
			{ return fsdk::EyesEstimation::EyeAttributes::irisLandmarksCount; },
			"Called to implement the built-in function len(). Should return the length of the object, an integer >= 0.\n"
			"\tExample: len(landmarks)")
		
		.def("__getitem__", [](const fsdk::EyesEstimation::EyeAttributes::IrisLandmarks &l, size_t i) {
				if (i >= fsdk::EyesEstimation::EyeAttributes::irisLandmarksCount) throw py::index_error();
				return l.landmarks[i]; },
			"Called to implement evaluation of self[key]. The accepted keys should be integers.\n "
			"\tExample:lanmarks[3]")
		
		.def("__setitem__", [](fsdk::EyesEstimation::EyeAttributes::IrisLandmarks &s,
							 size_t i,
							 fsdk::Vector2<float> v) {
				if (i >= fsdk::EyesEstimation::EyeAttributes::irisLandmarksCount) throw py::index_error();
				s.landmarks[i].x = v.x;
				s.landmarks[i].y = v.y;
				return s.landmarks[i]; },
			"Called to implement assignment to self[key]. \n"
			"\tThe method `__setitem__` is used only for test and research purposes with class Vector2f.\n "
			"\tExample: lanmarks[i] = FaceEngine.Vector2f(10.0, 20.0)")
			;   //IrisLandmarks
	
	py::class_<fsdk::EyesEstimation::EyeAttributes::EyelidLandmarks>(f, "EyelidLandmarks",
		"Eyelid landmarks, length is fixed and equal to 6.\n"
		"\tMTCNN face detector is capable of face landmarks detection. "
		"\tLandmarks are special classes binded to python. \n"
		"\tThey are similar on python lists. It is possible to use some standard "
		"python built-in functions for them: \n"
		"\t`__len__`, `__getitem__`. The method `__setitem__` "
		"is used only for test and research purposes with class Vector2f. \n"
		"\tMore detailed description see in FaceEngineSDK_Handbook.pdf or source C++ interface.\n")
	
		.def(py::init<>())
		.def("__len__", [](const fsdk::EyesEstimation::EyeAttributes::EyelidLandmarks &v)
			{ return fsdk::EyesEstimation::EyeAttributes::eyelidLandmarksCount; },
			"Called to implement the built-in function len(). Should return the length of the object, an integer >= 0.\n"
			"\tExample: len(landmarks)")
		
		.def("__getitem__", [](const fsdk::EyesEstimation::EyeAttributes::EyelidLandmarks &l, size_t i) {
				if (i >= fsdk::EyesEstimation::EyeAttributes::eyelidLandmarksCount) throw py::index_error();
				return l.landmarks[i]; },
			"Called to implement evaluation of self[key]. The accepted keys should be integers.\n "
			"\tExample:lanmarks[3]")
		
		.def("__setitem__", [](
			fsdk::EyesEstimation::EyeAttributes::EyelidLandmarks &s,
			size_t i,
			fsdk::Vector2<float> v) {
				if (i >= fsdk::EyesEstimation::EyeAttributes::eyelidLandmarksCount) throw py::index_error();
				s.landmarks[i].x = v.x;
				s.landmarks[i].y = v.y;
				return s.landmarks[i];
			},
			"Called to implement assignment to self[key]. \n"
			"\tThe method `__setitem__` is used only for test and research purposes with class Vector2f.\n "
			"\tExample: lanmarks[i] = FaceEngine.Vector2f(10.0, 20.0)")
			; //EyelidLandmarks
// Attribute
	py::class_<fsdk::AttributeEstimation>(f, "AttributeEstimation",
		"Attribute estimation output.\n"
		"\tThese values are produced by IComplexEstimator object.\n"
		"\tEach estimation is given in normalized [0, 1] range. Parameter meanings:\n"
		"\t\tgender: 1 - male, 0 - female;\n"
		"\t\tglasses: 1 - person wears glasses, 0 - person doesn't wear glasses;\n"
		"\t\tage: estimated age (in years).\n")
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
	py::class_<fsdk::Quality>(f, "Quality",
		"Quality estimation structure\n"
		"\tEach estimation is given in normalized [0, 1] range. Parameter meanings:\n"
		"\t\tlight: image overlighting degree. 1 - ok, 0 - overlighted;\n"
		"\t\tdark: image darkness degree. 1 - ok, 0 - to dark;\n"
		"\t\tgray: image grayness degree 1 - ok, 0 - to gray;\n"
		"\t\tblur: image blur degree. 1 - ok, 0 - to blured.\n")
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
	py::class_<fsdk::EthnicityEstimation>(f, "EthnicityEstimation",
		"Ethnicity estimation structure.\n"
		"\tEach estimation is given in normalized [0, 1] range.\n")
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
		.def("getEthnicityScore", &fsdk::EthnicityEstimation::getEthnicityScore,
			"Returns score of required ethnicity")
		.def("getPredominantEthnicity", &fsdk::EthnicityEstimation::getPredominantEthnicity,
			"Returns ethnicity with greatest score"
			"\tArgs:"
			"\t\tparam1 (Ethnicity): ethnicity")
			;

//	HeadPose
	py::class_<fsdk::HeadPoseEstimation>(f, "HeadPoseEstimation",
		"Head pose estimation output.\n"
		"\tThese values are produced by IHeadPoseEstimator object.\n"
		"\tEach angle is measured in degrees and in [-180, 180] range.\n")
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
		
		py::class_<fsdk::DepthRange>(f, "DepthRange",
			"Depth range configuration structure in millimeters.\n"
			"\tSpecifies working range of distances for depth estimator.\n"
			"\tAverage depth map value should belong to this range.\n"
			"\tBy default configured for kinect depth sensor.\n")
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

// EyesEstimation
	py::class_<fsdk::EyesEstimation>(f, "EyesEstimation",
		"Eyes estimation output.\n"
		"\tThese values are produced by IEyeEstimator object.\n")
		.def(py::init<>())
		.def_readwrite("leftEye", &fsdk::EyesEstimation::leftEye)
		.def_readwrite("rightEye", &fsdk::EyesEstimation::rightEye)
			;
	
		py::enum_<fsdk::EyesEstimation::EyeAttributes::State>(f, "State", "Enumeration of possible eye states.\n")
		.value("Closed", fsdk::EyesEstimation::EyeAttributes::State::Closed, "Eye is closed.\n")
		.value("Open", fsdk::EyesEstimation::EyeAttributes::State::Open, "Eye is open.\n")
		.value("Occluded", fsdk::EyesEstimation::EyeAttributes::State::Occluded,
			"Eye is blocked by something not transparent,\n"
			"\tor landmark passed to estimator doesn't point to an eye.\n")
			;

	py::class_<fsdk::EyesEstimation::EyeAttributes>(f, "EyeAttributes", "Eyes attribute structure.")
		.def_readwrite("state", &fsdk::EyesEstimation::EyeAttributes::state)
		.def_readwrite("iris", &fsdk::EyesEstimation::EyeAttributes::iris)
		.def_readwrite("eyelid", &fsdk::EyesEstimation::EyeAttributes::eyelid)
			;

// Emotions
	py::class_<fsdk::EmotionsEstimation>(f, "EmotionsEstimation",
		"Emotions estimation structure.\n"
		"\tEach estimation is given in normalized [0, 1] range.\n")
		.def(py::init<>())
		.def_readwrite("anger", &fsdk::EmotionsEstimation::anger)
		.def_readwrite("disgust", &fsdk::EmotionsEstimation::disgust)
		.def_readwrite("fear", &fsdk::EmotionsEstimation::fear)
		.def_readwrite("happiness", &fsdk::EmotionsEstimation::happiness)
		.def_readwrite("sadness", &fsdk::EmotionsEstimation::sadness)
		.def_readwrite("surprise", &fsdk::EmotionsEstimation::surprise)
		.def_readwrite("neutral", &fsdk::EmotionsEstimation::neutral)
		.def("getPredominantEmotion", &fsdk::EmotionsEstimation::getPredominantEmotion,
			"Returns emotion with greatest score")
		.def("getEmotionScore", &fsdk::EmotionsEstimation::getEmotionScore,
			"Returns score of required emotion"
			"\tArgs\n"
			"\t\tparam1 (emotion): emotion\n")
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
		; // Emotions
	
	py::enum_<fsdk::EmotionsEstimation::Emotions>(f, "Emotions", py::arithmetic(), "Emotions enum")
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
		py::class_<fsdk::GazeEstimation>(f, "GazeEstimation",
										"Gaze estimation output.\n"
										"\tThese values are produced by IGazeEstimatorPtr object.\n"
										"\tEach angle is measured in degrees and in [-180, 180] range.\n")
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
	
	py::class_<fsdk::GazeEstimation::EyeAngles>(f, "EyeAngles", "Eye angles.\n")
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
	py::enum_<fsdk::EthnicityEstimation::Ethnicities>(f, "Ethnicity", "Ethnicity enum.\n")
		.value("AfricanAmerican", fsdk::EthnicityEstimation::AfricanAmerican)
		.value("Indian", fsdk::EthnicityEstimation::Indian)
		.value("Asian", fsdk::EthnicityEstimation::Asian)
		.value("Caucasian", fsdk::EthnicityEstimation::Caucasian)
		.value("Count", fsdk::EthnicityEstimation::Count)
		.export_values();
			;
}
