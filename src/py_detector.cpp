#include <pybind11/pybind11.h>
#include <fsdk/FaceEngine.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>
#include "ErrorsAdapter.hpp"

namespace py = pybind11;

void detector_module(py::module& f) {

py::class_<fsdk::IDetectorPtr>(f, "IDetectorPtr", "Face detector interface")
	.def("detect",[](
		const fsdk::IDetectorPtr& det,
		const fsdk::Image& image,
		const fsdk::Rect& rect,
		uint32_t maxCount) {
		  std::vector<fsdk::Detection> detections(maxCount);
			std::vector<fsdk::Landmarks5> landmarks(maxCount);
			std::vector<fsdk::Landmarks68> landmarks68(maxCount);
			fsdk::ResultValue<fsdk::FSDKError, int> err = det->detect(
				image,
				rect,
				detections.data(),
				landmarks.data(),
				landmarks68.data(),
				maxCount);
			
			if (err.isOk()) {
				const uint32_t size = err.getValue();
				auto detectionResultPyList = py::list(size);
				for (uint32_t i = 0; i < size; ++i) {
					detectionResultPyList[i] = std::make_tuple(detections[i], landmarks[i], landmarks68[i]);
				}
				return std::make_tuple(FSDKErrorValueInt(err), detectionResultPyList);
			} else {
				return std::make_tuple(FSDKErrorValueInt(err), py::list());
			}},
		"Detect faces and landmarks on the image\n"
		"\tArgs:\n"
		"\t\tparam1 (Image): input image. Format must be R8G8B8\n"
		"\t\tparam2 (Rect): rect of interest inside of the image\n"
		"\t\tparam3 (int): length of `detections` and `landmarks` arrays\n"
		"\tReturns:\n"
		"\t\t(tuple with FSDKErrorValueInt code and list of tuples): \n"
		"\t\t\ttuple with FSDKErrorValueInt code and list of tuples from\n"
		"\t\t\tDetection, Landmarks5, Landmarks68see FSDKErrorValueInt (see FSDKErrorValueInt)\n")
	
	.def("detect_light",[](
		const fsdk::IDetectorPtr& det,
		const fsdk::Image& image,
		const fsdk::Rect& rect,
		int maxCount) {
			std::vector<fsdk::Detection> detections(maxCount);
			fsdk::ResultValue<fsdk::FSDKError, int> err = det->detect(
				image,
				rect,
				detections.data() ,
				maxCount);
			
			if (err.isOk()) {
				const uint32_t size = err.getValue();
				detections.resize(size);
				return std::make_tuple(FSDKErrorValueInt(err), std::move(detections));
			} else {
					return std::make_tuple(FSDKErrorValueInt(err), std::vector<fsdk::Detection>()); }},
		"Detect faces and landmarks on the image\n"
		"\tArgs:\n"
		"\t\tparam1 (Image): input image. Format must be R8G8B8\n"
		"\t\tparam2 (Rect): rect of interest inside of the image\n"
		"\t\tparam3 (int): length of `detections` array\n"
		"\tReturns:\n"
		"\t\t(tuple with FSDKErrorValueInt code and list of Detections): \n"
		"\t\t\ttuple with FSDKErrorValueInt code and list of Detections (see FSDKErrorValueInt)\n")
	
	.def("detect5",[](
		const fsdk::IDetectorPtr& det,
		const fsdk::Image& image,
		const fsdk::Rect& rect,
		int maxCount) {
			std::vector<fsdk::Detection> detections(maxCount);
			std::vector<fsdk::Landmarks5> landmarks(maxCount);
			fsdk::ResultValue<fsdk::FSDKError, int> err = det->detect(
				image,
				rect,
				detections.data(),
				landmarks.data(),
				maxCount);
		if (err.isOk()) {
			const uint32_t size = err.getValue();
			auto detectionResultPyList = py::list(size);
			for (uint32_t i = 0; i < size; ++i) {
				detectionResultPyList[i] = std::make_tuple(detections[i], landmarks[i]);
			}
			return std::make_tuple(FSDKErrorValueInt(err), detectionResultPyList);
		} else
			return std::make_tuple(FSDKErrorValueInt(err), py::list());
		},
		"Detect faces and landmarks on the image\n"
		"\tArgs:\n"
		"\t\tparam1 (Image): input image. Format must be R8G8B8\n"
		"\t\tparam2 (Rect): rect of interest inside of the image\n"
		"\t\tparam3 (int): length of `detections` and `landmarks` arrays\n"
		"\tReturns:\n"
		"\t\t(tuple with FSDKErrorValueInt code and list of tuples): \n"
		"\t\t\t tuple with FSDKErrorValueInt code and list of tuples from Detection, Landmarks5 (see FSDKErrorValueInt)\n")
		;
	
py::class_<fsdk::Detection>(f, "Detection",
	"Face detection.\n"
	"\tStores a detected face bounding box within a source image frame as well as detection confidence score.\n")
	.def(py::init<>())
	.def_readwrite("rect", &fsdk::Detection::rect, "Object bounding box")
	.def_readwrite("score", &fsdk::Detection::score, "Object detection score)")
	.def("isValid", &fsdk::Detection::isValid)
	.def("__repr__",
		 [](const fsdk::Detection &d) {
			 return "Detection: rect: x = " + std::to_string(d.rect.x) +
				", y = " + std::to_string(d.rect.y) +
				", width = " + std::to_string(d.rect.width) +
				", height = " + std::to_string(d.rect.height) +
				"; score = " + std::to_string(d.score) +
				"; isValid = " + std::to_string(d.isValid());
		 })
	;
}