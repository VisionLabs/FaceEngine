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
		"There are two versions of quality estimator that one can use. Each version uses its own CNN internally and outputs slightly different scores.\n "
		"This estimator is designed to work with warped face image.\n")

		.def("estimate_quality",[](
			const fsdk::IQualityEstimatorPtr& est,
			const fsdk::Image &warp) {
				fsdk::Quality out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, out);
				return std::make_tuple(FSDKErrorResult(err), out);
			},
			"Predict quality of an image. Upon success returns fsdk::Quality output structure with quality params and error code "
			"(see FSDKErrorResult for details). \n"
			"\tArgs:\n"
			"\t\tparam1 (Image): image with warped face. Format must be R8G8B8"
			"\tReturns:\n"
			"\t\t(tuple): tuple with error code FSDKErrorResult and fsdk::Quality output structure\n")

		.def("estimate_subjective_quality",[](
			const fsdk::IQualityEstimatorPtr& est,
			const fsdk::Image &warp) {
				fsdk::SubjectiveQuality out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, out);
				return std::make_tuple(FSDKErrorResult(err), out);
			},
			"Predict subjective quality of an image. Upon success returns fsdk::SubjectiveQuality output structure with quality params and error code "
			"(see FSDKErrorResult for details). \n"
			"\tArgs:\n"
			"\t\tparam1 (Image): image with warped face. Format must be R8G8B8"
			"\tReturns:\n"
			"\t\t(tuple): tuple with error code FSDKErrorResult and fsdk::SubjectiveQuality output structure\n")

		.def("estimate",[](
			const fsdk::IQualityEstimatorPtr& est,
			const fsdk::Image &warp) {
				fsdk::SubjectiveQuality out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, out);
				return std::make_tuple(FSDKErrorResult(err), out);
			},
			"Alias for estimate_subjective_quality function call. Kept for backward compatibility with older SDK versions\n")
		;
	
	py::class_<fsdk::IAttributeEstimatorPtr>(f, "IAttributeEstimatorPtr",
		"Face image attribute estimator interface. This estimator is designed to work with a person face image; \n"
		"you should pass a warped face detection image. Estimated attributes are: \n"
		"\tage;\n"
		"\tgender;\n"
		"\tethnicity;\n")
	
		.def("estimate", [](
			const fsdk::IAttributeEstimatorPtr& est,
			const fsdk::Image &warp,
			const fsdk::IAttributeEstimator::EstimationRequest request) {
				fsdk::IAttributeEstimator::EstimationResult result;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, request, result);
				return std::make_tuple(FSDKErrorResult(err), result); },
			"Estimate the attributes for image.\n"
			"\t\t(see FSDKErrorResult for details)\n"
			"\tArgs:\n"
			"\t\tparam1 (Image): image with warped face. Format must be R8G8B8\n"
			"\t\tparam2 (AttributeRequest): request with flags to check parameters to estimate\n"
			"\tReturns:\n"
			"\t\t(tuple): \n"
			"\t\t\t tuple with FSDKErrorResult code and EstimatorResult (see AttributeResult)\n")
		.def("estimate", [](
				const fsdk::IAttributeEstimatorPtr& est,
				const std::vector<fsdk::Image>& warps,
				const fsdk::IAttributeEstimator::EstimationRequest request) {
					std::vector<fsdk::AttributeEstimationResult> results(warps.size());
					fsdk::Span<fsdk::AttributeEstimationResult> resultSpan(results.data(), warps.size());
					fsdk::ResultValue<fsdk::FSDKError, fsdk::AttributeEstimationResult>  err = est->estimate(
						fsdk::Span<const fsdk::Image>(warps.data(), warps.size()),
						request,
						resultSpan);
					if (err.isOk())
						return std::make_tuple(FSDKErrorResult(err),
							std::vector<fsdk::AttributeEstimationResult>(resultSpan.begin(), resultSpan.end()),
							err.getValue());
					else
						return std::make_tuple(FSDKErrorResult(err),
							std::vector<fsdk::AttributeEstimationResult>(),
							fsdk::AttributeEstimationResult()); },
			"Estimate the attributes for batch image.\n"
			"\t\t(see FSDKErrorResult for details)\n"
			"\tArgs:\n"
			"\t\tparam1 (list of Images): list of warped Images. Format must be R8G8B8\n"
			"\t\tparam2 (AttributeRequest): request with flags to check parameters to estimate\n"
			"\tReturns:\n"
			"\t\t(tuple): \n"
			"\t\t\t tuple with FSDKErrorResult code, attribute result and aggregation of results (see AttributeResult)\n")
			;

	py::enum_<fsdk::IAttributeEstimator::EstimationRequest>(f, "AttributeRequest", py::arithmetic(),
		"Face image attribute estimation request enumeration.\n")
		.value("estimateAge", fsdk::IAttributeEstimator::EstimationRequest::estimateAge, "Get age estimation\n")
		.value("estimateGender", fsdk::IAttributeEstimator::EstimationRequest::estimateGender, "Get gender estimation\n")
		.value("estimateEthnicity", fsdk::IAttributeEstimator::EstimationRequest::estimateEthnicity, "Get ethnicity estimation\n")
		.export_values();
			;

	py::class_<fsdk::IAttributeEstimator::EstimationResult>(f, "AttributeResult",
		"Face image attribute estimator results.")
		.def(py::init<>())
		.def_readwrite("age_opt", &fsdk::IAttributeEstimator::EstimationResult::age, "age estimation optional\n")
		.def_readwrite("gender_opt", &fsdk::IAttributeEstimator::EstimationResult::gender, "gender estimation optional\n")
		.def_readwrite("genderScore_opt", &fsdk::IAttributeEstimator::EstimationResult::genderScore, "gender score optional\n")
		.def_readwrite("ethnicity_opt", &fsdk::IAttributeEstimator::EstimationResult::ethnicity, "ethnicity estimation optional\n")
		.def("__repr__",
			[](const fsdk::IAttributeEstimator::EstimationResult &r) {
				return "AttributeResult: "
						"age = " + (r.age.valid() ? std::to_string(r.age.value()) : std::string("not requested!\n")) +
						" gender = " + (r.gender.valid() ? std::to_string(r.gender.value()) : std::string("not requested!\n")) +
						" genderScore = " + (r.genderScore.valid() ? std::to_string(r.genderScore.value()) : std::string("not requested!\n")) +
						" ethnicity = " + (r.ethnicity.valid() ? std::to_string(static_cast<int>(r.ethnicity.value().getPredominantEthnicity())) : std::string("not requested!\n"))
						; })
		;
	
	py::class_<fsdk::IMedicalMaskEstimatorPtr>(f, "IMedicalMaskEstimatorPtr",
		"Medical mask estimator interface.\n"
		"\tStores flags that indicates which medical mask feature is present; \n"
		"\tProbability scores are defined in [0,1] range.\n")
		
		.def("estimate", [](
				const fsdk::IMedicalMaskEstimatorPtr& est,
				const fsdk::Image& warp) {
				fsdk::MedicalMaskEstimation estimation{};
				fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, estimation);
				return std::make_tuple(FSDKErrorResult(err), estimation); },
			"Estimate Medical Mask probabilities..\n"
			"\t\t(see FSDKErrorResult for details)\n"
			"\tArgs:\n"
			"\t\tparam1 (Image): image with warped face. Format must be R8G8B8\n"
			"\tReturns:\n"
			"\t\t(tuple): \n"
			"\t\t\t tuple with FSDKErrorResult code and MedicalMaskEstimation\n")
			
		.def("estimate", [](
				const fsdk::IMedicalMaskEstimatorPtr& est,
				const fsdk::Image &image,
				const fsdk::BaseDetection<float>& detection) {
				fsdk::MedicalMaskEstimation estimation{};
				fsdk::Result<fsdk::FSDKError> err = est->estimate(image, detection, estimation);
				return std::make_tuple(FSDKErrorResult(err), estimation); },
			"Estimate Medical Mask probabilities..\n"
			"\t\t(see FSDKErrorResult for details)\n"
			"\tArgs:\n"
			"\t\tparam1 (Image): image with warped face. Format must be R8G8B8\n"
			"\t\tparam2 (Detection): detection coords in image space;\n"
			"\tReturns:\n"
			"\t\t(tuple): \n"
			"\t\t\t tuple with FSDKErrorResult code and MedicalMaskEstimation\n")
			
		.def("estimate", [](
				const fsdk::IMedicalMaskEstimatorPtr& est,
				const std::vector<fsdk::Image>& warps) {
				std::vector<fsdk::MedicalMaskEstimation> estimations(warps.size());
				fsdk::Result<fsdk::FSDKError>  err = est->estimate(warps, estimations);
				if (err.isOk())
					return std::make_tuple(FSDKErrorResult(err), estimations);
				else
					return std::make_tuple(FSDKErrorResult(err),
						std::vector<fsdk::MedicalMaskEstimation>()); },
			"Estimate Medical Mask probabilities.\n"
			"\t\t(see FSDKErrorResult for details)\n"
			"\tArgs:\n"
			"\t\tparam1 (list of Images): list of warped images, format of images must be R8G8B8. Must be warped!\n"
			"\t\tparam2 (estimations): request with flags to check parameters to estimate\n"
			"\tReturns:\n"
			"\t\t(tuple): \n"
			"\t\t\t tuple with FSDKErrorResult code and list of MedicalMaskEstimation estimations\n")
			
		.def("estimate", [](
				const fsdk::IMedicalMaskEstimatorPtr& est,
				const std::vector<fsdk::Image>& images,
				const std::vector<fsdk::BaseDetection<float>> detections) {
				std::vector<fsdk::BaseDetection<int>> detectionsInt(detections.size());
				for (uint32_t i = 0; i < detections.size(); ++i) {
					detectionsInt[i] = fsdk::Detection(detections[i]);
				}
				std::vector<fsdk::MedicalMaskEstimation> estimations(images.size());
				fsdk::Result<fsdk::FSDKError>  err = est->estimate(images, detectionsInt, estimations);
				if (err.isOk())
					return std::make_tuple(FSDKErrorResult(err), estimations);
				else
					return std::make_tuple(FSDKErrorResult(err),
						std::vector<fsdk::MedicalMaskEstimation>()); },
			"Estimate Medical Mask probabilities.\n"
			"\t\t(see FSDKErrorResult for details)\n"
			"\tArgs:\n"
			"\t\tparam1 (list of Images): list of Images. Format must be R8G8B8\n"
			"\t\tparam1 (list of Detections): list of face detections.\n"
			"\tReturns:\n"
			"\t\t(tuple): \n"
			"\t\t\t tuple with FSDKErrorResult code and list of MedicalMaskEstimation estimations\n")
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
				return std::make_tuple(FSDKErrorResult(err), out);
			})
	
		.def("estimate",[](
			const fsdk::IHeadPoseEstimatorPtr& est,
			const fsdk::Image& image,
			// cast to detection<int> inside c++ interface
			const fsdk::BaseDetection<float>& detection) {
				fsdk::HeadPoseEstimation out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(image, detection, out);
				return std::make_tuple(FSDKErrorResult(err), out);
			},
			"Estimate the angles.\n"
			"\tArgs\n"
			"\t\tparam1 (Image): image source image. Format must be R8G8B8.\n"
			"\t\tparam2 (Detection): detection.\n"
			"\tReturns:\n"
			"\t\t(tuple): tuple with error code FSDKErrorResult and output EthnicityEstimation\n")
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
				return std::make_tuple(FSDKErrorResult(err), outIsGrayscale);
			},
			"Check if image is grayscale or colo.\n"
			"\tArgs\n"
			"\t\tparam1 (Image): image source image. Format must be R8G8B8.\n"
			"\tReturns:\n"
			"\t\t(tuple): estimation result and bool answer (true if image is grayscale, false if not).\n")
			;
	
	py::class_<fsdk::ILivenessDepthEstimatorPtr>(f, "ILivenessDepthEstimatorPtr",
			"Depth estimator interface.\n"
			"\tThis estimator is designed for face analysis using depth map. "
			"It works with 16 bit depth map of face warp.\n"
			"\tSee IWarper for details")
		.def("estimate",[](
			const fsdk::ILivenessDepthEstimatorPtr& est,
			const fsdk::Image& image) {
				fsdk::DepthEstimation out = {};
				fsdk::Result<fsdk::FSDKError> err = est->estimate(image, out);
				return std::make_tuple(FSDKErrorResult(err), out); 
			},
			"Check whether or not depth map corresponds to the real person.\n"
			"\tArgs\n"
			"\t\tparam1 (Image): warped depth image with R16 format.\n"
			"\tReturns:\n"
			"\t\t(tuple): tuple with error code and fsdk::DepthEstimation output structure.\n"
			"\t\t\tEstimation score normalized between 0.0 and 1.0,\n"
			"\t\t\twhere 1.0 equals to 100% confidence that person on image is alive, and 0.0 equals to 0%.\n")
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
				return std::make_tuple(FSDKErrorResult(err), irEstimation);
			},
			"Check whether or not infrared warp corresponds to the real person.\n"
			"\tArgs\n"
			"\t\tparam1 (Image): irWarp infra red face warp\n"
			"\tReturns:\n"
			"\t\t(tuple):  tuple with Error code and irEstimation\n")
				;
	
	py::class_<fsdk::ILivenessFlyingFacesEstimatorPtr>(f, "ILivenessFlyingFacesEstimatorPtr",
		"Flying faces liveness estimator interface.\n"
		"\t\tThis estimator helps determine whether a person is real or not.\n")
		
		.def("estimate",[](
				const fsdk::ILivenessFlyingFacesEstimatorPtr& est,
				const fsdk::Face& face) {
					fsdk::LivenessFlyingFacesEstimation estimation = {};
					fsdk::Result<fsdk::FSDKError> err = est->estimate(face, estimation);
					return std::make_tuple(FSDKErrorResult(err), estimation);
			},
			"Checks whether or not detection corresponds to the real person.\n"
			"\tArgs\n"
			"\t\tparam1 (Face): Face with valid input image and Detection. Image format must be R8G8B8.\n"
			"\tReturns:\n"
			"\t\t(tuple): tuple with Error code and LivenessFlyingFacesEstimation.\n")
		.def("estimate",[](
				const fsdk::ILivenessFlyingFacesEstimatorPtr& est,
				const std::vector<fsdk::Face>& faces) {
					std::vector<fsdk::LivenessFlyingFacesEstimation> out(faces.size());
					auto scoreSpan = fsdk::Span<fsdk::LivenessFlyingFacesEstimation>(out.data(), out.size());
					fsdk::Result<fsdk::FSDKError> err = est->estimate(
						fsdk::Span<const fsdk::Face>(faces.data(), faces.size()),
						scoreSpan);
					return std::make_tuple(FSDKErrorResult(err), out);
			},
			"Checks whether or not detections corresponds to the real persons.\n"
			"\tArgs\n"
			"\t\tparam1 (Faces): List of Faces with valid Images and corresponding Detections.\n"
			"\t\t\tImage format must be R8G8B8.\n"
			"\tReturns:\n"
			"\t\t(tuple): tuple with Error code and list of LivenessFlyingFacesEstimations.\n")
		;

	py::class_<fsdk::ILivenessRGBMEstimatorPtr>(f, "ILivenessRGBMEstimatorPtr",
		"LivenessRGBMEstimator interface.\n"
		"\t\tThis estimator helps determine whether a person is real or not.\n")

		.def("update", [](
			const fsdk::ILivenessRGBMEstimatorPtr& est,
			const fsdk::Image& image,
			uint32_t iFrame,
			const fsdk::Image& background) {
				fsdk::Image resultBackground = background;
				auto result = est->update(
						image,
						iFrame,
						resultBackground
					);
				return std::make_tuple(FSDKErrorResult(result), resultBackground);
			},
			"Prepare background method. Pass here every frame from the stream to extract the background\n"
			"\tArgs\n"
			"\t\tparam1 (Image): current frame\n"
			"\t\t\tImage format must be R8G8B8.\n"
			"\t\tparam2 (uint32_t): current frame number\n"
			"\t\tparam3 (Image): current accumulated background (empty for the first call and result of the previous calls for next calls\n"
			"\tReturns:\n"
			"\t\t(tuple): tuple with Error code and accumulated background\n")

		.def("estimate", [](
			const fsdk::ILivenessRGBMEstimatorPtr& est,
			const fsdk::Image& image,
			// cast to detection<int> inside c++ interface
			const fsdk::BaseDetection<float>& detection,
			const fsdk::Image& background) {
				fsdk::LivenessRGBMEstimation estimation;
				auto result = est->estimate(
						image,
						detection,
						background,
						estimation
					);
				return std::make_tuple(FSDKErrorResult(result), estimation);
			},
			"Checks whether or not detections corresponds to the real persons.\n"
			"\tArgs\n"
			"\t\tparam1 (Image): source image\n"
			"\t\t\tImage format must be R8G8B8.\n"
			"\t\tparam2 (Detection): detection\n"
			"\t\tparam3 (Image): accumulated background.\n"
			"\tReturns:\n"
			"\t\t(tuple): tuple with Error code and LivenessRGBMEstimation structure\n")
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
			const std::vector<fsdk::Image>& frames) {
				double score = 0.0;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(small, frames.data(), (int)frames.size(), score);
				return std::make_tuple(FSDKErrorResult(err), score); },
			"Check if correct optical flow can be calculated from input images..\n"
			"\tArgs\n"
			"\t\tparam1 (Image): small face crop\n"
			"\t\tparam2 (list): list with big face crops.\n"
			"\tReturns:\n"
			"\t\t(tuple): tuple with error code and float score.\n")
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
				const fsdk::EyeCropper::EyesRects& eyeRects) {
				fsdk::EyesEstimation out;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, eyeRects, out);
				return std::make_tuple(FSDKErrorResult(err), out);
			},
			"Estimate the attributes.\n"
				"\tArgs\n"
				"\t\tparam1 (Image): warp source image. Format must be R8G8B8. Must be warped!\n"
				"\t\tparam2 (EyeRects): Cropped rects.\n"
				"\tReturns:\n"
				"\t\t(tuple): returns error code FSDKErrorResult and EyesEstimation\n")
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
				return std::make_tuple(FSDKErrorResult(err), out);
			},
			"\tEstimate the attributes.\n"
			"\tArgs\n"
			"\t\tparam1 (Image): warp source image. If format is not R8 it would be converted to R8. "
			"Must be warped!\n"
			"\tReturns:\n"
			"\t\t(tuple): returns error code FSDKErrorResult and EmotionsEstimation\n")
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
				const fsdk::Image& warp,
				const fsdk::Landmarks5& landmarks5Transformed) {
				fsdk::GazeEstimation outEyeAngles;
				fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, landmarks5Transformed, outEyeAngles);
				return std::make_tuple(FSDKErrorResult(err), outEyeAngles);
			},
			"Estimate the eye angles.\n"
			"\tArgs\n"
			"\t\tparam1 (Image): Warped Image.\n"
			"\t\tparam2 (Landmarks5): Origin Landmarks5 got from face detector.\n"
			"\t\tparam3 (Landmarks5): Transformed Landmarks5 got from warper. See Warper.\n"
			"\tReturns:\n"
			"\t\t(tuple): returns error code FSDKErrorResult and GazeEstimation\n")
		.def("getFaceCenter",[](
				const fsdk::IGazeEstimatorPtr& est,
				const fsdk::Landmarks5& landmarks5) {
				return est->getFaceCenter(landmarks5);
			},
			"Estimate the point for calculating of gaze projection.\n"
			"\tArgs\n"
			"\t\tparam1 (Landmarks5): Transformed Landmarks5 got from warper. See Warper.\n"
			"\tReturns:\n"
			"\t\t(Point): point for calculating of gaze projection\n")
			;
	
	py::class_<fsdk::IAGSEstimatorPtr>(f, "IAGSEstimatorPtr",
		"Approximate Garbage Score estimator interface.\n"
		"\tThis estimator is designed to work with Image and detection.\n")
			.def("estimate",[](
				const fsdk::IAGSEstimatorPtr& est,
				const fsdk::Image& image,
					// cast to detection<int> inside c++ interface
					const fsdk::BaseDetection<float>& detection) {
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
	
	py::class_<fsdk::IGlassesEstimatorPtr>(f, "IGlassesEstimatorPtr",
		"Glasses estimator interface.\n"
		"\tThis estimator is designed to work with a person face image.\n"
		"\tsee IWarper for details.\n")
		.def("estimate",[](
			const fsdk::IGlassesEstimatorPtr& est,
			const fsdk::Image& warp) {
				fsdk::ResultValue<fsdk::FSDKError, fsdk::GlassesEstimation> err = est->estimate(warp);
				if (err.isOk())
					return std::make_tuple(FSDKErrorResult(err), err.getValue());
				return std::make_tuple(FSDKErrorResult(err), fsdk::GlassesEstimation::EstimationError);
				
			},
			"\tChecks whether person wearing any glasses or not.\n"
			"\tArgs\n"
			"\t\tparam1 (Image): warped source image in R8G8B8 format.\n"
			"\tReturns:\n"
			"\t\t(tuple): returns error code FSDKErrorResult and GlassesEstimation\n")
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
			"\tFace detector is capable of face landmarks detection. "
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
		
		.def("__setitem__", [](
			fsdk::EyesEstimation::EyeAttributes::IrisLandmarks &s,
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
		"\tFace detector is capable of face landmarks detection. "
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

//	Quality
	py::class_<fsdk::Quality>(f, "Quality",
		"Quality estimation structure\n"
		"\tEach estimation is given in normalized [0, 1] range. Parameter meanings:\n"
		"\t\t light: image overlighting degree. 1 - ok, 0 - overlighted;\n"
		"\t\t dark: image darkness degree. 1 - ok, 0 - too dark;\n"
		"\t\t gray: image grayness degree 1 - ok, 0 - too gray;\n"
		"\t\t blur: image blur degree. 1 - ok, 0 - too blured.\n")
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
		.def("getQuality", &fsdk::Quality::getQuality, "\tcomplex estimation of quality (minimin of flor estimations). 0 - low quality, 1 - high quality.\n")
		;

//	Quality
	py::class_<fsdk::SubjectiveQuality>(f, "SubjectiveQuality",
		"SubjectiveQuality estimation structure\n"
		"\tEach estimation is given in normalized [0, 1] range. Parameter meanings:\n"
		"\t\tblur: image blur degree. 1 - ok, 0 - too blured;\n"
		"\t\tlight: image brightness degree. 1 - ok, 0 - too bright;\n"
		"\t\tdark: image darkness degree. 1 - ok, 0 - too dark;\n"
		"\t\tillumination: image illumination uniformity degree. 1 - ok, 0 - is too illuminated;\n"
		"\t\tspecularity: image specularity degree. 1 - ok, 0 - too specular;\n"
		"\t\tisBlurred image is blurred flag;\n"
		"\t\tisHighlighted image is overlighted flag;\n"
		"\t\tisDark image image is too dark flag;\n"
		"\t\tisIlluminated image is too illuminated flag;\n"
		"\t\tisNotSpecular image is not specular flag;\n")
		.def(py::init<>())
		.def_readwrite("blur", &fsdk::SubjectiveQuality::blur)
		.def_readwrite("light", &fsdk::SubjectiveQuality::light)
		.def_readwrite("darkness", &fsdk::SubjectiveQuality::darkness)
		.def_readwrite("illumination", &fsdk::SubjectiveQuality::illumination)
		.def_readwrite("specularity", &fsdk::SubjectiveQuality::specularity)
		.def_readwrite("isBlurred", &fsdk::SubjectiveQuality::isBlurred)
		.def_readwrite("isHighlighted", &fsdk::SubjectiveQuality::isHighlighted)
		.def_readwrite("isDark", &fsdk::SubjectiveQuality::isDark)
		.def_readwrite("isIlluminated", &fsdk::SubjectiveQuality::isIlluminated)
		.def_readwrite("isNotSpecular", &fsdk::SubjectiveQuality::isNotSpecular)
		.def("__repr__",
			[](const fsdk::SubjectiveQuality &q) {
				return "SubjectiveQuality: "
						"blur = " + std::to_string(q.blur)
						+ ", light = " + std::to_string(q.light)
						+ ", darkness = " + std::to_string(q.darkness)
						+ ", illumination = " + std::to_string(q.illumination)
						+ ", specularity = " + std::to_string(q.specularity)
						+ ", isBlurred = " + std::to_string(q.isBlurred)
						+ ", isHighlighted = " + std::to_string(q.isHighlighted)
						+ ", isDark = " + std::to_string(q.isDark)
						+ ", isIlluminated = " + std::to_string(q.isIlluminated)
						+ ", isNotSpecular = " + std::to_string(q.isNotSpecular);
			})
		.def("isGood", &fsdk::SubjectiveQuality::isGood, "\tIf all boolean flags are false returns true - high quality, else false - low quality.\n")
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
		.value("FrontalFace0", fsdk::HeadPoseEstimation::FrontalFace0, "\tNon-frontal face. \n")
		.value("FrontalFace1", fsdk::HeadPoseEstimation::FrontalFace1, "\tGood for recognition; Doesn't descrease recall and looks fine. \n")
		.value("FrontalFace2", fsdk::HeadPoseEstimation::FrontalFace2, "\tGOST/ISO angles \n")
			;
		
	py::class_<fsdk::DepthRange>(f, "DepthRange",
		"Depth range configuration structure in millimeters.\n"
		"\tSpecifies working range of distances for depth estimator.\n"
		"\tAverage depth map value should belong to this range.\n"
		"\tBy default configured for kinect depth sensor.\n")
	.def(py::init<>())
	.def_readwrite("min", &fsdk::DepthRange::min)
	.def_readwrite("max", &fsdk::DepthRange::max)
	.def("isOk", &fsdk::DepthRange::isOk)
	.def("__repr__",
		[](const fsdk::DepthRange &h) {
			return "DepthRange: "
					", min = " + std::to_string(h.min)
					+ ", max = " + std::to_string(h.max);
		})
		;

	py::class_<fsdk::DepthEstimation>(f, "DepthEstimation", "Depth estimator output structure\n")
	.def(py::init<>())
	.def(py::init<float, bool>())
	.def_readwrite("score", &fsdk::DepthEstimation::score)
	.def_readwrite("isReal", &fsdk::DepthEstimation::isReal)
	.def("__repr__",
		[](const fsdk::DepthEstimation &d) {
			return "DepthEstimation: "
					" score = " + std::to_string(d.score) +
					", isReal = " + std::to_string(d.isReal);
		});
	
	// EyesEstimation
	py::class_<fsdk::EyesEstimation>(f, "EyesEstimation",
		"Eyes estimation output.\n"
		"\tThese values are produced by IEyeEstimator object.\n")
		.def(py::init<>())
		.def_readwrite("leftEye", &fsdk::EyesEstimation::leftEye)
		.def_readwrite("rightEye", &fsdk::EyesEstimation::rightEye)
			;
	
	// EyesCropper
	py::class_<fsdk::EyeCropper>(f, "EyeCropper",
		"EyeCropper is a helper structure for IEyeEstimator interfacet.\n"
			"\tMethods of this structure crop an input warped image "
			"and returns rectangle coordinates of each eye.\n")
		.def(py::init<>())
		.def("cropByLandmarks5", &fsdk::EyeCropper::cropByLandmarks5, "Crop rects by Landmarks5\n")
		.def("cropByLandmarks68", &fsdk::EyeCropper::cropByLandmarks68, "Crop rects by Landmarks68\n")
		;
	
	py::class_<fsdk::EyeCropper::EyesRects>(f, "EyesRects")
		.def_readwrite("leftEyeRect", &fsdk::EyeCropper::EyesRects::leftEyeRect)
		.def_readwrite("rightEyeRect", &fsdk::EyeCropper::EyesRects::rightEyeRect)
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
		;
	
	// Gaze
	py::class_<fsdk::GazeEstimation>(f, "GazeEstimation",
			"Gaze estimation output.\n"
			"\tThese values are produced by IGazeEstimatorPtr object.\n"
			"\tEach angle is measured in degrees and in [-180, 180] range.\n")
		.def(py::init<>())
		.def_readwrite("yaw", &fsdk::GazeEstimation::yaw)
		.def_readwrite("pitch", &fsdk::GazeEstimation::pitch)
		.def("__repr__",
			[](const fsdk::GazeEstimation &g) {
				return "GazeEstimation: "
						" yaw = " + std::to_string(g.yaw) +
						", pitch = " + std::to_string(g.pitch);
			})
		;
	
	// LivenessFlyingFaces
	py::class_<fsdk::LivenessFlyingFacesEstimation>(f, "LivenessFlyingFacesEstimation",
			"LivenessFlyingFaces estimation output.\n"
			"\tThese values are produced by ILivenessFlyingFacesEstimatorPtr object.\n"
			"\tScore is returned in range [0, 1), 1 - is maximum and real, 0 - is minimum and not real, "
			"\tisReal - is boolean answer, true - person is real, false - fake.\n")
		.def(py::init<>())
		.def_readwrite("score", &fsdk::LivenessFlyingFacesEstimation::score, "\tscore in range [0,1]\n")
		.def_readwrite("isReal", &fsdk::LivenessFlyingFacesEstimation::isReal, "\tis real person or not\n")
		.def("__repr__",
			 [](const fsdk::LivenessFlyingFacesEstimation &est) {
				 return "LivenessFlyingFacesEstimation: "
						" score = " + std::to_string(est.score) +
						", isReal = " + std::to_string(est.isReal);
			 })
		;

	// LivenessRGBMEstimation
	py::class_<fsdk::LivenessRGBMEstimation>(f, "LivenessRGBMEstimation",
			"LivenessRGBMEstimator estimation output.\n"
			"\tThese values are produced by ILivenessRGBMEstimatorPtr object.\n"
			"\tScore is returned in range [0, 1), 1 - is maximum and real, 0 - is minimum and not real, "
			"\tisReal - is boolean answer, true - person is real, false - fake.\n")
		.def(py::init<>())
		.def_readwrite("score", &fsdk::LivenessRGBMEstimation::score, "\tscore in range [0,1]\n")
		.def_readwrite("isReal", &fsdk::LivenessRGBMEstimation::isReal, "\tis real person or not\n")
		.def("__repr__",
			 [](const fsdk::LivenessRGBMEstimation &est) {
				 return "LivenessRGBMEstimation: "
						" score = " + std::to_string(est.score) +
						", isReal = " + std::to_string(est.isReal);
			 })
		;

	//	Ethnicity
	py::enum_<fsdk::EthnicityEstimation::Ethnicities>(f, "Ethnicity", "Ethnicity enum.\n")
		.value("AfricanAmerican", fsdk::EthnicityEstimation::AfricanAmerican)
		.value("Indian", fsdk::EthnicityEstimation::Indian)
		.value("Asian", fsdk::EthnicityEstimation::Asian)
		.value("Caucasian", fsdk::EthnicityEstimation::Caucasian)
		.export_values()
		;
	
	//	Glasses
	py::enum_<fsdk::GlassesEstimation>(f, "GlassesEstimation", py::arithmetic(), "GlassesEstimation enum\n")
		.value("NoGlasses", fsdk::GlassesEstimation::NoGlasses, "Person is not wearing glasses\n")
		.value("EyeGlasses", fsdk::GlassesEstimation::EyeGlasses, "Person is wearing eyeglasses\n")
		.value("SunGlasses", fsdk::GlassesEstimation::SunGlasses, "Person is wearing sunglasses\n")
		.value("EstimationError", fsdk::GlassesEstimation::EstimationError, "Failed to estimate\n")
		.export_values()
		;

	//	Mouth
	py::class_<fsdk::MouthEstimation>(f, "MouthEstimation",
		"MouthEstimatorPtr output structure\n"
		"\tStores flags that indicates which mouth feature is present.\n"
		"\tProbability scores are defined in [0,1] range.\n")
		.def_readwrite("opened", &fsdk::MouthEstimation::opened, "Mouth opened score\n")
		.def_readwrite("smile", &fsdk::MouthEstimation::smile, "Person is smiling score\n")
		.def_readwrite("occluded", &fsdk::MouthEstimation::occluded, "Mouth is occluded score\n")
		.def_readwrite("isOpened", &fsdk::MouthEstimation::isOpened, "Mouth is opened flag\n")
		.def_readwrite("isSmiling", &fsdk::MouthEstimation::isSmiling, "Person is smiling flag\n")
		.def_readwrite("isOccluded", &fsdk::MouthEstimation::isOccluded, "Mouth is occluded flag\n")
		.def("__repr__", [](const fsdk::MouthEstimation& e) {
				return "Mouth State: \n"
					"opened score = " + std::to_string(e.opened) + "\n" +
					"smile score = " + std::to_string(e.smile) + "\n" +
					"occlusion score = " + std::to_string(e.occluded) + "\n" +
					"isOpened = " + std::string(e.isOpened ? "True\n" : "False\n") +
					"isSmiling = " + std::string(e.isSmiling ? "True\n" : "False\n") +
					"isOccluded = " + std::string(e.isOccluded ? "True\n" : "False\n");
			})
		;
	
	//	MedicalMask
	py::enum_<fsdk::MedicalMask>(f, "MedicalMask", py::arithmetic(),
		"MedicalMask estimator output enumeration.\n")
		.value("Mask", fsdk::MedicalMask::Mask, "Medical mask is on the face\n")
		.value("NoMask", fsdk::MedicalMask::NoMask, "No medical mask on the face\n")
		.value("OccludedFace", fsdk::MedicalMask::OccludedFace, "Face is occluded by something\n")
		.export_values();
		;

	py::class_<fsdk::MedicalMaskEstimation>(f, "MedicalMaskEstimation",
		"MedicalMaskEstimator output structure\n"
		"\tResult enumeration with the medical status.\n"
		"\tProbability scores are defined in [0,1] range.\n")
		.def_readwrite("result", &fsdk::MedicalMaskEstimation::result, "Estimation result\n")
		.def_readwrite("maskScore", &fsdk::MedicalMaskEstimation::maskScore, "Medical mask is on the face score\n")
		.def_readwrite("noMaskScore", &fsdk::MedicalMaskEstimation::noMaskScore, "No medical mask on the face score\n")
		.def_readwrite("occludedFaceScore", &fsdk::MedicalMaskEstimation::occludedFaceScore, "Face is occluded by something score\n")
		;
	
	py::class_<fsdk::OverlapEstimation>(f, "OverlapEstimation", "Face overlap estimation output.\n")
		.def_readwrite("overlapValue", &fsdk::OverlapEstimation::overlapValue, "Mouth opened score\n")
		.def_readwrite("overlapped", &fsdk::OverlapEstimation::overlapped, "Person is smiling score\n")
		.def("__repr__", [](const fsdk::OverlapEstimation& e) {
			return "OverlapEstimation: \n"
				"overlapValue = " + std::to_string(e.overlapValue) + "\n" +
				"isOpened = " + std::string(e.overlapped ? "True\n" : "False\n");
			})
		;

	py::class_<fsdk::IMouthEstimatorPtr>(f, "IMouthEstimatorPtr",
		"Mouth estimator interface.\n"
		"\tPredicts person's mouth state.\n")
		.def("estimate", [](
			const fsdk::IMouthEstimatorPtr& estimator,
			const fsdk::Image& warp) {
			fsdk::MouthEstimation out = {};
			fsdk::Result<fsdk::FSDKError> status = estimator->estimate(warp, out);
			return std::make_tuple(FSDKErrorResult(status), out);
			},
			"\tEstimates MouthEstimation probabilities.\n"
			"\tArgs\n"
			"\t\tparam1 (warp): warped source image in R8G8B8 format.\n"
			"\tReturns:\n"
			"\t\t(tuple): returns error code FSDKErrorResult and MouthEstimation\n")
		;
	
	py::class_<fsdk::IOverlapEstimatorPtr>(f, "IOverlapEstimatorPtr",
		"Overlap estimator interface.\n"
		"\tEstimates the face overlap.\n")
		.def("estimate", [] (
			const fsdk::IOverlapEstimatorPtr& estimator,
			const fsdk::Image& image,
			const fsdk::BaseDetection<float>& detection) {
				fsdk::OverlapEstimation out = {};
				fsdk::Result<fsdk::FSDKError> status = estimator->estimate(image, detection, out);
				return std::make_tuple(FSDKErrorResult(status), out);
			},
			"\tEstimates the face overlap.\n"
			"\tArgs\n"
			"\t\tparam1 (image): image source image in R8G8B8 format.\n"
			"\t\tparam2 (detection): detection coords in image space.\n"
			"\tReturns:\n"
			"\t\t(tuple): returns error code FSDKErrorResult and OverlapEstimation\n")
		;
}
