//
// Created by mar on 28.04.18.
//
#pragma once

#include <fsdk/FaceEngine.h>
#include "SettingsProviderAdapter.hpp"

class PyIFaceEngine {
public:
	fsdk::IFaceEnginePtr faceEnginePtr;
	
	PyIFaceEngine(const char* dataPath, const char* configPath);
	
	fsdk::FaceEngineEdition getFaceEngineEdition();

	fsdk::IDetectorPtr createDetector(
		fsdk::ObjectDetectorClassType type = fsdk::FACE_DET_DEFAULT, 
		fsdk::RecognitionMode mode = fsdk::RecognitionMode::RM_RGB);

	fsdk::IAttributeEstimatorPtr createAttributeEstimator();

	fsdk::IQualityEstimatorPtr createQualityEstimator();

	fsdk::IEthnicityEstimatorPtr createEthnicityEstimator();

//	warper
	fsdk::IWarperPtr createWarper();
	
//	descriptor
	fsdk::IDescriptorPtr createDescriptor(const uint32_t version  = 0);

	fsdk::IDescriptorBatchPtr createDescriptorBatch(int32_t size, int32_t version = 0);

	fsdk::IDescriptorExtractorPtr createExtractor(const uint32_t version = 0);

	fsdk::IDescriptorMatcherPtr createMatcher(const uint32_t version = 0);

//	second part of estimators
	fsdk::IHeadPoseEstimatorPtr createHeadPoseEstimator();

	fsdk::Ref<fsdk::IBlackWhiteEstimator> createBlackWhiteEstimator();

	fsdk::ILivenessDepthEstimatorPtr createDepthEstimator();

	fsdk::ILivenessIREstimatorPtr createIREstimator();

	fsdk::ISmileEstimatorPtr createSmileEstimator();

	fsdk::ILivenessFlowEstimatorPtr createFaceFlowEstimator();

	fsdk::IEyeEstimatorPtr createEyeEstimator(fsdk::RecognitionMode mode = fsdk::RecognitionMode::RM_RGB);

	fsdk::IEmotionsEstimatorPtr createEmotionsEstimator();

	fsdk::IGazeEstimatorPtr createGazeEstimator();
	
	fsdk::IGlassesEstimatorPtr createGlassesEstimator();
	
	fsdk::IMouthEstimatorPtr createMouthEstimator();
	
	fsdk::IIndexBuilderPtr createIndexBuilder();
	
	fsdk::Ref<fsdk::IHumanDetector> createHumanDetector();
	
	fsdk::ResultValue<fsdk::FSDKError, fsdk::IDenseIndex*> loadDenseIndex(
	const char* indexPath);
	
	fsdk::ResultValue<fsdk::FSDKError, fsdk::IDynamicIndex*> loadDynamicIndex(
	const char* indexPath);
	
	fsdk::IAGSEstimatorPtr createAGSEstimator();

	void setSettingsProvider(PyISettingsProvider& provider);
	
	fsdk::ILicensePtr getLicense();

	bool activateLicense(const fsdk::ILicensePtr& license, const char* configPath);
};
