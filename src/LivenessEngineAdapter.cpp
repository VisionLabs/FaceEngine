//
// Created by mar on 24.02.19.
//

#include "LivenessEngineAdapter.hpp"
#include <pybind11/pybind11.h>


namespace py = pybind11;

PyILivenessEngine::PyILivenessEngine(
	const PyIFaceEngine& pyIFaceEngine,
	const char* dataPath) {
	livenessEnginePtr = fsdk::acquire(lsdk::createLivenessEngine(pyIFaceEngine.faceEnginePtr, dataPath));
	if (!livenessEnginePtr)
		throw py::cast_error("\nFailed to create livenessEnginePtr instance! See the \"Troubleshooting and diagnostics\" chapter in the documentation for possible reasons.");
}

lsdk::ILivenessPtr PyILivenessEngine::createLiveness(lsdk::LivenessAlgorithmType type) {
	lsdk::ILivenessPtr livenessPtr = fsdk::acquire(livenessEnginePtr->createLiveness(type));
	if (!livenessPtr)
		throw py::cast_error("\nFailed to create liveness instance! See the \"Troubleshooting and diagnostics\" chapter in the documentation for possible reasons.");
	return livenessPtr;
}

lsdk::IComplexLivenessPtr PyILivenessEngine::createComplexLiveness(lsdk::ComplexLivenessAlgorithmType type) {
	lsdk::IComplexLivenessPtr livenessPtr = fsdk::acquire(livenessEnginePtr->createComplexLiveness(type));
	if (!livenessPtr)
		throw py::cast_error("\nFailed to create complex liveness instance! See the \"Troubleshooting and diagnostics\" chapter in the documentation for possible reasons.");
	return livenessPtr;
}

void PyILivenessEngine::setSettingsProvider(PyISettingsProvider& provider) {
	livenessEnginePtr->setSettingsProvider(provider.settingsProviderPtr);
}

