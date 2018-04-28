//
// Created by mar on 28.04.18.
//

#include "FaceEngineAdapter.hpp"


PyIFaceEngine::PyIFaceEngine(const char* dataPath = nullptr, const char* configPath = nullptr) {
	faceEnginePtr = fsdk::acquire(fsdk::createFaceEngine(dataPath, configPath));
}

fsdk::IDetectorPtr PyIFaceEngine::createDetector(fsdk::ObjectDetectorClassType type) {
	return fsdk::acquire(faceEnginePtr->createDetector(type));
}

fsdk::IAttributeEstimatorPtr PyIFaceEngine::createAttributeEstimator() {
	return fsdk::acquire(faceEnginePtr->createAttributeEstimator());
}

fsdk::IQualityEstimatorPtr PyIFaceEngine::createQualityEstimator() {
	return fsdk::acquire(faceEnginePtr->createQualityEstimator());

}

fsdk::IEthnicityEstimatorPtr PyIFaceEngine::createEthnicityEstimator() {
	return fsdk::acquire(faceEnginePtr->createEthnicityEstimator());
}

//	warper
fsdk::IWarperPtr PyIFaceEngine::createWarper() {
	return fsdk::acquire(faceEnginePtr->createWarper());
}
//	descriptor
fsdk::IDescriptorPtr PyIFaceEngine::createDescriptor() {
	return fsdk::acquire(faceEnginePtr->createDescriptor());
}

fsdk::IDescriptorBatchPtr PyIFaceEngine::createDescriptorBatch(int32_t size, int32_t version = 0) {
	return fsdk::acquire(faceEnginePtr->createDescriptorBatch(size, version));
}

fsdk::IDescriptorExtractorPtr PyIFaceEngine::createExtractor() {
	return fsdk::acquire(faceEnginePtr->createExtractor());
}

fsdk::IDescriptorMatcherPtr PyIFaceEngine::createMatcher() {
	return fsdk::acquire(faceEnginePtr->createMatcher());
}

fsdk::ILSHTablePtr PyIFaceEngine::createLSHTable(const fsdk::IDescriptorBatchPtr& batch) {
	return fsdk::acquire(faceEnginePtr->createLSHTable(batch));
}

//	second part of estimators
fsdk::IHeadPoseEstimatorPtr PyIFaceEngine::createHeadPoseEstimator() {
	return fsdk::acquire(faceEnginePtr->createHeadPoseEstimator());
}

fsdk::Ref<fsdk::IBlackWhiteEstimator> PyIFaceEngine::createBlackWhiteEstimator() {
	return fsdk::acquire(faceEnginePtr->createBlackWhiteEstimator());
}

fsdk::ILivenessDepthEstimatorPtr PyIFaceEngine::createDepthEstimator() {
	return fsdk::acquire(faceEnginePtr->createDepthEstimator());
}

fsdk::ILivenessIREstimatorPtr PyIFaceEngine::createIREstimator() {
	return fsdk::acquire(faceEnginePtr->createIREstimator());
}

fsdk::ISmileEstimatorPtr PyIFaceEngine::createSmileEstimator() {
	return fsdk::acquire(faceEnginePtr->createSmileEstimator());
}

fsdk::ILivenessFlowEstimatorPtr PyIFaceEngine::createFaceFlowEstimator() {
	return fsdk::acquire(faceEnginePtr->createFaceFlowEstimator());
}

fsdk::IEyeEstimatorPtr PyIFaceEngine::createEyeEstimator() {
	return fsdk::acquire(faceEnginePtr->createEyeEstimator());
}

fsdk::IEmotionsEstimatorPtr PyIFaceEngine::createEmotionsEstimator() {
	return fsdk::acquire(faceEnginePtr->createEmotionsEstimator());
}

fsdk::IGazeEstimatorPtr PyIFaceEngine::createGazeEstimator() {
	return fsdk::acquire(faceEnginePtr->createGazeEstimator());
}

void PyIFaceEngine::setSettingsProvider(PyISettingsProvider& provider) {
	faceEnginePtr->setSettingsProvider(provider.settingsProviderPtr);
}

// only for depth test
fsdk::Image loadImage(const char* name) {
	std::ifstream file(name, std::ios::in|std::ios::binary);

	int channels =0;
	int elementSize = 0;
	int rows = 0;
	int cols = 0;

	file.read((char*)&channels,sizeof(channels));
	file.read((char*)&elementSize,sizeof(elementSize));
	file.read((char*)&rows,sizeof(rows));
	file.read((char*)&cols,sizeof(cols));

	fsdk::Image image(cols,rows,fsdk::Format::R16);

	file.read((char*)image.getData(),rows*cols*channels*elementSize);

	return image;
};
