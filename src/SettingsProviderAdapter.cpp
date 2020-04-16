//
// Created by mar on 28.04.18.
//
#include "SettingsProviderAdapter.hpp"

PyISettingsProvider::PyISettingsProvider(const char* path = nullptr) :
	settingsProviderPtr(fsdk::acquire(fsdk::createSettingsProvider(path))) {}

PyISettingsProvider::PyISettingsProvider(const fsdk::ISettingsProviderPtr& config) :
	settingsProviderPtr(config) {}

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