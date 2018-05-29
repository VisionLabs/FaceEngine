//
// Created by mar on 28.04.18.
//
#include "SettingsProviderAdapter.hpp"


PyISettingsProvider::PyISettingsProvider(const char* path = nullptr) {
	settingsProviderPtr = fsdk::acquire(fsdk::createSettingsProvider(path));
}

const char* PyISettingsProvider::getDefaultPath() {
	settingsProviderPtr->getDefaultPath();
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

void PyISettingsProvider::setValue(
		const char* section,
		const char* parameter,
		const fsdk::ISettingsProvider::Value& value) {
	settingsProviderPtr->setValue(section, parameter, value);
}

fsdk::ISettingsProvider::Value PyISettingsProvider::getValue(
		const char* section,
		const char* parameter) {
	return settingsProviderPtr->getValue(section, parameter);
}

