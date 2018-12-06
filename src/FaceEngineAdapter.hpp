//
// Created by mar on 28.04.18.
//
#include <fsdk/FaceEngine.h>
#include "SettingsProviderAdapter.hpp"

class PyIFaceEngine {
public:
	fsdk::IFaceEnginePtr faceEnginePtr;
	
	PyIFaceEngine(const char* dataPath, const char* configPath);
	
	fsdk::FaceEngineEdition getFaceEngineEdition();

	fsdk::IDetectorPtr createDetector(fsdk::ObjectDetectorClassType type);

	fsdk::IAttributeEstimatorPtr createAttributeEstimator();

	fsdk::IQualityEstimatorPtr createQualityEstimator();

	fsdk::IEthnicityEstimatorPtr createEthnicityEstimator();

//	warper
	fsdk::IWarperPtr createWarper();
//	descriptor
	fsdk::IDescriptorPtr createDescriptor();

	fsdk::IDescriptorBatchPtr createDescriptorBatch(int32_t size, int32_t version);

	fsdk::IDescriptorExtractorPtr createExtractor();

	fsdk::IDescriptorMatcherPtr createMatcher();

//	second part of estimators
	fsdk::IHeadPoseEstimatorPtr createHeadPoseEstimator();

	fsdk::Ref<fsdk::IBlackWhiteEstimator> createBlackWhiteEstimator();

	fsdk::ILivenessDepthEstimatorPtr createDepthEstimator();

	fsdk::ILivenessIREstimatorPtr createIREstimator();

	fsdk::ISmileEstimatorPtr createSmileEstimator();

	fsdk::ILivenessFlowEstimatorPtr createFaceFlowEstimator();

	fsdk::IEyeEstimatorPtr createEyeEstimator();

	fsdk::IEmotionsEstimatorPtr createEmotionsEstimator();

	fsdk::IGazeEstimatorPtr createGazeEstimator();
	
	fsdk::IIndexBuilderPtr createIndexBuilder();
	
	fsdk::ResultValue<fsdk::FSDKError, fsdk::IDenseIndex*> loadDenseIndex(
	const char* indexPath);
	
	fsdk::ResultValue<fsdk::FSDKError, fsdk::IDynamicIndex*> loadDynamicIndex(
	const char* indexPath);
	
	fsdk::IAGSEstimatorPtr createAGSEstimator();

	void setSettingsProvider(PyISettingsProvider& provider);
	
};
