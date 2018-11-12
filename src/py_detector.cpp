#include <pybind11/pybind11.h>
#include <fsdk/FaceEngine.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>
#include "ErrorsAdapter.hpp"

namespace py = pybind11;

void detector_module(py::module& f) {
	
	py::class_<fsdk::IDetectorPtr>(f, "IDetectorPtr", "Face detector interface")
		.def("detect", [](
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
				}
			},
			"Detect faces and landmarks on the image\n"
			"\tArgs:\n"
			"\t\tparam1 (Image): input image. Format must be R8G8B8\n"
			"\t\tparam2 (Rect): rect of interest inside of the image\n"
			"\t\tparam3 (int): length of `detections` and `landmarks` arrays\n"
			"\tReturns:\n"
			"\t\t(tuple with FSDKErrorValueInt code and list of tuples): \n"
			"\t\t\ttuple with FSDKErrorValueInt code and list of tuples from\n"
			"\t\t\tDetection, Landmarks5, Landmarks68, see FSDKErrorValueInt (see FSDKErrorValueInt)\n")
		
		.def("detect", [](
			const fsdk::IDetectorPtr& det,
			const std::vector<fsdk::Image>& imagesVec,
			const std::vector<fsdk::Rect>& rectanglesVec,
			const uint32_t detectionPerImageNum,
			const fsdk::DetectionType type) {
			fsdk::Span<const fsdk::Image> images(imagesVec.data(), imagesVec.size());
			fsdk::Span<const fsdk::Rect> rectangles(rectanglesVec.data(), rectanglesVec.size());
			fsdk::ResultValue<fsdk::FSDKError, fsdk::Ref<fsdk::IResultBatch<fsdk::Face>>> err =
				det->detect(images, rectangles, detectionPerImageNum, type);
			if (err.isOk()) {
				const uint32_t size = err.getValue()->getSize();
				const auto* resultsPtr = err.getValue()->getResults().begin();
				return std::make_tuple(FSDKErrorResult(err), std::vector<fsdk::Face>(resultsPtr, resultsPtr + size));
			} else {
				return std::make_tuple(FSDKErrorResult(err), std::vector<fsdk::Face>());
			}
			
		},
			"Detect faces and landmarks on multiple images\n"
			"\tArgs:\n"
			"\t\tparam1 (list of images): input images list. Format must be R8G8B8\n"
			"\t\tparam2 (list of rects): input rectangles of interest list.\n"
			"\t\t\tSize of list must be the same with images list\n"
			"\t\tparam3 (int): max number of detections per input image\n"
			"\t\tparam4 (DetectionType): type of detection: dtBBox, dt5landmarks or dt68landmarks\n"
			"\tReturns:\n"
			"\t\t(tuple with FSDKErrorValueInt code and list of tuples): \n"
			"\t\t\ttuple with FSDKErrorValueInt code and list of tuples from\n"
			"\t\t\tDetection, Landmarks5, Landmarks68, see FSDKErrorValueInt (see FSDKErrorValueInt)\n")
		
		.def("setDetectionComparer", [](
			const fsdk::IDetectorPtr& det,
			fsdk::DetectionComparerType comparerType){
			det->setDetectionComparer(comparerType);
		}, "Set detection comparer from SDK defined list\n")
		
		.def("detectOne", [](
			const fsdk::IDetectorPtr& det,
			const fsdk::Image& image,
			const fsdk::Rect rect,
			const fsdk::DetectionType type){
			
			fsdk::ResultValue<fsdk::FSDKError, fsdk::Face> err = det->detectOne(image, rect, type);
			if (err.isOk())
				return std::make_tuple(FSDKErrorResult(err), err.getValue());
			else
				return std::make_tuple(FSDKErrorResult(err), fsdk::Face());
		},
			"Light function to get just one best detection from single input image\n"
				"\tArgs:\n"
				"\t\tparam1 (Image): input image\n"
				"\t\tparam2 (Rect): rectangle of interest on image\n"
				"\t\tparam3 (DetectionType): type of detection: dtBBox, dtlandmarks or dt68landmarks\n"
				"\tReturns:\n"
				"\t\t(tuple with FSDKErrorResult and list of Detections): \n"
				"\t\t\twith error code and a Face object (detection bbox, landmarks, etc)\n")
		
		.def("detect_light", [](
				const fsdk::IDetectorPtr& det,
				const fsdk::Image& image,
				const fsdk::Rect& rect,
				int maxCount) {
				std::vector<fsdk::Detection> detections(maxCount);
				fsdk::ResultValue<fsdk::FSDKError, int> err = det->detect(
					image,
					rect,
					detections.data(),
					maxCount);
			
				if (err.isOk()) {
					const uint32_t size = err.getValue();
					detections.resize(size);
					return std::make_tuple(FSDKErrorValueInt(err), std::move(detections));
				} else {
					return std::make_tuple(FSDKErrorValueInt(err), std::vector<fsdk::Detection>());
				}
			},
			"Detect faces and landmarks on the image\n"
				"\tArgs:\n"
				"\t\tparam1 (Image): input image. Format must be R8G8B8\n"
				"\t\tparam2 (Rect): rect of interest inside of the image\n"
				"\t\tparam3 (int): length of `detections` array\n"
				"\tReturns:\n"
				"\t\t(tuple with FSDKErrorValueInt code and list of Detections): \n"
				"\t\t\ttuple with FSDKErrorValueInt code and list of Detections\n")
		
		.def("detect5", [](
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
			"\t\t\t tuple with FSDKErrorValueInt code and list of tuples from Detection, "
			"Landmarks5 (see FSDKErrorValueInt)\n")
		
		.def("redetectOne", [](
				const fsdk::IDetectorPtr& det,
				fsdk::Face& face,
				const fsdk::DetectionType type,
				const int tol) {
				fsdk::Result<fsdk::FSDKError> err = det->redetectOne(face, type, tol);
				return std::make_tuple(FSDKErrorResult(err), face);
			}, py::arg("face"), py::arg("type"), py::arg("tolerance") = 0,
			"Batched redetect faces.\n"
			"\tArgs:\n"
			"\t\tparam1 (Face): face structure with detection and landmarks.\n"
			"\t\tparam2 (type): type of detection: BBox, 5landmarks or 68landmarks.\n"
			"\t\tparam3 (int): tolerance allowed face shifting between two nearest detection frames in pixels.\n"
			"\tReturns:\n"
			"\t\t(tuple): tuple with FSDKErrorResult and Face structure\n")
		
		.def("redetect", [](
				const fsdk::IDetectorPtr& det,
				std::vector<fsdk::Face>& faces,
				const fsdk::DetectionType type,
				const int tol = 0) {
					const fsdk::Span<fsdk::Face> facesSpan(faces.data(), faces.size());
					fsdk::Result<fsdk::FSDKError> err = det->redetect(facesSpan, type, tol);
					const auto* const ptr = facesSpan.begin();
					const uint32_t size = facesSpan.size();
					return std::make_tuple(FSDKErrorResult(err),
							std::move(std::vector<fsdk::Face>(ptr, ptr + size)));
				}, py::arg("faces"), py::arg("type"), py::arg("tolerance") = 0,
			"Batched redetect faces.\n"
			"\tArgs:\n"
			"\t\tparam1 ([Face]): detections list.\n"
			"\t\tparam2 (type): type of detection: BBox, 5landmarks or 68landmarks.\n"
			"\t\tparam3 (int): tolerance allowed face shifting between two nearest detection frames in pixels.\n"
			"\tReturns:\n"
			"\t\t(tuple with FSDKErrorResult code and list of Faces): \n"
			"\t\t\t tuple with FSDKErrorResult and list of tuples from Detection\n")
					;
	
	
	py::class_<fsdk::Detection>(f, "Detection",
		"Face detection.\n"
			"\tStores a detected face bounding box within a source image frame "
			"as well as detection confidence score.\n")
		.def(py::init<>())
		.def_readwrite("rect", &fsdk::Detection::rect, "Object bounding box")
		.def_readwrite("score",&fsdk::Detection::score, "Object detection score)")
		.def("isValid", &fsdk::Detection::isValid)
		.def("__repr__",
			[](const fsdk::Detection& d) {
				return "Detection: rect: x = " + std::to_string(d.rect.x) +
						", y = " + std::to_string(d.rect.y) +
						", width = " + std::to_string(d.rect.width) +
						", height = " + std::to_string(d.rect.height) +
						"; score = " + std::to_string(d.score) +
						"; isValid = " + std::to_string(d.isValid());
			});
	
}
