#include <pybind11/pybind11.h>
#include <fsdk/FaceEngine.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>
#include "ErrorsAdapter.hpp"

namespace py = pybind11;

template<class T, class Y>
py::class_<fsdk::BaseDetection<T>> detection_class(py::module& this_module, const char* name)
{
	py::class_<fsdk::BaseDetection<T>> class_instance(this_module, name);
	
	class_instance.def(py::init<>());
	class_instance.def(py::init<fsdk::BaseDetection<Y>>());
	
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
				", height = " + std::to_string(r.rect.height) +
				", score = " + std::to_string(r.score);
		})
		;
	class_instance.def("set", [](fsdk::BaseDetection<T>& self, fsdk::BaseDetection<Y>& other) {
		self = other;
		return self;
	});
	
	class_instance.def(py::init(
		[](const fsdk::BaseRect<T>& rect,
			const float score) {
			fsdk::BaseDetection<T> detection;
			detection.rect = rect;
			detection.score = score;
			return detection;
		}
	));
	
	class_instance.def(py::init(
		[](const fsdk::BaseRect<Y>& rect,
			const float score) {
			fsdk::BaseDetection<T> detection;
			detection.rect = rect;
			detection.score = score;
			return detection;
		}
	));
	
	class_instance.def("asInt", [](fsdk::BaseDetection<T>& self) {
		fsdk::BaseDetection<int> detection;
		detection.rect = self.rect;
		detection.score = self.score;
		return detection;
	});
	
	class_instance.def("asFloat", [](fsdk::BaseDetection<T>& self) {
		fsdk::BaseDetection<float> detection;
		detection.rect = self.rect;
		detection.score = self.score;
		return detection;
	});
	
	return class_instance;
}

void set_detection_class(py::module& f)
{
	auto detection = detection_class<int, float>(f, "Detection");
	auto detectionFloat = detection_class<float, int>(f, "DetectionFloat");
}


