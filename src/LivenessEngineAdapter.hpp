//
// Created by mar on 24.02.19.
//

#pragma once

#include "FaceEngineAdapter.hpp"
#include <fsdk/FaceEngine.h>
#include <lsdk/LivenessEngine.h>

class PyILivenessEngine {

private:
	lsdk::ILivenessEnginePtr livenessEnginePtr;
	
public:
	PyILivenessEngine(const PyIFaceEngine& pyIFaceEngine, const char* dataPath);
	
	lsdk::ILivenessPtr createLiveness(lsdk::LivenessAlgorithmType type);
	
	lsdk::IComplexLivenessPtr createComplexLiveness(lsdk::ComplexLivenessAlgorithmType type);
	
	void setSettingsProvider(PyISettingsProvider& provider);
	
};

