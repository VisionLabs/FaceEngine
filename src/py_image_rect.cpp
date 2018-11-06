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

void image_rect_module(py::module& f) {

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
		int c = getChannelCount(type);
		const auto* const data_uint = image.getDataAs<uint8_t>();
		std::vector<uint8_t> data(data_uint, data_uint + image.getDataSize());
		std::vector<ssize_t> shape { image.getHeight(), image.getWidth(), c };
		auto ptr = data.data();
		return py::array(shape, ptr);
	}, "\tReturns image as numpy array.\n")
	
	.def("getChannelCount", [](const fsdk::Image& image) {
		fsdk::Format type = fsdk::Format::Type(image.getFormat());
		return getChannelCount(type);
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
	
	.def("setData", [](fsdk::Image& image, py::array npImage) {
		auto size = npImage.shape();
		fsdk::Format type;
		if (size[2] == 3)
			type = fsdk::Format::R8G8B8;
		else if (size[2] == 1)
			type = fsdk::Format::R8;
		else
			throw py::cast_error("\nUnsupported types of image! Convert it to R8G8B8 or R8, or "
								"point exact format as second parameter, example: "
								"image.setData(numpy_array, FaceEngine.FormatType.R8G8B8X8)");
		image.set(size[1], size[0], type, npImage.data());
	}, "Set image by numpy array. Convert it to R8G8B8 or R8.\n")
	
	.def("setData", [](fsdk::Image& image, py::array npImage, fsdk::Format::Type type) {
		auto size = npImage.shape();
		image.set(size[1], size[0], fsdk::Format(type), npImage.data());
	}, "\n\tSet image by numpy array. Please point format. example: \n"
		"\t\timage.setData(numpy_array, FaceEngine.FormatType.R8G8B8X8)")
	
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
	})
	
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
	})
		;

// first of all for test
f.def("loadImage", &loadImage, "used only for depth test");

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
py::class_<fsdk::Rect>(f, "Rect", "Rectangle")
	.def(py::init<>())
	.def(py::init<int, int, int, int>())
	.def(py::init<fsdk::Vector2<int>, fsdk::Vector2<int>>())
	.def(py::init<const fsdk::Rect&>())
	.def(py::self != py::self)
	.def(py::self == py::self)
	.def_readwrite("x", &fsdk::Rect::x)
	.def_readwrite("y", &fsdk::Rect::y)
	.def_readwrite("width", &fsdk::Rect::width)
	.def_readwrite("height", &fsdk::Rect::height)
	.def_static("coords", &fsdk::Rect::coords)
	.def("size", &fsdk::Rect::size)
	.def("topLeft", &fsdk::Rect::topLeft)
	.def("center", &fsdk::Rect::center)
	.def("bottomRight", &fsdk::Rect::bottomRight)
	.def("top", &fsdk::Rect::top)
	.def("bottom", &fsdk::Rect::bottom)
	.def("left", &fsdk::Rect::left)
	.def("right", &fsdk::Rect::right)
	.def("set", &fsdk::Rect::set)
	.def("adjust", &fsdk::Rect::adjust)
	.def("adjusted", &fsdk::Rect::adjusted)
	.def("getArea", &fsdk::Rect::getArea)
	.def("inside", &fsdk::Rect::inside)
	.def("isValid", &fsdk::Rect::isValid)
	.def("__repr__",
		[](const fsdk::Rect &r) {
			return "x = " + std::to_string(r.x) +
					", y = " + std::to_string(r.y) +
					", width = " + std::to_string(r.width) +
					", height = " + std::to_string(r.height);
		})
	;
}