//
// Created by mar on 28.04.18.
//

#include <iostream>
#include "FaceEngineAdapter.hpp"


PyIFaceEngine::PyIFaceEngine(const char* dataPath = nullptr, const char* configPath = nullptr) {
	faceEnginePtr = fsdk::acquire(fsdk::createFaceEngine(dataPath, configPath));
	if (!faceEnginePtr)
		std::cerr << "Warning: cannot create FaceEngine instance! VERIFY PATH to \"data\" directory!" << std::endl;
}

fsdk::IDetectorPtr PyIFaceEngine::createDetector(fsdk::ObjectDetectorClassType type) {
	fsdk::IDetectorPtr detectorPtr = fsdk::acquire(faceEnginePtr->createDetector(type));
	if (!detectorPtr)
		std::cerr << "Warning: cannot create detector instance! VERIFY PATH to \"data\" directory!" << std::endl;
	return detectorPtr;
}

fsdk::IAttributeEstimatorPtr PyIFaceEngine::createAttributeEstimator() {
	fsdk::IAttributeEstimatorPtr attributeEstimatorPtr = fsdk::acquire(faceEnginePtr->createAttributeEstimator());
	if (!attributeEstimatorPtr)
		std::cerr << "Warning: cannot create attribute estimator instance! VERIFY PATH to \"data\" directory!" << std::endl;
	return attributeEstimatorPtr;
}

fsdk::IQualityEstimatorPtr PyIFaceEngine::createQualityEstimator() {

	fsdk::IQualityEstimatorPtr qualityEstimatorPtr = fsdk::acquire(faceEnginePtr->createQualityEstimator());
	if (!qualityEstimatorPtr)
		std::cerr << "Warning: cannot create quality estimator instance! VERIFY PATH to \"data\" directory!" << std::endl;
	return qualityEstimatorPtr;

}

fsdk::IEthnicityEstimatorPtr PyIFaceEngine::createEthnicityEstimator() {
	fsdk::IEthnicityEstimatorPtr ethnicityEstimatorPtr = fsdk::acquire(faceEnginePtr->createEthnicityEstimator());
	if (!ethnicityEstimatorPtr)
		std::cerr << "Warning: cannot create ethnicity estimator instance! VERIFY PATH to \"data\" directory!" << std::endl;
	return ethnicityEstimatorPtr;
}

//	warper
fsdk::IWarperPtr PyIFaceEngine::createWarper() {
	fsdk::IWarperPtr warperPtr = fsdk::acquire(faceEnginePtr->createWarper());
	if (!warperPtr)
		std::cerr << "Warning: cannot create warper instance! VERIFY PATH to \"data\" directory!" << std::endl;
	return warperPtr;
}
//	descriptor
fsdk::IDescriptorPtr PyIFaceEngine::createDescriptor() {
	return fsdk::acquire(faceEnginePtr->createDescriptor());
}

fsdk::IDescriptorBatchPtr PyIFaceEngine::createDescriptorBatch(int32_t size, int32_t version = 0) {
	return fsdk::acquire(faceEnginePtr->createDescriptorBatch(size, version));
}

fsdk::IDescriptorExtractorPtr PyIFaceEngine::createExtractor() {
	fsdk::IDescriptorExtractorPtr descriptorExtractorPtr = fsdk::acquire(faceEnginePtr->createExtractor());
	if (!descriptorExtractorPtr)
		std::cerr << "Warning: cannot create descriptor extractor instance! VERIFY PATH to \"data\" directory!" << std::endl;
	return descriptorExtractorPtr;
}

fsdk::IDescriptorMatcherPtr PyIFaceEngine::createMatcher() {
	fsdk::IDescriptorMatcherPtr descriptorMatcherPtr = fsdk::acquire(faceEnginePtr->createMatcher());
	if (!descriptorMatcherPtr)
		std::cerr << "Warning: cannot create descriptor matcher instance! VERIFY PATH to \"data\" directory!" << std::endl;
	return descriptorMatcherPtr;
}

fsdk::ILSHTablePtr PyIFaceEngine::createLSHTable(const fsdk::IDescriptorBatchPtr& batch) {
	fsdk::ILSHTablePtr ilshTablePtr = fsdk::acquire(faceEnginePtr->createLSHTable(batch));
	if (!ilshTablePtr)
		std::cerr << "Warning: cannot create ILSHTable instance! VERIFY PATH to \"data\" directory!" << std::endl;
	return ilshTablePtr;
}

