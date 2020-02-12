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

template<class T>
void setValue(PyISettingsProvider& provider,
	const char* section,
	const char* parameter,
	const T param);

template<class T>
void setValue(PyISettingsProvider& provider,
	const char* section,
	const char* parameter,
	const std::vector<T>& values);

py::list getValue(PyISettingsProvider& provider,
	const char* section,
	const char* parameter);

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
	
	.def("setValue", [](
		PyISettingsProvider provider,
		const char* section,
		const char* parameter,
		const fsdk::ISettingsProvider::Value& value) {
			provider.settingsProviderPtr->setValue(section, parameter, value);
	}, "Set parameter value.\n"
		"Lookup parameter by key. Creates a parameter if it does not already exist.\n"
		"Sets settings provider\n"
		"\tArgs:\n"
		"\t\tparam1 (str): parameter section\n"
		"\t\tparam2 (str): parameter name\n"
		"\t\tparam3 (value): parameter value\n")
		
	.def("setValue", [](
		PyISettingsProvider provider,
		const char* section,
		const char* parameter,
		const bool param) {
			setValue(provider, section, parameter, param);
	}, "Set parameter value.\n"
		"Lookup parameter by key. Creates a parameter if it does not already exist.\n"
		"Sets settings provider\n"
		"\tArgs:\n"
		"\t\tparam1 (str): parameter section\n"
		"\t\tparam2 (str): parameter name\n"
		"\t\tparam3 (value): parameter value\n")
	
	.def("setValue", [](
		PyISettingsProvider provider,
		const char* section,
		const char* parameter,
		const int param) {
			setValue(provider, section, parameter, param);
	}, "Set parameter value.\n"
		"Lookup parameter by key. Creates a parameter if it does not already exist.\n"
		"Sets settings provider\n"
		"\tArgs:\n"
		"\t\tparam1 (str): parameter section\n"
		"\t\tparam2 (str): parameter name\n"
		"\t\tparam3 (value): parameter value\n")
	
	.def("setValue", [](
		PyISettingsProvider provider,
		const char* section,
		const char* parameter,
		const char* param) {
			setValue(provider, section, parameter, param);
	}, "Set parameter value.\n"
		"Lookup parameter by key. Creates a parameter if it does not already exist.\n"
		"Sets settings provider\n"
		"\tArgs:\n"
		"\t\tparam1 (str): parameter section\n"
		"\t\tparam2 (str): parameter name\n"
		"\t\tparam3 (value): parameter value\n")
	
	.def("setValue", [](
		PyISettingsProvider provider,
		const char* section,
		const char* parameter,
		const float param) {
			setValue(provider, section, parameter, param);
	}, "Set parameter value.\n"
		"Lookup parameter by key. Creates a parameter if it does not already exist.\n"
		"Sets settings provider\n"
		"\tArgs:\n"
		"\t\tparam1 (str): parameter section\n"
		"\t\tparam2 (str): parameter name\n"
		"\t\tparam3 (value): parameter value\n")
	
	.def("setValue", [](
		PyISettingsProvider provider,
		const char* section,
		const char* parameter,
		const std::vector<int>& params) {
			setValue(provider, section, parameter, params);
	}, "Set parameter value.\n"
		"Lookup parameter by key. Creates a parameter if it does not already exist.\n"
		"Sets settings provider\n"
		"\tArgs:\n"
		"\t\tparam1 (str): parameter section\n"
		"\t\tparam2 (str): parameter name\n"
		"\t\tparam3 (list): list of values\n")
	
	.def("setValue", [](
		PyISettingsProvider provider,
		const char* section,
		const char* parameter,
		const std::vector<float>& params) {
			setValue(provider, section, parameter, params);
	}, "Set parameter value.\n"
		"Lookup parameter by key. Creates a parameter if it does not already exist.\n"
		"Sets settings provider\n"
		"\tArgs:\n"
		"\t\tparam1 (str): parameter section\n"
		"\t\tparam2 (str): parameter name\n"
		"\t\tparam3 (list): list of values\n")
	
	.def("getValue", [](
		PyISettingsProvider provider,
		const char* section,
		const char* parameter) {
			return getValue(provider, section, parameter);
	},
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
		; // SettingsProviderValue
}

template<class T>
void setValue(PyISettingsProvider& provider,
	const char* section,
	const char* parameter,
	const T param) {
		fsdk::ISettingsProvider::Value value(param);
		provider.settingsProviderPtr->setValue(section, parameter, value);
}

template<class T>
void setValue(PyISettingsProvider& provider,
	const char* section,
	const char* parameter,
	const std::vector<T>& values) {
		fsdk::ISettingsProvider::Value value;
		if (values.size() == 1)
			value = fsdk::ISettingsProvider::Value(values[0]);
		else if (values.size() == 2) {
			value = fsdk::ISettingsProvider::Value(values[0], values[1]);
		} else if (values.size() == 3) {
			value = fsdk::ISettingsProvider::Value(values[0], values[1], values[2]);
		} else if (values.size() == 4) {
			value = fsdk::ISettingsProvider::Value(values[0], values[1], values[2], values[3]);
		}
		
		provider.settingsProviderPtr->setValue(section, parameter, value);
}

py::list getValue(PyISettingsProvider& provider,
	const char* section,
	const char* parameter) {
		auto value = provider.settingsProviderPtr->getValue(section, parameter);
		using valueType = fsdk::ISettingsProvider::Value::Type;
		py::list pyValueList;
		
		switch (value.m_type) {
			
			case valueType::Int1: {
				int val = value.asInt();
				pyValueList.append(val);
				break;
			}
			
			case valueType::Int2: {
				int x[2];
				bool result = value.getInt(&x[0], &x[1]);
				if (result) {
					pyValueList.append(x[0]);
					pyValueList.append(x[1]);
				}
				break;
			}
			
			case valueType::Int3: {
				int x[3];
				bool result = value.getInt(&x[0], &x[1], &x[2]);
				if (result) {
					pyValueList.append(x[0]);
					pyValueList.append(x[1]);
					pyValueList.append(x[2]);
				}
				break;
			}
			
			case valueType::Int4: {
				int x[4];
				bool result = value.getInt(&x[0], &x[1], &x[2], &x[3]);
				if (result) {
					pyValueList.append(x[0]);
					pyValueList.append(x[1]);
					pyValueList.append(x[2]);
					pyValueList.append(x[3]);
				}
				break;
			}
			
			case valueType::Float1: {
				float val = value.asFloat();
				pyValueList.append(val);
			}
			
			case valueType::Float2: {
				float x[2];
				bool result = value.getFloat(&x[0], &x[1]);
				if (result) {
					pyValueList.append(x[0]);
					pyValueList.append(x[1]);
				}
				break;
			}
			
			case valueType::Float3: {
				float x[3];
				bool result = value.getFloat(&x[0], &x[1], &x[2]);
				if (result) {
					pyValueList.append(x[0]);
					pyValueList.append(x[1]);
					pyValueList.append(x[2]);
				}
				break;
			}
			
			case valueType::Float4: {
				float x[4];
				bool result = value.getFloat(&x[0], &x[1], &x[2], &x[3]);
				if (result) {
					pyValueList.append(x[0]);
					pyValueList.append(x[1]);
					pyValueList.append(x[2]);
					pyValueList.append(x[3]);
				}
				break;
			}
			
			case valueType::String: {
				const std::string str = value.asString();
				pyValueList.append(str);
				break;
			}
		}
		return pyValueList;
}