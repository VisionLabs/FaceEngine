#include <ErrorsAdapter.hpp>

#include <fsdk/FaceEngine.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>

namespace py = pybind11;

void warper_module(py::module& f) {

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
	
	.def("unwarp",[](
			const fsdk::IWarperPtr& warper,
			const fsdk::GazeEstimation& eyeAngles,
			const fsdk::Transformation& transformation) {
			fsdk::GazeEstimation outEyeAngles;
			fsdk::Result<fsdk::FSDKError> error = warper->unwarp(eyeAngles, transformation, outEyeAngles);
			if (error.isOk())
				return std::make_tuple(FSDKErrorResult(error), outEyeAngles);
			else
				return std::make_tuple(FSDKErrorResult(error), fsdk::GazeEstimation()); },
		"Warp landmarks of size 68\n"
		"\tArgs:\n"
		"\t\tparam1 (Landmarks68): landmarks array of size 68\n"
		"\t\tparam2 (Transformation): transformation data\n"
		"\tReturns:\n"
		"\t\t(tuple): tuple with FSDKErrorResult and transformed landmarks68\n")
		
	.def("createTransformation",[](
		const fsdk::IWarperPtr& warper,
		// cast to detection<int> inside c++ interface
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
	.def_readwrite("isValid", &fsdk::Transformation::isValid)
	.def("__repr__", [](const fsdk::Transformation &t) {
		return "Transformation: "
			" angleDeg = " + std::to_string(t.angleDeg)
			+ ", scale = " + std::to_string(t.scale)
			+ ", centerP: x = " + std::to_string(t.centerP.x) + " y = " + std::to_string(t.centerP.y)
			+ ", detectionTopLeft: x = " + std::to_string(t.detectionTopLeft.x)
			+ " y = " + std::to_string(t.detectionTopLeft.y);
	})
	;

py::class_<fsdk::IHumanWarperPtr>(f, "IHumanWarperPtr",
	"Human detection area warper interface.\n"
	"\t Perform cropping and resize of an image to size (192x384) for human descriptor extraction.\n")
	.def("warp",[](
			const fsdk::IHumanWarperPtr& warper,
			const fsdk::Image& image,
			const fsdk::Detection& detection) {
				fsdk::Image transformedImage;
				fsdk::Result<fsdk::FSDKError> error = warper->warp(image, detection, transformedImage);
				if (error.isOk())
					 return std::make_tuple(FSDKErrorResult(error), transformedImage);
				 else
					 return std::make_tuple(FSDKErrorResult(error), fsdk::Image()); },
			 "Warp image\n"
			 "\tArgs:\n"
			 "\t\tparam1 (Image): image to make warping. The format of image must be R8G8B8.\n"
			 "\t\tparam2 (Detection): human detection.\n"
			 "\tReturns:\n"
			 "\t\t(tuple): tuple with FSDKError and output transformed image, size: width = 192, height = 384\n")
	;

}
