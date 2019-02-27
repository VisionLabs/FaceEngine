#include <lsdk/LivenessEngine.h>
#include <pybind11/pybind11.h>
#include <fsdk/FaceEngine.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>
#include "LivenessEngineAdapter.hpp"
#include "ErrorsAdapter.hpp"

namespace py = pybind11;


void liveness_module(py::module& f) {
	
	py::class_<PyILivenessEngine>(f, "PyILivenessEngine", "Root Liveness object interface\n")
		.def("createLiveness", &PyILivenessEngine::createLiveness, "liveness object if succeeded, nullptr if failed.\n")
		.def("setSettingsProvider", &PyILivenessEngine::setSettingsProvider, "\n")
//		.def("getSettingsProvider", &PyILivenessEngine::getSettingsProvider, "\n")
		.def("setDataDirectory", &PyILivenessEngine::setDataDirectory, "\n")
		.def("getDataDirectory", &PyILivenessEngine::getDataDirectory, "\n")
		.def("createComplexLiveness", &PyILivenessEngine::createComplexLiveness, "\n")
			; // PyILivenessEngine
	
	py::enum_<lsdk::LivenessAlgorithmType>(f, "LivenessType",
			"Liveness type enumeration.\n")
		.value("LA_PITCH_DOWN", lsdk::LA_PITCH_DOWN, "Algorithm based on downward face movement.\n")
		.value("LA_PITCH_UP", lsdk::LA_PITCH_UP, "Algorithm based on upward face movement.\n")
		.value("LA_YAW_LEFT", lsdk::LA_YAW_LEFT, "Algorithm based on leftward face movement.\n")
		.value("LA_YAW_RIGHT", lsdk::LA_YAW_RIGHT, "Algorithm based on rightward face movement.\n")
		.value("LA_SMILE", lsdk::LA_SMILE, " Algorithm based on smile.\n")
		.value("LA_MOUTH", lsdk::LA_MOUTH, "Algorithm based on mouth opening..\n")
		.value("LA_EYEBROW", lsdk::LA_EYEBROW, "Algorithm based on eyebrows rising.\n")
		.value("LA_EYE", lsdk::LA_EYE, "Algorithm based on eye blinking.\n")
		.value("LA_ZOOM", lsdk::LA_ZOOM, "Algorithm based on camera zooming.\n")
		.value("LA_INFRARED", lsdk::LA_INFRARED, "ALgorithm based on infrared warp analisys.\n")
		.value("LA_COUNT", lsdk::LA_COUNT, "Not a type; counts number of available types.\n")
		.export_values();
			;
	
	py::enum_<lsdk::ComplexLivenessAlgorithmType>(f, "ComplexLivenessType", py::arithmetic(),
			"Liveness type enumeration.\n")
		.value("CLA_DEPTH", lsdk::CLA_DEPTH, "Algorithm based on depth map analysis.\n")
		.value("CLA_COUNT", lsdk::CLA_COUNT, "Not a type; counts number of available types.\n")
		.export_values();
			;
	
	py::enum_<lsdk::LSDKError>(f, "LSDKError", "Liveness type enumeration.\n")
		.value("Ok", lsdk::LSDKError::Ok, "Ok.\n")
		.value("NotInitialized", lsdk::LSDKError::NotInitialized,
			"Liveness not initialized..\n")
		.value("NotReady", lsdk::LSDKError::NotReady,
			"Liveness not ready, require more updates.\n")
		.value("PreconditionFailed", lsdk::LSDKError::PreconditionFailed,
			"Starting condition is not accomplished, liveness not started yet..\n")
		.value("Internal", lsdk::LSDKError::Internal, "Internal error.\n")
		.export_values();
			;
	
	py::class_<lsdk::Angles>(f, "Angles")
		.def(py::init<>())
		.def_readwrite("left", &lsdk::Angles::yaw)
		.def_readwrite("pitch", &lsdk::Angles::pitch)
		.def_readwrite("roll", &lsdk::Angles::roll)
		.def("__repr__",
			[](const lsdk::Angles &h) {
				std::ostringstream ss;
				ss << "Angles: pitch = " << h.pitch
					<< ", yaw = " << h.yaw
					<< ", roll = " << h.roll;
				return ss.str();
			})
			;
	
	py::class_<lsdk::Scores>(f, "Scores")
		.def(py::init<>())
		.def_readwrite("smile", &lsdk::Scores::smile)
		.def_readwrite("mouth", &lsdk::Scores::mouth)
		.def_readwrite("eyebrow", &lsdk::Scores::eyebrow)
		.def("__repr__",
			[](const lsdk::Scores &h) {
				std::ostringstream ss;
				ss << "Scores: smile = " << h.smile
					<< ", mouth = " << h.mouth
					<< ", eyebrow = " << h.eyebrow;
				return ss.str();
			})
		;
			;
	
	py::class_<lsdk::EyeStates>(f, "EyeStates")
		.def(py::init<>())
		.def_readwrite("left", &lsdk::EyeStates::left)
		.def_readwrite("right", &lsdk::EyeStates::right)
		.def("__repr__",
			[](const lsdk::EyeStates &h) {
				std::ostringstream ss;
				ss << "EyeStates: left = " << h.left
					<< ", right = " << h.right;
				return ss.str();
			})
			;
	
	py::class_<lsdk::ILivenessPtr>(f, "Liveness")
	.def("update", [](
			const lsdk::ILivenessPtr& livenessPtr,
			fsdk::Image &image) {
			fsdk::ResultValue<lsdk::LSDKError, bool> err = livenessPtr->update(image);
		if (err.isOk())
			 return std::make_tuple(LSDKErrorResult(err), err.getValue());
		else
			return std::make_tuple(LSDKErrorResult(err), false);},
		 "")
	.def("reset", [](const lsdk::ILivenessPtr& livenessPtr){
		livenessPtr->reset();
	})
	.def("getDetection", [](const lsdk::ILivenessPtr& livenessPtr) {
		fsdk::Detection detection;
		bool success = livenessPtr->getDetection(&detection);
		return py::make_tuple(success, detection);
	})
	.def("getWarp", [](const lsdk::ILivenessPtr& livenessPtr) {
		fsdk::Image warp;
		bool success = livenessPtr->getWarp(&warp);
		return py::make_tuple(success, warp);
			;
	})
	.def("getLandmarks68", [](const lsdk::ILivenessPtr& livenessPtr) {
		fsdk::Landmarks68 landmarks68;
		bool success = livenessPtr->getLandmarks68(&landmarks68);
		return py::make_tuple(success, landmarks68);
	})
	.def("getLandmarks5", [](const lsdk::ILivenessPtr& livenessPtr) {
		fsdk::Landmarks5 landmarks5;
		bool success = livenessPtr->getLandmarks5(&landmarks5);
		return py::make_tuple(success, landmarks5);
	})
	.def("getIrisLandmarks", [](const lsdk::ILivenessPtr& livenessPtr) {
		fsdk::Landmarks<32> irisLandmarks;
		bool success = livenessPtr->getIrisLandmarks(&irisLandmarks);
		return py::make_tuple(success, irisLandmarks);
	})
	.def("getAngles", [](const lsdk::ILivenessPtr& livenessPtr) {
		lsdk::Angles angles;
		bool success = livenessPtr->getAngles(&angles);
		return py::make_tuple(success, angles);
	})
	.def("getScores", [](const lsdk::ILivenessPtr& livenessPtr) {
		lsdk::Scores scores;
		bool success = livenessPtr->getScores(&scores);
		return py::make_tuple(success, scores);
	})
	.def("getEyestates", [](const lsdk::ILivenessPtr& livenessPtr){
		lsdk::EyeStates eyeStates;
		bool success = livenessPtr->getEyestates(&eyeStates);
		return py::make_tuple(success, eyeStates);
	})
		;
	
	py::class_<lsdk::IComplexLivenessPtr>(f, "ComplexLiveness")
	.def("update", [](
			const lsdk::IComplexLivenessPtr& livenessPtr,
			fsdk::Image &rgb,
			fsdk::Image& map) {
			fsdk::ResultValue<lsdk::LSDKError, bool> err = livenessPtr->update(rgb, map);
			if (err.isOk())
				 return std::make_tuple(LSDKErrorResult(err), err.getValue());
			 else
				 return std::make_tuple(LSDKErrorResult(err), false);},
		 "")
	.def("reset", [](const lsdk::IComplexLivenessPtr& livenessPtr){
		livenessPtr->reset();
	})
	.def("getDetection", [](const lsdk::IComplexLivenessPtr& livenessPtr) {
		fsdk::Detection detection;
		bool success = livenessPtr->getDetection(&detection);
		return py::make_tuple(success, detection);
	})
	.def("getWarp", [](const lsdk::IComplexLivenessPtr& livenessPtr) {
		fsdk::Image warp;
		bool success = livenessPtr->getWarp(&warp);
		return py::make_tuple(success, warp);
		;
	})
	.def("getLandmarks68", [](const lsdk::IComplexLivenessPtr& livenessPtr) {
		fsdk::Landmarks68 landmarks68;
		bool success = livenessPtr->getLandmarks68(&landmarks68);
		return py::make_tuple(success, landmarks68);
	})
	.def("getLandmarks5", [](const lsdk::IComplexLivenessPtr& livenessPtr) {
		fsdk::Landmarks5 landmarks5;
		bool success = livenessPtr->getLandmarks5(&landmarks5);
		return py::make_tuple(success, landmarks5);
	})
	.def("getIrisLandmarks", [](const lsdk::IComplexLivenessPtr& livenessPtr) {
		std::vector<fsdk::Landmarks<32>> irisLandmarks(2);
		bool success = livenessPtr->getIrisLandmarks(irisLandmarks.data());
		return py::make_tuple(success, irisLandmarks);
	})
	.def("getAngles", [](const lsdk::IComplexLivenessPtr& livenessPtr) {
		lsdk::Angles angles;
		bool success = livenessPtr->getAngles(&angles);
		return py::make_tuple(success, angles);
	})
	.def("getScores", [](const lsdk::IComplexLivenessPtr& livenessPtr) {
		lsdk::Scores scores;
		bool success = livenessPtr->getScores(&scores);
		return py::make_tuple(success, scores);
	})
	.def("getEyestates", [](const lsdk::IComplexLivenessPtr& livenessPtr){
		lsdk::EyeStates eyeStates;
		bool success = livenessPtr->getEyestates(&eyeStates);
		return py::make_tuple(success, eyeStates);
	})
		;
	
}