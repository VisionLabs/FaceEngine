#include <lsdk/LivenessEngine.h>
#include <pybind11/pybind11.h>
#include <fsdk/FaceEngine.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>
#include "LivenessEngineAdapter.hpp"

namespace py = pybind11;


void liveness_module(py::module& f) {
	
	py::class_<PyILivenessEngine>(f, "PyILivenessEngine", "Root Liveness object interface\n")
		.def("createLiveness", &PyILivenessEngine::createLiveness, "liveness object if succeeded, nullptr if failed.\n")
		.def("setSettingsProvider", &PyILivenessEngine::setSettingsProvider, "\n")
//		.def("getSettingsProvider", &PyILivenessEngine::getSettingsProvider, "\n")
		.def("setDataDirectory", &PyILivenessEngine::setDataDirectory, "\n")
		.def("getDataDirectory", &PyILivenessEngine::getDataDirectory, "\n")
			; // PyILivenessEngine
	
	py::enum_<lsdk::LivenessAlgorithmType>(f, "LivenessAlgorithmType", py::arithmetic(), "Liveness type enumeration.\n")
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
	
	py::class_<lsdk::ILivenessEnginePtr>(f, "LivenessEngine");
	py::class_<lsdk::ILivenessPtr>(f, "Liveness");

	
}