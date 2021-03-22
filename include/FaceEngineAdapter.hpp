//
// Created by mar on 28.04.18.
//
#pragma once

#include <fsdk/FaceEngine.h>
#include "SettingsProviderAdapter.hpp"

class PyIFaceEngine {
public:
	fsdk::IFaceEnginePtr faceEnginePtr;
	
	PyIFaceEngine(const char* dataPath, const char* configPath, const char* runtimeConfigPath);
	
	fsdk::FaceEngineEdition getFaceEngineEdition();

	fsdk::IDetectorPtr createDetector(
		fsdk::ObjectDetectorClassType type = fsdk::FACE_DET_DEFAULT, 
		fsdk::SensorType mode = fsdk::SensorType::Visible);

	fsdk::IAttributeEstimatorPtr createAttributeEstimator();

	fsdk::IQualityEstimatorPtr createQualityEstimator();

	fsdk::IBestShotQualityEstimatorPtr createBestShotQualityEstimator();
	
//	warper
	fsdk::IWarperPtr createWarper();
	
	fsdk::IHumanWarperPtr createHumanWarper();
	
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

	fsdk::ILivenessFlyingFacesEstimatorPtr createLivenessFlyingFacesEstimator();

	fsdk::ILivenessFPREstimatorPtr createLivenessFPREstimator();

	fsdk::ILivenessRGBMEstimatorPtr createLivenessRGBMEstimator();

	fsdk::ILivenessOneShotRGBEstimatorPtr createLivenessOneShotRGBEstimator();

	fsdk::ILivenessFlowEstimatorPtr createFaceFlowEstimator();

	fsdk::IEyeEstimatorPtr createEyeEstimator(fsdk::SensorType mode = fsdk::SensorType::Visible);

	fsdk::IEmotionsEstimatorPtr createEmotionsEstimator();

	fsdk::IGazeEstimatorPtr createGazeEstimator(fsdk::SensorType mode = fsdk::SensorType::Visible);
	
	fsdk::IGlassesEstimatorPtr createGlassesEstimator();
	
	fsdk::IMouthEstimatorPtr createMouthEstimator();
	
	fsdk::IMedicalMaskEstimatorPtr createMedicalMaskEstimator();
	
	fsdk::IOverlapEstimatorPtr createOverlapEstimator();

	fsdk::ICredibilityCheckEstimatorPtr createCredibilityCheckEstimator();
	
	fsdk::IIndexBuilderPtr createIndexBuilder(
		const uint32_t capacity = 0,
		const uint32_t version = 0);
	
	fsdk::Ref<fsdk::IHumanDetector> createHumanDetector();

	fsdk::Ref<fsdk::IOrientationEstimator> createOrientationEstimator();
	
	fsdk::ResultValue<fsdk::FSDKError, fsdk::IDenseIndexPtr> loadDenseIndex(
	const char* indexPath);
	
	fsdk::ResultValue<fsdk::FSDKError, fsdk::IDynamicIndexPtr> loadDynamicIndex(
	const char* indexPath);
	
	fsdk::IAGSEstimatorPtr createAGSEstimator();
	
	fsdk::IPPEEstimatorPtr createPPEEstimator();

	void setSettingsProvider(PyISettingsProvider& provider);
	
	PyISettingsProvider getSettingsProvider();
	
	PyISettingsProvider getRuntimeSettingsProvider();
	
	void setRuntimeSettingsProvider(PyISettingsProvider& provider);

	fsdk::ILicensePtr getLicense();

	fsdk::Result<fsdk::FSDKError> activateLicense(const fsdk::ILicensePtr& license, const char* configPath);
};
