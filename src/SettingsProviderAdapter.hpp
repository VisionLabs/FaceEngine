//
// Created by mar on 28.04.18.
//
#include <fsdk/FaceEngine.h>
#include "ErrorsAdapter.hpp"
#pragma once

class PyISettingsProvider {
public:
	fsdk::ISettingsProviderPtr settingsProviderPtr;

	PyISettingsProvider(const char* path);

	const char* getDefaultPath();

	SettingsProviderResult load(const char* path);

	bool save(const char* path);

	void clear();

	void isEmpty();

	void setValue(
	const char* section,
	const char* parameter,
	const fsdk::ISettingsProvider::Value& value);

	fsdk::ISettingsProvider::Value getValue(const char* section, const char* parameter);
};

