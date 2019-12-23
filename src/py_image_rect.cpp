#include <pybind11/pybind11.h>
#include <fsdk/FaceEngine.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/numpy.h>
#include "ErrorsAdapter.hpp"
#include "FaceEngineAdapter.hpp"
#include "SettingsProviderAdapter.hpp"
#include "helpers.hpp"

namespace py = pybind11;

template<class T, class Y>
py::class_<fsdk::BaseRect<T>> rect_class(py::module& this_module, const char* name)
{
	py::class_<fsdk::BaseRect<T>>class_instance(this_module, name);
	
	class_instance.def(py::init<>());
	class_instance.def(py::init<T, T, T, T>());
	class_instance.def(py::init<fsdk::Vector2<T>, fsdk::Vector2<T>>());
	class_instance.def(py::init<fsdk::BaseRect<T>>());
	class_instance.def(py::init<fsdk::BaseRect<Y>>());
	class_instance.def(py::self != py::self);
	class_instance.def(py::self == py::self);
	class_instance.def(py::self & py::self);
	class_instance.def("isValid", &fsdk::BaseRect<T>::isValid);
	class_instance.def("size", &fsdk::BaseRect<T>::size);
	class_instance.def_readwrite("x", &fsdk::BaseRect<T>::x);
	class_instance.def_readwrite("y", &fsdk::BaseRect<T>::y);
	class_instance.def_readwrite("width", &fsdk::BaseRect<T>::width);
	class_instance.def_readwrite("height", &fsdk::BaseRect<T>::height);
	class_instance.def_static("coords", &fsdk::BaseRect<T>::coords);
	class_instance.def("size", &fsdk::BaseRect<T>::size);
	class_instance.def("topLeft", &fsdk::BaseRect<T>::topLeft);
	class_instance.def("center", &fsdk::BaseRect<T>::center);
	class_instance.def("bottomRight", &fsdk::BaseRect<T>::bottomRight);
	class_instance.def("top", &fsdk::BaseRect<T>::top);
	class_instance.def("bottom", &fsdk::BaseRect<T>::bottom);
	class_instance.def("left", &fsdk::BaseRect<T>::left);
	class_instance.def("right", &fsdk::BaseRect<T>::right);
	class_instance.def("set", &fsdk::BaseRect<T>::set);
	class_instance.def("adjust", &fsdk::BaseRect<T>::adjust);
	class_instance.def("adjusted", &fsdk::BaseRect<T>::adjusted);
	class_instance.def("getArea", &fsdk::BaseRect<T>::getArea);
	class_instance.def("inside", &fsdk::BaseRect<T>::inside);
	class_instance.def("isValid", &fsdk::BaseRect<T>::isValid);
	class_instance.def("set", [](fsdk::BaseRect<T>& self, const fsdk::BaseRect<Y>& other) {
		self = other;
		return self;
	});
	
	class_instance.def("__repr__",
		[](const fsdk::BaseRect<T> &r) {
			return "x = " + std::to_string(r.x) +
				", y = " + std::to_string(r.y) +
				", width = " + std::to_string(r.width) +
				", height = " + std::to_string(r.height);
		})
		;
	
	return class_instance;
}


void set_rect_class(py::module& f)
{
	auto rect = rect_class<int, float>(f, "Rect");
	auto rectFloat = rect_class<float, int>(f, "RectFloat");
}

