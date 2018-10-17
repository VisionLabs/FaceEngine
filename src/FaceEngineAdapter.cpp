//
// Created by mar on 28.04.18.
//

#include <iostream>
#include "FaceEngineAdapter.hpp"
#include <pybind11/pybind11.h>

namespace py = pybind11;


PyIFaceEngine::PyIFaceEngine(const char* dataPath = nullptr, const char* configPath = nullptr) {
	faceEnginePtr = fsdk::acquire(fsdk::createFaceEngine(dataPath, configPath));
}

fsdk::IDetectorPtr PyIFaceEngine::createDetector(fsdk::ObjectDetectorClassType type) {
	fsdk::IDetectorPtr detectorPtr = fsdk::acquire(faceEnginePtr->createDetector(type));
	if (!detectorPtr)
		throw py::cast_error("\nFailed to create detector instance! VERIFY PATH to \"data\" directory!");

	return detectorPtr;
}

fsdk::IAttributeEstimatorPtr PyIFaceEngine::createAttributeEstimator() {
	fsdk::IAttributeEstimatorPtr attributeEstimatorPtr = fsdk::acquire(faceEnginePtr->createAttributeEstimator());
	if (!attributeEstimatorPtr)
		throw py::cast_error("\nFailed to create attribute estimator instance! VERIFY PATH to \"data\" directory!");

	return attributeEstimatorPtr;
}

fsdk::IQualityEstimatorPtr PyIFaceEngine::createQualityEstimator() {

	fsdk::IQualityEstimatorPtr qualityEstimatorPtr = fsdk::acquire(faceEnginePtr->createQualityEstimator());
	if (!qualityEstimatorPtr) {
		throw py::cast_error("\nFailed to create quality estimator instance! VERIFY PATH to \"data\" directory!");
	}
	return qualityEstimatorPtr;

}

fsdk::IEthnicityEstimatorPtr PyIFaceEngine::createEthnicityEstimator() {
	fsdk::IEthnicityEstimatorPtr ethnicityEstimatorPtr = fsdk::acquire(faceEnginePtr->createEthnicityEstimator());
	if (!ethnicityEstimatorPtr)
		throw py::cast_error("\nFailed to create ethnicity estimator instance! VERIFY PATH to \"data\" directory!");
	return ethnicityEstimatorPtr;
}

//	warper
fsdk::IWarperPtr PyIFaceEngine::createWarper() {
	fsdk::IWarperPtr warperPtr = fsdk::acquire(faceEnginePtr->createWarper());
	if (!warperPtr)
		throw py::cast_error("\nFailed to create warper instance! VERIFY PATH to \"data\" directory!");
	return warperPtr;
}
//	descriptor
fsdk::IDescriptorPtr PyIFaceEngine::createDescriptor() {
	fsdk::IDescriptorPtr descriptorPtr = fsdk::acquire(faceEnginePtr->createDescriptor());
	if (!descriptorPtr)
		throw py::cast_error("\nFailed to create descriptor instance, possible you use front-edition version!");
	return descriptorPtr;
}

fsdk::IDescriptorBatchPtr PyIFaceEngine::createDescriptorBatch(int32_t size, int32_t version = 0) {
	fsdk::IDescriptorBatchPtr descriptorBatchPtr = fsdk::acquire(faceEnginePtr->createDescriptorBatch(size, version));
	if (!descriptorBatchPtr)
		throw py::cast_error("\nFailed to create descriptor batch instance, possible you use front-edition version!");
	return descriptorBatchPtr;
}

fsdk::IDescriptorExtractorPtr PyIFaceEngine::createExtractor() {
	fsdk::IDescriptorExtractorPtr descriptorExtractorPtr = fsdk::acquire(faceEnginePtr->createExtractor());
	if (!descriptorExtractorPtr)
		throw py::cast_error("\nFailed to create descriptor extractor instance! VERIFY PATH to \"data\" directory!");
	return descriptorExtractorPtr;
}

fsdk::IDescriptorMatcherPtr PyIFaceEngine::createMatcher() {
	fsdk::IDescriptorMatcherPtr descriptorMatcherPtr = fsdk::acquire(faceEnginePtr->createMatcher());
	if (!descriptorMatcherPtr)
		throw py::cast_error("\nFailed to create descriptor matcher instance! VERIFY PATH to \"data\" directory!");
	return descriptorMatcherPtr;
}

//	second part of estimators
fsdk::IHeadPoseEstimatorPtr PyIFaceEngine::createHeadPoseEstimator() {
	fsdk::IHeadPoseEstimatorPtr headPoseEstimatorPtr = fsdk::acquire(faceEnginePtr->createHeadPoseEstimator());
	if (!headPoseEstimatorPtr)
		throw py::cast_error("\nFailed to create head pose estimator instance! VERIFY PATH to \"data\" directory!");
	return headPoseEstimatorPtr;
}

fsdk::Ref<fsdk::IBlackWhiteEstimator> PyIFaceEngine::createBlackWhiteEstimator() {
	fsdk::Ref<fsdk::IBlackWhiteEstimator> blackWhiteEstimator = fsdk::acquire(faceEnginePtr->createBlackWhiteEstimator());
	if (!blackWhiteEstimator)
		throw py::cast_error("\nFailed to create black white estimator instance! VERIFY PATH to \"data\" directory!");
	return blackWhiteEstimator;
}