void detector_module(py::module& f) {
	
	set_detection_class(f);
	
	py::class_<fsdk::IDetectorPtr>(f, "IDetectorPtr", "Face detector interface")
		.def("detect", [](
			const fsdk::IDetectorPtr& det,
			const std::vector<fsdk::Image>& imagesVec,
			const std::vector<fsdk::Rect>& rectanglesVec,
			const int detectionPerImageNum,
			const fsdk::DetectionType type) {
				fsdk::Span<const fsdk::Image> images(imagesVec);
				fsdk::Span<const fsdk::Rect> rectangles(rectanglesVec);
				fsdk::ResultValue<fsdk::FSDKError, fsdk::Ref<fsdk::IResultBatch<fsdk::Face>>> err =
				det->detect(images, rectangles, detectionPerImageNum, type);
				if (err.isOk()) {
					const size_t sizeBatch = err.getValue()->getSize();
					py::list outList(sizeBatch);
					
					for (size_t i = 0; i < sizeBatch; ++i) {
						fsdk::Span<fsdk::Face> resultsSpan = err.getValue()->getResults(i);
						const size_t rowSize = resultsSpan.size();
						py::list outRow(rowSize);
						for (size_t j = 0; j < rowSize; ++j) {
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
			"\t\tparam4 (DetectionType): type of detection: DT_BBOX, DT_LANDMARKS5 or DT_LANDMARKS68\n"
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
			const fsdk::Rect& rect,
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
				 "\t\tparam3 (DetectionType): type of detection: DT_BBOX, DT_LANDMARKS5 or DT_LANDMARKS68\n"
				 "\tReturns:\n"
				 "\t\t(tuple): \n"
				 "\t\t\twith error code and Face object (detection bbox, landmarks, etc)\n")
				 
		.def("redetectOne", [](
				const fsdk::IDetectorPtr& det,
				fsdk::Face face,
				const fsdk::DetectionType type) {
				fsdk::ResultValue<fsdk::FSDKError, bool> err = det->redetectOne(face, type);
				if (err.isOk() && err.getValue()) {
					return std::make_tuple(FSDKErrorResult(err), face);
				}
				
				return std::make_tuple(FSDKErrorResult(err), fsdk::Face());
			}, py::arg("face"), py::arg("type"),
			"Redetect face.\n"
			"\tArgs:\n"
			"\t\tparam1 (Face): face structure with detection and landmarks.\n"
			"\t\tparam2 (type): type of detection: BBox, 5landmarks or 68landmarks.\n"
			"\tReturns:\n"
			"\t\t(tuple): tuple with FSDKErrorResult and Face structure\n")
		
		.def("redetectOne", [](
				const fsdk::IDetectorPtr& det,
				const fsdk::Image& image,
				const fsdk::BaseRect<float>& rect,
				const fsdk::DetectionType type) {
					fsdk::ResultValue<fsdk::FSDKError, fsdk::Face> result = det->redetectOne(image, rect, type);
					if (result.isOk()) {
						return std::make_tuple(FSDKErrorResult(result), result.getValue());
					}
					return std::make_tuple(FSDKErrorResult(result), fsdk::Face());
				}, py::arg("image"), py::arg("detection"), py::arg("type"),
			"Redetect face.\n"
			"\tArgs:\n"
			"\t\tparam1 (image): input image. Format must be R8G8B8.\n"
			"\t\tparam2 (rect): rectangle of interest on image.\n"
			"\t\tparam3 (type): type of detection: BBox, 5landmarks or 68landmarks.\n"
			"\tReturns:\n"
			"\t\t(tuple): tuple with FSDKErrorResult and Face structure\n")

		.def("redetectOne", [](
				const fsdk::IDetectorPtr& det,
				const fsdk::Image& image,
				const fsdk::Rect& rect,
				const fsdk::DetectionType type) {
					fsdk::ResultValue<fsdk::FSDKError, fsdk::Face> result = det->redetectOne(image, rect, type);
					if (result.isOk()) {
						return std::make_tuple(FSDKErrorResult(result), result.getValue());
					}
					return std::make_tuple(FSDKErrorResult(result), fsdk::Face());
				}, py::arg("image"), py::arg("detection"), py::arg("type"),
			"Redetect face.\n"
			"\tArgs:\n"
			"\t\tparam1 (image): input image. Format must be R8G8B8.\n"
			"\t\tparam2 (rect): rectangle of interest on image.\n"
			"\t\tparam3 (type): type of detection: BBox, 5landmarks or 68landmarks.\n"
			"\tReturns:\n"
			"\t\t(tuple): tuple with FSDKErrorResult and Face structure\n")
		
		.def("redetect", [](
				const fsdk::IDetectorPtr& det,
				std::vector<fsdk::Face>& faces,
				const fsdk::DetectionType type) {
					const fsdk::Span<fsdk::Face> facesSpan(faces.data(), faces.size());
					std::vector<fsdk::Result<fsdk::FSDKError>> outErrors(faces.size());
					fsdk::Span<fsdk::Result<fsdk::FSDKError>> errorsSpan(outErrors);
					fsdk::Result<fsdk::FSDKError> err = det->redetect(facesSpan, type, errorsSpan);
					return std::make_tuple(FSDKErrorResult(err),
						std::vector<fsdk::Face>(facesSpan.begin(), facesSpan.end()),
						std::vector<FSDKErrorResult>(errorsSpan.begin(), errorsSpan.end()));
			},
			"Batched redetect faces.\n"
			"\tArgs:\n"
			"\t\tparam1 ([Face]): detections list.\n"
			"\t\tparam2 (type): type of detection: BBox, 5landmarks or 68landmarks.\n"
			"\tReturns:\n"
			"\t\t(tuple) tuple with FSDKErrorResult, list of tuples from Detection, list of FSDKErrorResult for each face\n")
		;
	
	py::class_<fsdk::Ref<fsdk::IHumanDetector>>(f, "IHumanDetectorPtr", "Human detector interface.\n")
		.def("detect", [](
				const fsdk::Ref<fsdk::IHumanDetector>& det,
				const std::vector<fsdk::Image>& imagesVec,
				const std::vector<fsdk::Rect>& rectanglesVec,
				const uint32_t detectionPerImageNum,
				fsdk::HumanDetectionType type = fsdk::HumanDetectionType::DCT_BOX) {
					fsdk::Span<const fsdk::Image> images(imagesVec);
					fsdk::Span<const fsdk::Rect> rectangles(rectanglesVec);
					fsdk::ResultValue<fsdk::FSDKError, fsdk::Ref<fsdk::IResultBatch<fsdk::Human>>> err =
						det->detect(
							images,
							rectangles,
							detectionPerImageNum,
							type);
					if (err.isOk()) {
						const size_t sizeBatch = err.getValue()->getSize();
						py::list outList(sizeBatch);

						for (size_t i = 0; i < sizeBatch; ++i) {
							fsdk::Span<fsdk::Human> resultsSpan = err.getValue()->getResults(i);
							const size_t rowSize = resultsSpan.size();
							py::list outRow(rowSize);
							for (size_t j = 0; j < rowSize; ++j) {
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
				"\t\tparam4 (HumanDetectionType) Human detection type enumeration \n"
				"\tReturns:\n"
				"\t\t(tuple): \n"
				"\t\t\ttuple with FSDKErrorResult code and list of lists of Detections\n")

		.def("redetectOne", [](
				const fsdk::Ref<fsdk::IHumanDetector>& det,
				const fsdk::Human& human) {
					fsdk::Human inOutHuman = human;
					fsdk::ResultValue<fsdk::FSDKError, bool> resValue = 
						det->redetectOne(inOutHuman);
					return std::make_tuple(FSDKErrorValueBool(resValue), inOutHuman);
			},
			"Redetects one human based on the previous detection on the new image\n"
			"\tArgs:\n"
			"\t\tparam1 (Human): human structure with detection and image\n"
			"\tReturns:\n"
			"\t\t(tuple): tuple with FSDKErrorValueBool and Human structure\n")
			;

}