void image_rect_module(py::module& f) {
	
	set_rect_class(f);
	
// Image type and format
py::enum_<fsdk::Format::Type>(f, "FormatType", "Format type enumeration.\n")
	.value("Unknown", fsdk::Format::Unknown)
	.value("B8G8R8X8", fsdk::Format::B8G8R8X8)
	.value("R8G8B8X8", fsdk::Format::R8G8B8X8)
	.value("B8G8R8", fsdk::Format::B8G8R8)
	.value("R8G8B8", fsdk::Format::R8G8B8)
	.value("R8", fsdk::Format::R8)
//		R16 is used for internal transformations for depth map, as i understand it can be saved as png
	.value("R16", fsdk::Format::R16,
		"\tUsed for internal transformations for depth map and "
		"cannot be saved or downloaded")
	.value("IR_X8X8X8", fsdk::Format::IR_X8X8X8,
		"\t3 channel, 8 bit per channel format with InfraRed semantics")
	;
py::class_<fsdk::Image>(f, "Image",
	"Image objects\n"
	"More detailed description see in FaceEngineSDK_Handbook.pdf or source C++ interface.\n")
	.def(py::init<>())
	.def("getWidth", &fsdk::Image::getWidth)
	.def("getHeight", &fsdk::Image::getHeight)
	.def("isValid", &fsdk::Image::isValid)
	.def("getFormat", [](const fsdk::Image& image){
		fsdk::Format::Type type = fsdk::Format::Type(image.getFormat());
		return type;
	})
	
	.def("getRect", &fsdk::Image::getRect,
		"Image rectangle.\n"
		"\tResulting rectangle top left corner is always at (0, 0).\n")
	
	.def("getData", [](const fsdk::Image& image) {
		fsdk::Format type = fsdk::Format::Type(image.getFormat());
		int c = type.getChannelCount();
		const auto* const data_uint = image.getDataAs<uint8_t>();
		std::array<ssize_t, 3> shape { image.getHeight(), image.getWidth(), c };
		auto ptr = data_uint;
		return py::array(shape, ptr);
	}, "\tReturns image as numpy array.\n")
	
	.def("getDataR16", [](const fsdk::Image& image) {
		fsdk::Format type = fsdk::Format::R16;
		int c = type.getChannelCount();
		const auto* const data_uint = image.getDataAs<uint16_t>();
		std::array<ssize_t, 3> shape { image.getHeight(), image.getWidth(), c };
		auto ptr = data_uint;
		return py::array(shape, ptr);
	}, "\tReturns image as numpy array.\n")
	
	.def("getChannelCount", [](const fsdk::Image& image) {
		fsdk::Format type = fsdk::Format::Type(image.getFormat());
		return type.getChannelCount();
	}, "\tReturns channel count.\n")

	.def("getChannelStep", [](const fsdk::Image& image) {
		return image.getFormat().getChannelStep();
	}, "\tGet channel step.Padding bytes are considered spare channels.\n")
	
	.def("getChannelSize", [](const fsdk::Image& image) {
		return image.getFormat().getChannelSize();
	})
	
	.def("getBitDepth", [](const fsdk::Image& image) {
		return image.getFormat().getBitDepth();
	})
	
	.def("getByteDepth", [](const fsdk::Image& image) {
		return image.getFormat().getByteDepth();
	})
	
	.def("computePitch", [](const fsdk::Image& image, int rowWidth) {
		return image.getFormat().computePitch(rowWidth);
	})
	
	.def("isPadded", [](const fsdk::Image& image) {
		return image.getFormat().isPadded();
	})
	
	.def("isBGR", [](const fsdk::Image& image) {
		return image.getFormat().isBGR();
	})
	
	.def("isValidFormat", [](const fsdk::Image& image) {
		return image.getFormat().isValid();
	}, "\tReturns true if image format is one of valid types, i.e. not Unknown.\n")
	
	.def("setData", [](fsdk::Image& image, py::array npImage, fsdk::Format::Type type) {
		auto size = npImage.shape();
		
		fsdk::Result<fsdk::Image::Error> error = image.set((int)size[1], (int)size[0], fsdk::Format(type), npImage.data());
		return ImageErrorResult(error);
	}, "\n\tSet image by numpy array. Please point format. example: \n"
		"\t\timage.setData(numpy_array, FaceEngine.FormatType.R8G8B8X8)\n"
		"\t\tThis method is unsafe. The responsibility for correct buffer lies on the user.\n")
	
	.def("save", [](const fsdk::Image& image, const char* path) {
		fsdk::Result<fsdk::Image::Error> error = image.save(path);
		return ImageErrorResult(error);
	})
	
	.def("save", [](const fsdk::Image& image,
					const char* path,
					const fsdk::Format::Type type) {
		fsdk::Result<fsdk::Image::Error> error = image.save(path, fsdk::Format(type));
		return ImageErrorResult(error);
	})
	.def("load",[](fsdk::Image& image, const char* path) {
		fsdk::Result<fsdk::Image::Error> error = image.load(path);
		return ImageErrorResult(error);
	})
	
	.def("save", [](const fsdk::Image& image,
		const char* path,
		fsdk::Image::ImageCompression additionalFlag) {
		fsdk::Result<fsdk::Image::Error> error = image.save(path, additionalFlag);
		return ImageErrorResult(error);
	})
	
	.def("load", [](
		fsdk::Image& image,
		const char* path,
		const fsdk::Format::Type type) {
			fsdk::Result<fsdk::Image::Error> error = image.load(path, fsdk::Format(type));
			return ImageErrorResult(error);
		})
	
	.def("loadFromMemory", [](fsdk::Image& image, const char* bytes, int sizeInBytes) {
		fsdk::Result<fsdk::Image::Error> error = image.loadFromMemory(bytes, sizeInBytes);
		return ImageErrorResult(error);
	}, "\tThis method is unsafe. The responsibility for correct buffer lies on the user.\n")
	
	.def("loadFromMemory", [](
		fsdk::Image& image,
		const char* bytes,
		int sizeInBytes,
		const fsdk::Format::Type type) {
		
			fsdk::Result<fsdk::Image::Error> error = image.loadFromMemory(
			bytes,
			sizeInBytes,
			fsdk::Format(type));
			return ImageErrorResult(error);
		}, "\tThis method is unsafe. The responsibility for correct buffer lies on the user.\n")
	
	.def("convert", [](
		const fsdk::Image& srcImage,
		const fsdk::Format::Type type) {
			fsdk::Image dest;
			fsdk::Result<fsdk::Image::Error> error = srcImage.convert(dest, fsdk::Format(type));
			return std::make_tuple(ImageErrorResult(error), dest);
		}, "\tConverts image format.\n"
			"\t\tparam1 (Format): format of destination image\n"
			"\tNote: IR_X8X8X8 is special format for infra-red images, it is not recommended to convert ordinary images to this format\n"
			"\tR16 is special format for depth images, in is not possible to convert ordinary images to this format\n"
			"\tReturns:\n"
			"\t\t(tuple with error code and image): tuple with error code and in success case new converted image\n"
			"\t\t\tMore detailed description see in FaceEngineSDK_Handbook.pdf or source C++ interface.\n")
		;

// first of all for test
f.def("loadImage", &loadImage, "used only for depth test");
f.def("loadFrameSequence", &loadFrameSequence, "used only for depth test");

py::enum_<fsdk::Image::Type>(f, "ImageType",
	"Supported image types.\n")
	.value("BMP", fsdk::Image::Type::BMP)
	.value("JPG", fsdk::Image::Type::JPG)
	.value("PNG", fsdk::Image::Type::PNG)
	.value("PPM", fsdk::Image::Type::PPM)
	.value("TIFF", fsdk::Image::Type::TIFF)
	.value("Unknown", fsdk::Image::Type::Unknown)
		;

py::enum_<fsdk::Image::Error>(f, "ImageError", "Image error codes.\n")
	.value("Ok", fsdk::Image::Error::Ok)
	.value("InvalidWidth", fsdk::Image::Error::InvalidWidth)
	.value("InvalidHeight", fsdk::Image::Error::InvalidHeight)
	.value("InvalidDataPtr", fsdk::Image::Error::InvalidDataPtr)
	.value("InvalidDataSize", fsdk::Image::Error::InvalidDataSize)
	.value("InvalidImage", fsdk::Image::Error::InvalidImage)
	.value("InvalidArchive", fsdk::Image::Error::InvalidArchive)
	.value("InvalidPath", fsdk::Image::Error::InvalidPath)
	.value("InvalidType", fsdk::Image::Error::InvalidType)
	.value("InvalidFormat", fsdk::Image::Error::InvalidFormat)
	.value("InvalidBitmap", fsdk::Image::Error::InvalidBitmap)
	.value("InvalidMemory", fsdk::Image::Error::InvalidMemory)
	.value("InvalidConversion", fsdk::Image::Error::InvalidConversion)
	.value("FailedToSave", fsdk::Image::Error::FailedToSave)
	.value("FailedToLoad", fsdk::Image::Error::FailedToLoad)
	.value("FailedToInitialize", fsdk::Image::Error::FailedToInitialize)
		;

py::enum_<fsdk::Image::ImageCompression>(f, "ImageCompression",
	"Supported compression type is used only for jpg and png types\n"
	"\t\tJPG case: the higher level of compression corresponds to the lower quality. Compression/decompression speed are constant.\n"
	"\t\tPNG case: the higher level of compression corresponds to the lower compression speed. Quality are constant.\n")
	.value("IC_NO_COMPRESSION", fsdk::Image::ImageCompression::IC_NO_COMPRESSION,
		"no compression (only for png or jpg image\n")
	.value("IC_SMALL_COMPRESSION", fsdk::Image::ImageCompression::IC_SMALL_COMPRESSION,
		"compression with minimal (or without) quality loss (only for png or jpg image)\n")
	.value("IC_MEDIUM_COMPRESSION", fsdk::Image::ImageCompression::IC_MEDIUM_COMPRESSION,
		"medium compression (only for png or jpg)\n")
	.value("IC_HARD_COMPRESSION", fsdk::Image::ImageCompression::IC_HARD_COMPRESSION,
		"more than medium compression (only for png or jpg)")
	.value("IC_BEST_COMPRESSION", fsdk::Image::ImageCompression::IC_BEST_COMPRESSION,
		"more than medium compression (only for png or jpg)")
		;
}