fsdk::ILivenessDepthEstimatorPtr PyIFaceEngine::createDepthEstimator() {
	fsdk::ILivenessDepthEstimatorPtr livenessDepthEstimatorPtr = fsdk::acquire(faceEnginePtr->createDepthEstimator());
	if (!livenessDepthEstimatorPtr)
		throw py::cast_error("\nFailed to create depth liveness estimator instance! VERIFY PATH to \"data\" directory!");
	return livenessDepthEstimatorPtr;
}

fsdk::ILivenessIREstimatorPtr PyIFaceEngine::createIREstimator() {
	fsdk::ILivenessIREstimatorPtr livenessIREstimatorPtr = fsdk::acquire(faceEnginePtr->createIREstimator());

	if (!livenessIREstimatorPtr)
		throw py::cast_error("\nFailed to create liveness estimator instance! VERIFY PATH to \"data\" directory!");
	return livenessIREstimatorPtr;
}

fsdk::ISmileEstimatorPtr PyIFaceEngine::createSmileEstimator() {
	fsdk::ISmileEstimatorPtr smileEstimatorPtr = fsdk::acquire(faceEnginePtr->createSmileEstimator());
	if (!smileEstimatorPtr)
		throw py::cast_error("\nFailed to create smile estimator instance! VERIFY PATH to \"data\" directory!");
	return smileEstimatorPtr;
}

fsdk::ILivenessFlowEstimatorPtr PyIFaceEngine::createFaceFlowEstimator() {
	fsdk::ILivenessFlowEstimatorPtr livenessFlowEstimatorPtr = fsdk::acquire(faceEnginePtr->createFaceFlowEstimator());
	if (!livenessFlowEstimatorPtr)
		throw py::cast_error("\nFailed to create liveness flow estimator instance! VERIFY PATH to \"data\" directory!");
	return livenessFlowEstimatorPtr;
}

fsdk::IEyeEstimatorPtr PyIFaceEngine::createEyeEstimator() {
	fsdk::IEyeEstimatorPtr eyeEstimatorPtr = fsdk::acquire(faceEnginePtr->createEyeEstimator());
	if (!eyeEstimatorPtr)
		throw py::cast_error("\nFailed to create eyes estimator instance! VERIFY PATH to \"data\" directory!");
	return eyeEstimatorPtr;
}

fsdk::IEmotionsEstimatorPtr PyIFaceEngine::createEmotionsEstimator() {
	fsdk::IEmotionsEstimatorPtr emotionsEstimatorPtr = fsdk::acquire(faceEnginePtr->createEmotionsEstimator());
	if (!emotionsEstimatorPtr)
		throw py::cast_error("\nFailed to create emotions estimator instance! VERIFY PATH to \"data\" directory!");
	return emotionsEstimatorPtr;
}

fsdk::IGazeEstimatorPtr PyIFaceEngine::createGazeEstimator() {
	fsdk::IGazeEstimatorPtr gazeEstimatorPtr = fsdk::acquire(faceEnginePtr->createGazeEstimator());
	if (!gazeEstimatorPtr)
		throw py::cast_error("\nFailed to create gaze estimator instance! VERIFY PATH to \"data\" directory!");
	return gazeEstimatorPtr;
}

fsdk::IIndexBuilderPtr PyIFaceEngine::createIndexBuilder() {
	fsdk::IIndexBuilderPtr indexBuilderPtr = fsdk::acquire(faceEnginePtr->createIndexBuilder());
	
	if (!indexBuilderPtr)
		throw py::cast_error("\nFailed to create indexBuilder instance! VERIFY PATH to \"data\" directory!");
	return indexBuilderPtr;
}

fsdk::ResultValue<fsdk::FSDKError, fsdk::IDenseIndex*> PyIFaceEngine::loadDenseIndex(
	const char* indexPath) {
	return faceEnginePtr->loadDenseIndex(indexPath);
}

fsdk::ResultValue<fsdk::FSDKError, fsdk::IDynamicIndex*> PyIFaceEngine::loadDynamicIndex(
	const char* indexPath) {
	return faceEnginePtr->loadDynamicIndex(indexPath);
}

void PyIFaceEngine::setSettingsProvider(PyISettingsProvider& provider) {
	faceEnginePtr->setSettingsProvider(provider.settingsProviderPtr);
}

// only for depth test
fsdk::Image loadImage(const char* name) {
	std::ifstream file(name, std::ios::in|std::ios::binary);

	int channels = 0;
	int elementSize = 0;
	int rows = 0;
	int cols = 0;

	file.read((char*)&channels, sizeof(channels));
	file.read((char*)&elementSize, sizeof(elementSize));
	file.read((char*)&rows, sizeof(rows));
	file.read((char*)&cols, sizeof(cols));

	fsdk::Image image(cols, rows, fsdk::Format::R16);

	file.read((char*)image.getData(), rows * cols * channels * elementSize);

	return image;
};
