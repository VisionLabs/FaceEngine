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

namespace py = pybind11;

PyITrackEngine createPyFaceEnginePtr(const PyIFaceEngine& fsdk, const std::string& configPath) {
	return PyITrackEngine{fsdk, configPath};
}

PYBIND11_MODULE(TrackEngine, t) {
	t.def("getSmth", []() -> std::string {
		return std::string("trackengine smth");
	});

	t.def("createTrackEngine", &createPyFaceEnginePtr, py::return_value_policy::take_ownership,
		  "Create TrackEngine",
		  "Create the TrackEngine object\n");


	py::class_<PyITrackEngine>(t, "PyITrackEngine", "Root LUNA SDK object interface\n")
	        .def("getStr", &PyITrackEngine::getStr,"fuck")
	        .def("getFSDKVer", &PyITrackEngine::getFSDKVer,"fuck2")
			.def("createStream", &PyITrackEngine::createStream, "fuck3");

	py::class_<PyIStream>(t, "PyIStream", "Root LUNA SDK object interface\n")
			.def("pushFrame", &PyIStream::pushFrame,"fuck");
}
