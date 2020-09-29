//
// Created by mar on 24.02.19.
//

#include "LivenessEngineAdapter.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;

PyILivenessEngine::PyILivenessEngine(
	const PyIFaceEngine& pyIFaceEngine,
	const char* dataPath) {
	auto reslivenessEnginePtr = lsdk::createLivenessEngine(pyIFaceEngine.faceEnginePtr, dataPath);
	
	if (reslivenessEnginePtr.isError())
		throw py::cast_error("\nFailed to create livenessEnginePtr instance! See the \"Troubleshooting and diagnostics\" chapter in the documentation for possible reasons.");
	
	livenessEnginePtr = reslivenessEnginePtr.getValue();
}

lsdk::ILivenessPtr PyILivenessEngine::createLiveness(lsdk::LivenessAlgorithmType type) {
	auto res = livenessEnginePtr->createLiveness(type);
	if (res.isError()) {
		throw py::cast_error("\nFailed to create liveness instance! See the \"Troubleshooting and diagnostics\" chapter in the documentation for possible reasons.");
	}
	return res.getValue();
}

lsdk::IComplexLivenessPtr PyILivenessEngine::createComplexLiveness(lsdk::ComplexLivenessAlgorithmType type) {
	auto res = livenessEnginePtr->createComplexLiveness(type);
	if (res.isError()) {
		throw py::cast_error("\nFailed to create complex liveness instance! See the \"Troubleshooting and diagnostics\" chapter in the documentation for possible reasons.");
	}
	return res.getValue();
}

void PyILivenessEngine::setSettingsProvider(PyISettingsProvider& provider) {
	livenessEnginePtr->setSettingsProvider(provider.settingsProviderPtr);
}

