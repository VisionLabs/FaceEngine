#include <pybind11/pybind11.h>
#include <fsdk/FaceEngine.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>
#include <ErrorsAdapter.hpp>

namespace py = pybind11;

void detector_module(py::module& f) {

	py::enum_<fsdk::SensorType>(f, "SensorType", py::arithmetic(), "Camera sensor type.\n")
		.value("Visible", fsdk::SensorType::Visible, "Visible sensor type (default mode)")
		.value("NIR", fsdk::SensorType::NIR, "NIR sensor type")
		.export_values();
			;

	py::enum_<fsdk::ObjectDetectorClassType>(f, "ObjectDetectorClassType", py::arithmetic(), "Object detector type enumeration.\n")
		.value("FACE_DET_DEFAULT", fsdk::FACE_DET_DEFAULT, "Default detector cpecified in config file")
		.value("FACE_DET_V1", fsdk::FACE_DET_V1, "First detector type")
		.value("FACE_DET_V2", fsdk::FACE_DET_V2, "Light detector type")
		.value("FACE_DET_V3", fsdk::FACE_DET_V3, "Third detector type")
		.export_values();
			;

	py::class_<fsdk::Face>(f, "Face", "Container for detection and landmakrs\n")
		.def(py::init<>())
		.def(py::init<fsdk::Image>())
		.def(py::init<fsdk::Image, fsdk::Detection>())
		.def(py::init<fsdk::Image, fsdk::Detection>())
		.def_readwrite("img", &fsdk::Face::img, "Image\n")
		.def_readwrite("detection", &fsdk::Face::detection, "Detection\n")
		.def_readwrite("landmarks5_opt", &fsdk::Face::landmarks5, "Landmarks5 optinal\n")
		.def_readwrite("landmarks68_opt", &fsdk::Face::landmarks68, "Landmarks68 optinal\n")
		.def("isValid", &fsdk::Face::isValid, "Valid or not\n")
			;

	py::class_<fsdk::Detection>(f, "Detection", "Detection structure")
		.def(py::init<>())
		.def(py::init<fsdk::FloatRect, float>())
		.def(py::init<fsdk::Rect, float>())
		.def(py::init<fsdk::FloatRect, int, int, float>())
		.def(py::init<fsdk::Rect, int, int, float>())
		.def(py::init<fsdk::FloatRect, fsdk::Rect, float>())
		.def(py::init<fsdk::Rect, fsdk::Rect, float>())
		.def("getRect", &fsdk::Detection::getRect)
		.def("getRawRect", &fsdk::Detection::getRawRect)
		.def("setRawRect", &fsdk::Detection::setRawRect)
		.def("getScore", &fsdk::Detection::getScore)
		.def("setScore", &fsdk::Detection::setScore)
		.def("isValid", &fsdk::Detection::isValid)
		.def("__repr__",
			[](const fsdk::Detection& d) {
				const auto rawRect = d.getRawRect();
				const auto rect = d.getRect();
				return "rawRect: x = " + std::to_string(rawRect.x) +
					", y = " + std::to_string(rawRect.y) +
					", width = " + std::to_string(rawRect.width) +
					", height = " + std::to_string(rawRect.height) +
					"; Rect: x = " + std::to_string(rect.x) +
					", y = " + std::to_string(rect.y) +
					", width = " + std::to_string(rect.width) +
					", height = " + std::to_string(rect.height);
			})
		;

	py::class_<fsdk::IFaceDetectionBatchPtr>(f, "IFaceDetectionBatchPtr", "Face detection result batch structure")
		.def("getSize", [](
			const fsdk::IFaceDetectionBatchPtr& batch
		) {
			return batch->getSize();
		})

		.def("getDetections", [](
			const fsdk::IFaceDetectionBatchPtr& batch,
			size_t index) {
				const size_t rowSize = batch->getSize(index);
				py::list out(rowSize);
				const auto detections = batch->getDetections(index);
				for (size_t i = 0; i < rowSize; ++i) {
					out[i] = detections[i];
				}
				return out;
			},
			"Returns list of detections for the one image"
			"\tArgs:\n"
			"\t\tparam1 (index): index of the image"
			"\tReturns:\n"
			"\t\t(list): \n"
			"\t\t\tlist with of Detection objects\n")

		.def("getLandmarks5", [](
			const fsdk::IFaceDetectionBatchPtr& batch,
			size_t index) {
				const size_t rowSize = batch->getSize(index);
				const auto landmarks5 = batch->getLandmarks5(index);
				if (landmarks5.empty()) {
					return py::list{};
				}
				py::list out(rowSize);
				for (size_t i = 0; i < rowSize; ++i) {
					out[i] = landmarks5[i];
				}
				return out;
			},
			"Returns list of landmarks5 for the one image"
			"\tArgs:\n"
			"\t\tparam1 (index): index of the image"
			"\tReturns:\n"
			"\t\t(list): \n"
			"\t\t\tlist with of Landmarks5 objects\n")

		.def("getLandmarks68", [](
			const fsdk::IFaceDetectionBatchPtr& batch,
			size_t index) {
				const size_t rowSize = batch->getSize(index);
				const auto landmarks68 = batch->getLandmarks68(index);
				if (landmarks68.empty()) {
					return py::list{};
				}
				py::list out(rowSize);
				for (size_t i = 0; i < rowSize; ++i) {
					out[i] = landmarks68[i];
				}
				return out;
			},
			"Returns list of landmarks68 for the one image"
			"\tArgs:\n"
			"\t\tparam1 (index): index of the image"
			"\tReturns:\n"
			"\t\t(list): \n"
			"\t\t\tlist with of Landmarks68 objects\n")
		;

	py::class_<fsdk::IDetectorPtr>(f, "IDetectorPtr", "Face detector interface")
		.def("detect", [](
			const fsdk::IDetectorPtr& det,
			const std::vector<fsdk::Image>& images,
			const std::vector<fsdk::Rect>& rectangles,
			const uint32_t detectionPerImageNum,
			const fsdk::DetectionType type) {
				fsdk::ResultValue<fsdk::FSDKError, fsdk::Ref<fsdk::IFaceDetectionBatch>> err =
					det->detect(images, rectangles, detectionPerImageNum, type);
				if (err.isOk()) {
					return std::make_tuple(FSDKErrorResult(err), err.getValue());
				} else {
					return std::make_tuple(FSDKErrorResult(err), fsdk::Ref<fsdk::IFaceDetectionBatch>{});
				}
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
			"\t\t\ttuple with FSDKErrorResult code and IFaceDetectionBatchPtr\n")

		.def("validate", [](
			 const fsdk::IDetectorPtr& det,
			 const std::vector<fsdk::Image>& images,
			 const std::vector<fsdk::Rect>& rects,
			 const int detectionPerImageNum) {
				 std::vector<fsdk::Result<fsdk::FSDKError>> errors(images.size());
				 fsdk::Result<fsdk::FSDKError> err = det->validate(images, rects, detectionPerImageNum, errors);
				 return std::make_tuple(FSDKErrorResult(err), std::vector<FSDKErrorResult>(errors.begin(), errors.end())); 
			},
			 "Validate input of multiple frames in a single function call.\n"
			 "\tArgs:\n"
			 "\t\tparam1 (list of Images): list of Images.\n"
			 "\t\tparam2 (list of Detections): list of detections.\n"
			 "\t\tparam3 (int): max number of detections per input image.\n"
			 "\tReturns:\n"
			 "\t\t(tuple): \n"
			 "\t\t\t tuple with FSDKErrorResult code, list of errors for each image)\n")

		.def("detectOne", [](
			const fsdk::IDetectorPtr& det,
			const fsdk::Image& image,
			const fsdk::Rect& rect,
			const fsdk::DetectionType type
		) {
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

		.def("redetect", [](
			const fsdk::IDetectorPtr& det,
			const std::vector<fsdk::Image>& images,
			fsdk::Ref<fsdk::IFaceDetectionBatch>& detectionBatch,
			const fsdk::DetectionType type) {
				fsdk::ResultValue<fsdk::FSDKError, fsdk::Ref<fsdk::IFaceDetectionBatch>> err = 
					det->redetect(images, detectionBatch, type);
				if (err.isOk()) {
					return std::make_tuple(FSDKErrorResult(err), err.getValue());
				} else {
					return std::make_tuple(FSDKErrorResult(err), fsdk::Ref<fsdk::IFaceDetectionBatch>{});
				}
			},
			"Batched redetect faces.\n"
			"\tArgs:\n"
			"\t\tparam1 ([images]): images list.\n"
			"\t\tparam1 (detectionBatch): result of the detect to make a redetect for.\n"
			"\t\tparam2 (type): type of detection: BBox, 5landmarks or 68landmarks.\n"
			"\tReturns:\n"
			"\t\t(tuple) tuple with FSDKErrorResult and IFaceDetectionBatchPtr object.\n")
		
		.def("validate", [](
			const fsdk::IDetectorPtr& det,
			const std::vector<fsdk::Image>& images,
			fsdk::Ref<fsdk::IFaceDetectionBatch>& detectionBatch,
			const uint32_t detectionPerImageNum) {
				std::vector<fsdk::Result<fsdk::FSDKError>> errors(images.size());
				fsdk::Result<fsdk::FSDKError> err = det->validate(images, detectionBatch, errors);
				return std::make_tuple(FSDKErrorResult(err), std::vector<FSDKErrorResult>(errors.begin(), errors.end())); 
			},
			"Validate input of multiple frames in a single function call.\n"
			"\tArgs:\n"
			"\t\tparam1 ([images]): images list.\n"
			"\t\tparam1 (detectionBatch): result of the detect to make a redetect for.\n"
			"\t\tparam3 (int): max number of detections per input image.\n"
			"\tReturns:\n"
			"\t\t(tuple): \n"
			"\t\t\t tuple with FSDKErrorResult code, list of errors for each image)\n")
			
		.def("redetect", [](
			const fsdk::IDetectorPtr& det,
			const std::vector<fsdk::Image>& images,
			const std::vector<std::vector<fsdk::Detection>>& detectionBatch,
			const fsdk::DetectionType type) {
				std::vector<fsdk::Span<const fsdk::Detection>> detections2D;
				detections2D.reserve(detectionBatch.size());
				for (const auto& row : detectionBatch) {
					detections2D.emplace_back(row.data(), row.size());
				}
				fsdk::ResultValue<fsdk::FSDKError, fsdk::Ref<fsdk::IFaceDetectionBatch>> err = 
					det->redetect(images, detections2D, type);
				if (err.isOk()) {
					return std::make_tuple(FSDKErrorResult(err), err.getValue());
				} else {
					return std::make_tuple(FSDKErrorResult(err), fsdk::Ref<fsdk::IFaceDetectionBatch>{});
				}
			},
			"Batched redetect faces.\n"
			"\tArgs:\n"
			"\t\tparam1 ([images]): source images list.\n"
			"\t\tparam1 ([detectionBatch]): detections - 2D array\n"
			"\t\tparam2 (type): type of detection: BBox, 5landmarks or 68landmarks.\n"
			"\tReturns:\n"
			"\t\t(tuple) tuple with FSDKErrorResult and IFaceDetectionBatchPtr object.\n")

		.def("validate", [](
			const fsdk::IDetectorPtr& det,
			const std::vector<fsdk::Image>& images,
			const std::vector<std::vector<fsdk::Detection>>& detectionBatch) {

				const uint32_t batchSize = detectionBatch.size();
				std::vector<fsdk::Span<const fsdk::Detection>> detections2D;
				detections2D.reserve(batchSize);
				std::vector<fsdk::Span<fsdk::Result<fsdk::FSDKError>>> errors2DSpan;
				errors2DSpan.reserve(batchSize);
				std::vector<std::vector<fsdk::Result<fsdk::FSDKError>>> errors2DVec(batchSize);

				for (uint32_t i = 0; i < batchSize; ++i) {
					const auto& row = detectionBatch[i];
					detections2D.emplace_back(row.data(), row.size());
					errors2DVec[i].resize(row.size());
					errors2DSpan.emplace_back(errors2DVec[i].data(), errors2DVec[i].size());
				}
				fsdk::Result<fsdk::FSDKError> err = det->validate(images, detections2D, errors2DSpan);
				const auto errorSize = errors2DSpan.size();
				py::list outErrors(errorSize);
				for (auto i = 0; i < errorSize; ++i) {
					const auto& row = errors2DSpan[i];
					py::list rowList(row.size());
					for (auto j = 0; j < rowList.size(); ++j) {
						rowList[j] = FSDKErrorResult(row[j]);
					}
					outErrors[i] = rowList;
				}
				return std::make_tuple(FSDKErrorResult(err), outErrors);

			 },
			"Validate input of multiple frames in a single function call.\n"
			"\tArgs:\n"
			"\t\tparam1 ([images]): source images list.\n"
			"\t\tparam1 ([detectionBatch]): detections - 2D array\n"
			"\tReturns:\n"
			"\t\t(tuple): \n"
			"\t\t\t tuple with FSDKErrorResult code, and List of List of errors)\n")

		.def("redetectOne", [](
			const fsdk::IDetectorPtr& det,
			const fsdk::Image& image,
			const fsdk::Detection& detection,
			fsdk::DetectionType type) {
				fsdk::ResultValue<fsdk::FSDKError, fsdk::Face> result = 
					det->redetectOne(image, detection, type);
				if (result.isOk()) {
					return std::make_tuple(FSDKErrorResult(result), result.getValue());
				}
				return std::make_tuple(FSDKErrorResult(result), fsdk::Face());
			}, py::arg("image"), py::arg("detection"), py::arg("type"),
			"Redetect face.\n"
			"\tArgs:\n"
			"\t\tparam1 (image): input image. Format must be R8G8B8.\n"
			"\t\tparam2 (detection): detection on image.\n"
			"\t\tparam3 (type): type of detection: BBox, 5landmarks or 68landmarks.\n"
			"\tReturns:\n"
			"\t\t(tuple): tuple with FSDKErrorResult and Face structure\n")

		.def("setDetectionComparer", [](
			const fsdk::IDetectorPtr& det,
			fsdk::DetectionComparerType comparerType) {
				det->setDetectionComparer(comparerType);
			},
			"Set detection comparer from SDK defined list\n")
		;

	py::class_<fsdk::IHumanDetectionBatchPtr>(f, "IHumanDetectionBatchPtr", "Face detection result batch structure")
		.def("getSize", [](
			const fsdk::IHumanDetectionBatchPtr& batch
		) {
			return batch->getSize();
		})

		.def("getDetections", [](
			const fsdk::IHumanDetectionBatchPtr& batch,
			size_t index) {
				const size_t rowSize = batch->getSize(index);
				py::list out(rowSize);
				const auto detections = batch->getDetections(index);
				for (size_t i = 0; i < rowSize; ++i) {
					out[i] = detections[i];
				}
				return out;
			},
			"Returns list of detections for the one image"
			"\tArgs:\n"
			"\t\tparam1 (index): index of the image"
			"\tReturns:\n"
			"\t\t(list): \n"
			"\t\t\tlist with of Detection objects\n")

		.def("getLandmarks17", [](
			const fsdk::IHumanDetectionBatchPtr& batch,
			size_t index) {
				const size_t rowSize = batch->getSize(index);
				const auto landmarks17 = batch->getLandmarks17(index);
				if (landmarks17.empty()) {
					return py::list{};
				}
				py::list out(rowSize);
				for (size_t i = 0; i < rowSize; ++i) {
					out[i] = landmarks17[i];
				}
				return out;
			},
			"Returns list of landmarks17 for the one image"
			"\tArgs:\n"
			"\t\tparam1 (index): index of the image"
			"\tReturns:\n"
			"\t\t(list): \n"
			"\t\t\tlist with of Landmarks17 objects\n")
		;


	py::class_<fsdk::Ref<fsdk::IHumanDetector>>(f, "IHumanDetectorPtr", "Human detector interface.\n")
		.def("detect", [](
			const fsdk::Ref<fsdk::IHumanDetector>& det,
			const std::vector<fsdk::Image>& images,
			const std::vector<fsdk::Rect>& rectangles,
			const uint32_t detectionPerImageNum,
			fsdk::HumanDetectionType type = fsdk::HumanDetectionType::HDT_BOX) {
				fsdk::ResultValue<fsdk::FSDKError, fsdk::Ref<fsdk::IHumanDetectionBatch>> err =
					det->detect(
						images,
						rectangles,
						detectionPerImageNum,
						type);
				if (err.isOk()) {
					return std::make_tuple(FSDKErrorResult(err), err.getValue());
				} else
					return std::make_tuple(FSDKErrorResult(err), fsdk::Ref<fsdk::IHumanDetectionBatch>{});
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
		
		.def("validate", [](
			 const fsdk::IHumanDetectorPtr& est,
			 const std::vector<fsdk::Image>& images,
			 const std::vector<fsdk::Rect>& rects,
			 const int detectionPerImageNum) {
				 std::vector<fsdk::Result<fsdk::FSDKError>> errors(images.size());
				 fsdk::Result<fsdk::FSDKError> err = est->validate(images, rects, detectionPerImageNum, errors);
				 return std::make_tuple(FSDKErrorResult(err), std::vector<FSDKErrorResult>(errors.begin(), errors.end())); 
			},
			 "Validate input of multiple frames in a single function call.\n"
			 "\tArgs:\n"
			 "\t\tparam1 (list of Images): list of Images.\n"
			 "\t\tparam2 (list of Detections): list of detections.\n"
			 "\t\tparam3 (int): max number of detections per input image.\n"
			 "\tReturns:\n"
			 "\t\t(tuple): \n"
				 "\t\t\t tuple with FSDKErrorResult code and IHumanDetectionBatchPtr object\n")

		.def("redetectOne", [](
				const fsdk::Ref<fsdk::IHumanDetector>& det,
				const fsdk::Image& image,
				const fsdk::Detection& detection,
				fsdk::HumanDetectionType type = fsdk::HumanDetectionType::HDT_BOX) {
					fsdk::ResultValue<fsdk::FSDKError, fsdk::Human> resValue = 
						det->redetectOne(image, detection, type);
					return std::make_tuple(FSDKErrorResult(resValue), resValue.getValue());
			},
			"Redetects one human based on the previous detection on the new image\n"
			"\tArgs:\n"
			"\t\tparam1 (image): image to make a redetect on\n"
			"\t\tparam2 (detection): detection from the previous frame\n"
			"\t\tparam3 (HumanDetectionType) Human detection type enumeration \n"
			"\tReturns:\n"
			"\t\t(tuple): tuple with FSDKErrorValueBool and Human structure\n")
			;

}

