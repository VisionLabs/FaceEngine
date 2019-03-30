#include <iostream>
#include <pybind11/pybind11.h>
#include <fsdk/FaceEngine.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/operators.h>
#include <pybind11/numpy.h>
#include "TrackEngineAdapter.hpp"
#include "../FaceEngineAdapter.hpp"
#include "TrackEngineCallback.hpp"

namespace py = pybind11;

PyITrackEngine createPyFaceEnginePtr(const PyIFaceEngine& fsdk, const std::string& configPath) {
	return PyITrackEngine{fsdk, configPath};
}

PYBIND11_MODULE(TrackEngine, t) {
	t.def("createTrackEngine", &createPyFaceEnginePtr, py::return_value_policy::take_ownership,
		  "Create TrackEngine",
		  "Create the TrackEngine object\n");


	py::class_<PyITrackEngine>(t, "PyITrackEngine", "Root LUNA SDK object interface\n")
	        .def("getStr", &PyITrackEngine::getStr,"fuck")
	        .def("getFSDKVer", &PyITrackEngine::getFSDKVer,"fuck2")
			.def("createStream", &PyITrackEngine::createStream, "fuck3");

	py::class_<PyIStream>(t, "PyIStream", "Root LUNA SDK object interface\n")
			.def("pushFrame", &PyIStream::pushFrame,"fuck");


	py::class_<PyICallback>(t, "PyICallback", "Root LUNA SDK object interface\n")
			.def_readonly("type", &PyICallback::type)
			.def_readonly("image", &PyICallback::image)
			.def_readonly("landmarks", &PyICallback::landmarks)
			.def_readonly("bbox", &PyICallback::bbox)
			.def_readonly("score", &PyICallback::score)
			.def_readonly("trackId", &PyICallback::trackId)
			.def_readonly("frameId", &PyICallback::frameId);

	py::enum_<PyICallback::CallbackType>(t, "TrackEngineCallbackType", "Complete or FrontEdition version.\n")
			.value("ctVisual", PyICallback::CallbackType::ctVisual)
			.value("ctBestShot", PyICallback::CallbackType::ctBestShot)
			.value("ctTrackEnd", PyICallback::CallbackType::ctTrackEnd)
			.export_values();
}
