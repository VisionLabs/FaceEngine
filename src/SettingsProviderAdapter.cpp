//
// Created by mar on 28.04.18.
//
#include "SettingsProviderAdapter.hpp"
#include <pybind11/detail/common.h>

namespace py = pybind11;

PyISettingsProvider::PyISettingsProvider(const char* path = nullptr) : settingsProviderPtr(fsdk::createSettingsProvider(path).getValue()) {
	if (!settingsProviderPtr)
		throw py::cast_error("\nFailed to Setting Provider instance! See the \"Troubleshooting and diagnostics\" chapter in the documentation for possible reasons.");
}

PyISettingsProvider::PyISettingsProvider(const fsdk::ISettingsProviderPtr& config) :
	settingsProviderPtr(config) {
	if (!config)
		throw py::cast_error("\nFailed to Setting Provider instance! See the \"Troubleshooting and diagnostics\" chapter in the documentation for possible reasons.");
}

const char* PyISettingsProvider::getDefaultPath() {
	return settingsProviderPtr->getDefaultPath();
}

SettingsProviderResult PyISettingsProvider::load(const char* path) {
	return SettingsProviderResult(settingsProviderPtr->load(path));
}

bool PyISettingsProvider::save(const char* path) {
	return settingsProviderPtr->save(path);
}

void PyISettingsProvider::clear() {
	settingsProviderPtr->clear();
}

bool PyISettingsProvider::isEmpty() {
	return settingsProviderPtr->isEmpty();
}