//	second part of estimators
fsdk::IHeadPoseEstimatorPtr PyIFaceEngine::createHeadPoseEstimator() {
	fsdk::IHeadPoseEstimatorPtr headPoseEstimatorPtr = fsdk::acquire(faceEnginePtr->createHeadPoseEstimator());
	if (!headPoseEstimatorPtr)
		std::cerr << "Warning: cannot create head pose estimator instance! VERIFY PATH to \"data\" directory!" << std::endl;
	return headPoseEstimatorPtr;
}

fsdk::Ref<fsdk::IBlackWhiteEstimator> PyIFaceEngine::createBlackWhiteEstimator() {
	fsdk::Ref<fsdk::IBlackWhiteEstimator> blackWhiteEstimator = fsdk::acquire(faceEnginePtr->createBlackWhiteEstimator());
	if (!blackWhiteEstimator)
		std::cerr << "Warning: cannot create black white estimator instance! VERIFY PATH to \"data\" directory!" << std::endl;
	return blackWhiteEstimator;
}

fsdk::ILivenessDepthEstimatorPtr PyIFaceEngine::createDepthEstimator() {
	fsdk::ILivenessDepthEstimatorPtr livenessDepthEstimatorPtr = fsdk::acquire(faceEnginePtr->createDepthEstimator());
	if (!livenessDepthEstimatorPtr)
		std::cerr << "Warning: cannot create depth liveness estimator instance! VERIFY PATH to \"data\" directory!" << std::endl;
	return livenessDepthEstimatorPtr;
}

fsdk::ILivenessIREstimatorPtr PyIFaceEngine::createIREstimator() {
	fsdk::ILivenessIREstimatorPtr livenessIREstimatorPtr = fsdk::acquire(faceEnginePtr->createIREstimator());

	if (!livenessIREstimatorPtr)
		std::cerr << "Warning: cannot create liveness estimator instance! VERIFY PATH to \"data\" directory!" << std::endl;
	return livenessIREstimatorPtr;
}

fsdk::ISmileEstimatorPtr PyIFaceEngine::createSmileEstimator() {
	fsdk::ISmileEstimatorPtr smileEstimatorPtr = fsdk::acquire(faceEnginePtr->createSmileEstimator());
	if (!smileEstimatorPtr)
		std::cerr << "Warning: cannot create smile estimator instance! VERIFY PATH to \"data\" directory!" << std::endl;
	return smileEstimatorPtr;
}

fsdk::ILivenessFlowEstimatorPtr PyIFaceEngine::createFaceFlowEstimator() {
	fsdk::ILivenessFlowEstimatorPtr livenessFlowEstimatorPtr = fsdk::acquire(faceEnginePtr->createFaceFlowEstimator());
	if (!livenessFlowEstimatorPtr)
		std::cerr << "Warning: cannot create liveness flow estimator instance! VERIFY PATH to \"data\" directory!" << std::endl;
	return livenessFlowEstimatorPtr;
}

fsdk::IEyeEstimatorPtr PyIFaceEngine::createEyeEstimator() {
	fsdk::IEyeEstimatorPtr eyeEstimatorPtr = fsdk::acquire(faceEnginePtr->createEyeEstimator());
	if (!eyeEstimatorPtr)
		std::cerr << "Warning: cannot create eyes estimator instance! VERIFY PATH to \"data\" directory!" << std::endl;
	return eyeEstimatorPtr;
}

fsdk::IEmotionsEstimatorPtr PyIFaceEngine::createEmotionsEstimator() {
	fsdk::IEmotionsEstimatorPtr emotionsEstimatorPtr = fsdk::acquire(faceEnginePtr->createEmotionsEstimator());
	if (!emotionsEstimatorPtr)
		std::cerr << "Warning: cannot create emotions estimator instance! VERIFY PATH to \"data\" directory!" << std::endl;
	return emotionsEstimatorPtr;
}

fsdk::IGazeEstimatorPtr PyIFaceEngine::createGazeEstimator() {
	fsdk::IGazeEstimatorPtr gazeEstimatorPtr = fsdk::acquire(faceEnginePtr->createGazeEstimator());
	if (!gazeEstimatorPtr)
		std::cerr << "Warning: cannot create gaze estimator instance! VERIFY PATH to \"data\" directory!" << std::endl;
	return gazeEstimatorPtr;
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
