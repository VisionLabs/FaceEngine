//
// Created by mar on 24.02.19.
//

#include <fsdk/FaceEngine.h>
#include <lsdk/LivenessEngine.h>
#include "FaceEngineAdapter.hpp"

#pragma once

class PyILivenessEngine {
public:
	lsdk::ILivenessEnginePtr livenessEnginePtr;
	
	PyILivenessEngine(const PyIFaceEngine& pyIFaceEngine, const char* dataPath);
	
	lsdk::ILivenessPtr createLiveness(lsdk::LivenessAlgorithmType type);
	
	lsdk::IComplexLivenessPtr createComplexLiveness(lsdk::ComplexLivenessAlgorithmType type);
	
	void setSettingsProvider(PyISettingsProvider& provider);
	
//	fsdk::ISettingsProviderPtr getSettingsProvider();
	
	void setDataDirectory(const char* path);
	
	const char* getDataDirectory();
	
};

