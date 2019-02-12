#include <pybind11/pybind11.h>
#include <fsdk/FaceEngine.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>
#include "ErrorsAdapter.hpp"

namespace py = pybind11;

template<class T>
py::class_<fsdk::BaseDetection<T>> detection_class(py::module& this_module, const char* name)
{
	py::class_<fsdk::BaseDetection<T>> class_instance(this_module, name);
	
	class_instance.def(py::init<>());
	class_instance.def_readwrite("rect", &fsdk::BaseDetection<T>::rect, "Object bounding box\n");
	class_instance.def_readwrite("score", &fsdk::BaseDetection<T>::score, "Object detection score\n");
	class_instance.def("isValid", &fsdk::BaseDetection<T>::isValid, 
		"Checks whether a detection is valid.\n"
		"\t\tA detection is considered valid if it has a valid rect and score in [0..1] range.\n");
	class_instance.def("__repr__",
		[](const fsdk::BaseDetection<T> &r) {
			return "x = " + std::to_string(r.rect.x) +
				", y = " + std::to_string(r.rect.y) +
				", width = " + std::to_string(r.rect.width) +
				", height = " + std::to_string(r.rect.height);
		})
		;
	
	return class_instance;
}

void set_detection_class(py::module& f)
{
	auto detection = detection_class<int>(f, "Detection");
	auto detectionFloat = detection_class<float>(f, "DetectionFloat");
}


void detector_module(py::module& f) {
	
	set_detection_class(f);
	
	py::class_<fsdk::IDetectorPtr>(f, "IDetectorPtr", "Face detector interface")
		.def("detect", [](
			const fsdk::IDetectorPtr& det,
			const std::vector<fsdk::Image>& imagesVec,
			const std::vector<fsdk::Rect>& rectanglesVec,
			const uint32_t detectionPerImageNum,
			const fsdk::DetectionType type) {
				fsdk::Span<const fsdk::Image> images(imagesVec);
				fsdk::Span<const fsdk::Rect> rectangles(rectanglesVec);
				fsdk::ResultValue<fsdk::FSDKError, fsdk::Ref<fsdk::IResultBatch<fsdk::Face>>> err =
				det->detect(images, rectangles, detectionPerImageNum, type);
				if (err.isOk()) {
					const uint32_t sizeBatch = err.getValue()->getSize();
					py::list outList(sizeBatch);
					
					for (uint32_t i = 0; i < sizeBatch; ++i) {
						fsdk::Span<fsdk::Face> resultsSpan = err.getValue()->getResults(i);
						const uint32_t rowSize = resultsSpan.size();
						py::list outRow(rowSize);
						for (uint32_t j = 0; j < rowSize; ++j) {
							outRow[j] = resultsSpan.data()[j];
						}
						outList[i] = outRow;
					}
					return std::make_tuple(FSDKErrorResult(err), outList);
				} else
					return std::make_tuple(FSDKErrorResult(err), py::list());
			
		},
			"Detect faces and landmarks on multiple images\n"
			"\tArgs:\n"
			"\t\tparam1 (list of images): input images list. Format must be R8G8B8\n"
			"\t\tparam2 (list of rects): input rectangles of interest list.\n"
			"\t\t\tSize of list must be the same with images list\n"
			"\t\tparam3 (int): max number of detections per input image\n"
			"\t\tparam4 (DetectionType): type of detection: dtBBox, dt5landmarks or dt68landmarks\n"
			"\tReturns:\n"
			"\t\t(tuple): \n"
			"\t\t\ttuple with FSDKErrorResult code and list of lists of Faces\n")
		
		.def("setDetectionComparer", [](
			const fsdk::IDetectorPtr& det,
			fsdk::DetectionComparerType comparerType) {
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
				 "\t\t(tuple): \n"
				 "\t\t\twith error code and Face object (detection bbox, landmarks, etc)\n")
		.def("redetectOne", [](
				const fsdk::IDetectorPtr& det,
				fsdk::Face face,
				const fsdk::DetectionType type,
				const int tol) {
				fsdk::ResultValue<fsdk::FSDKError, bool> err = det->redetectOne(face, type, tol);
				if (err.isOk() && err.getValue()) {
					return std::make_tuple(FSDKErrorValueBool(err), face);
				}
				
				return std::make_tuple(FSDKErrorValueBool(err), fsdk::Face());
			}, py::arg("face"), py::arg("type"), py::arg("tolerance") = 0,
			"Redetect face.\n"
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
	
	py::class_<fsdk::Human>(f, "Human", "Human detection\n")
		.def(py::init<>())
		.def_readwrite("detection", &fsdk::Human::m_detection, "Object bounding box")
		.def("isValid", &fsdk::Human::isValid)
		.def("__repr__",
			[](const fsdk::Human& d) {
				return "Human: rect: x = " + std::to_string(d.m_detection.rect.x) +
					", y = " + std::to_string(d.m_detection.rect.y) +
					", width = " + std::to_string(d.m_detection.rect.width) +
					", height = " + std::to_string(d.m_detection.rect.height) +
					"; score = " + std::to_string(d.m_detection.score) +
					"; isValid = " + std::to_string(d.m_detection.isValid());
			});
			;
	
	py::class_<fsdk::Ref<fsdk::IHumanDetector>>(f, "IHumanDetectorPtr", "Human detector interface.\n")
		.def("detect", [](
				const fsdk::Ref<fsdk::IHumanDetector>& det,
				const std::vector<fsdk::Image>& imagesVec,
				const std::vector<fsdk::Rect>& rectanglesVec,
				const uint32_t detectionPerImageNum) {
				fsdk::Span<const fsdk::Image> images(imagesVec);
				fsdk::Span<const fsdk::Rect> rectangles(rectanglesVec);
				fsdk::ResultValue<fsdk::FSDKError, fsdk::Ref<fsdk::IResultBatch<fsdk::Human>>> err =
					det->detect(images, rectangles, detectionPerImageNum);
				if (err.isOk()) {
					const uint32_t sizeBatch = err.getValue()->getSize();
					py::list outList(sizeBatch);
					
					for (uint32_t i = 0; i < sizeBatch; ++i) {
						fsdk::Span<fsdk::Human> resultsSpan = err.getValue()->getResults(i);
						const uint32_t rowSize = resultsSpan.size();
						py::list outRow(rowSize);
						for (uint32_t j = 0; j < rowSize; ++j) {
							outRow[j] = resultsSpan.data()[j];
						}
						outList[i] = outRow;
					}
					return std::make_tuple(FSDKErrorResult(err), outList);
				} else
					return std::make_tuple(FSDKErrorResult(err), py::list());
				
			},
			"Detects humans\n"
				"\tArgs:\n"
				"\t\tparam1 (list of images): input images list. Format must be R8G8B8\n"
				"\t\tparam2 (list of rects): input rectangles of interest list.\n"
				"\t\t\tSize of list must be the same with images list\n"
				"\t\tparam3 (int): max number of detections per input image\n"
				"\tReturns:\n"
				"\t\t(tuple): \n"
				"\t\t\ttuple with FSDKErrorResult code and list of lists of Detections\n")
			;
	
}

