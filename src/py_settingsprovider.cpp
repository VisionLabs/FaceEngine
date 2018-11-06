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

void settings_provider_module(py::module& f) {

// ISettingsProvider
py::class_<PyISettingsProvider>(f, "PyISettingsProvider")
	.def("getDefaultPath", &PyISettingsProvider::getDefaultPath,
		"Get settings path this provider is bound to.\n"
		"\tThis is the same path that was given to load().\n"
		"\tReturns path string.\n")
	
	.def("load", &PyISettingsProvider::load,
		"Load settings from given path.\n"
		"if `path` is null, load from the default path; @see getDefaultPath().\n"
		"\tReturns result with error code specified by ISettingsProvider\n")
	
	.def("save", &PyISettingsProvider::save,
		"Save settings values using the default path.\n"
		"Path may be null, in this case a path from getDefaultPath() will be used.\n"
		"\tReturns true if succeded, false otherwise.\n")
	
	.def("clear", &PyISettingsProvider::clear, "Clear settings.\n")
	
	.def("isEmpty", &PyISettingsProvider::isEmpty,
		"Check if there are loaded settings.\n"
		"Returns true if provider is empty.\n")
	
	.def("setValue", &PyISettingsProvider::setValue, "Set parameter value.\n"
	"Lookup parameter by key. Creates a parameter if it does not already exist.\n"
	"Sets settings provider\n"
	"\tArgs:\n"
	"\t\tparam1 (str): parameter section\n"
	"\t\tparam2 (str): parameter name\n"
	"\t\tparam3 (value): parameter value\n")
	
	.def("getValue", &PyISettingsProvider::getValue,
		"Get parameter value. Lookup parameter by key. Return empty value if the parameters does not exist.\n"
		"\tArgs:\n"
		"\t\tparam1 (str): parameter section\n"
		"\t\tparam2 (str): parameter name\n")
	; // PyISettingsProvider
	
py::class_<fsdk::ISettingsProvider::Value>(f, "SettingsProviderValue", "Configuration parameter value.\n")
	.def(py::init<>(), "Initialize an empty value. Value type will be set to `Undefined`")
	.def(py::init<int>(), "Initialize an integer value")
	.def(py::init<int, int>(), "Initialize a 2d integer value")
	.def(py::init<int, int, int>(), "Initialize a 3d integer value")
	.def(py::init<int, int, int, int>(), "Initialize a 4d integer value")
	.def(py::init<float>(), "Initialize a float value")
	.def(py::init<float, float>(), "Initialize a 2d float value")
	.def(py::init<float, float, float>(), "Initialize a 3d float value")
	.def(py::init<float, float, float, float>(), "Initialize a 4d float value")
	.def(py::init<const char*>(), "Initialize a string value. Note: only short strings (<15 chars) are supported.\n")
	.def(py::init<const fsdk::Rect&>(), "Initialize a rect value")
	.def(py::init<bool>(), "Initialize a bool value")
	
	.def("asFloat", &fsdk::ISettingsProvider::Value::asFloat, py::arg("defaultValue") = 0.f,
		"Safely get a float. If actual value type is float, the value is returned; "
		"if not a fallback value is returned"
		"\tArgs:\n"
		"\t\tparam1 (float): [optional] fallback value\n")
	
	.def("asBool", &fsdk::ISettingsProvider::Value::asBool, py::arg("defaultValue") = false,
		"Safely get a boolean. If actual value type is convertible to bool, "
		"the value is returned; if not a fallback value is returned.\n"
		"\tArgs:\n"
		"\t\tparam1 (bool): [optional] fallback value\n")
	
	.def("asInt", &fsdk::ISettingsProvider::Value::asInt, py::arg("defaultValue") = 0,
		"Safely get an integer. If actual value type is Int, the value is returned; "
		"if not a fallback value is returned.\n"
		"\tArgs:\n"
		"\t\tparam1 (int): [optional] fallback value\n")
	
	.def("asString", &fsdk::ISettingsProvider::Value::asString, py::arg("defaultValue") = "",
		"Safely get a string. If actual value type is String, the value is returned; "
		"if not a fallback value is returned. Note: doesn't allocate or copy memory.\n"
		"\tArgs:\n"
		"\t\tparam1 (int): [optional] fallback value\n")
	
	.def("asRect", []( const fsdk::ISettingsProvider::Value& v) {
		return v.asRect(); })
	.def("asRect", [](
		const fsdk::ISettingsProvider::Value& v, const fsdk::Rect& r) {
			return v.asRect(r); },
		"Safely get a Rect. If actual value type is convertible to Rect, the value is returned; "
		"if not a fallback value is returned\n"
		"\tArgs:\n"
		"\t\tparam1 (rect): [optional] fallback value\n")
	
	.def("asPoint2f", [](
		const fsdk::ISettingsProvider::Value& v) {
		return v.asPoint2f(); })
	.def("asPoint2f", [](
		const fsdk::ISettingsProvider::Value& v, const fsdk::Point2f& r) {
			return v.asPoint2f(r); },
		"Safely get a Point2f. If actual value type is convertible to Point2f, the value is returned; \n"
		"\tif not a fallback value is returned.\n"
		"\tArgs:\n"
		"\t\tparam1 (Vector2f): [optional] fallback value\n")
	
	.def("asPoint2i", [](
		const fsdk::ISettingsProvider::Value& v) {
		return v.asPoint2i(); })
	.def("asPoint2i", [](
		const fsdk::ISettingsProvider::Value& v, const fsdk::Point2i& r) {
			return v.asPoint2i(r); },
		"Safely get a Point2i. If actual value type is convertible to Point2i, the value is returned; \n"
		"\tif not a fallback value is returned.\n"
		"\tArgs:\n"
		"\t\tparam1 (Vector2i): [optional] fallback value\n")
	
	.def("asSize", [](
		const fsdk::ISettingsProvider::Value& v) {
		return v.asSize(); })
	.def("asSize", [](
		const fsdk::ISettingsProvider::Value& v, const fsdk::Size& s) {
		
			return v.asSize(s); },
		"Safely get a Size. If actual value type is convertible to Size, the value is returned;\n"
		"\tif not a fallback value is returned.\n"
		"\tArgs:\n"
		"\t\tparam1 (Vector2i): [optional] fallback value\n")
		; // SettingsProviderValue
}