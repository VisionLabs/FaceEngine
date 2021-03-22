#include <FaceEngineAdapter.hpp>
#include <pybind11/pybind11.h>

namespace py = pybind11;


namespace {
	std::string getErrorMessage(const char* what, const char* instanceName) {
		return
			std::string("\nFailed ") + instanceName + " instance! See the "
				"\"Troubleshooting and diagnostics\" chapter in the documentation "
				"for possible reasons. Reason: " + what;
	}
	
	template <typename R>
	typename R::ValueType processResult(R result, const char* instanceName) {
		if (!result) {
			const std::string errorText = getErrorMessage(result.what(), instanceName);
			throw py::cast_error(errorText.c_str());
		}
		return result.getValue();
	}
}


PyIFaceEngine::PyIFaceEngine(const char* dataPath = nullptr, const char* configPath = nullptr, const char* runtimeConfigPath = nullptr) {
	auto res = fsdk::createFaceEngine(dataPath, configPath, runtimeConfigPath);
	if (res.isError()) {
		throw py::cast_error(getErrorMessage(res.what(), "to create FaceEngine").c_str());
	}
	faceEnginePtr = res.getValue();
}

fsdk::FaceEngineEdition PyIFaceEngine::getFaceEngineEdition() {
	return faceEnginePtr->getFaceEngineEdition();
}

fsdk::IDetectorPtr PyIFaceEngine::createDetector(
	fsdk::ObjectDetectorClassType type/* = fsdk::FACE_DET_DEFAULT*/, 
		fsdk::SensorType mode/* = SensorType::Visible*/) {
	auto res = faceEnginePtr->createDetector(type, mode);
	return processResult(res, "to create detector");
}

fsdk::IAttributeEstimatorPtr PyIFaceEngine::createAttributeEstimator() {
	auto res = faceEnginePtr->createAttributeEstimator();
	return processResult(res, "to create attribute estimator");
}

fsdk::IQualityEstimatorPtr PyIFaceEngine::createQualityEstimator() {

	auto res = faceEnginePtr->createQualityEstimator();
	return processResult(res, "to create attribute estimator");
}

fsdk::IBestShotQualityEstimatorPtr PyIFaceEngine::createBestShotQualityEstimator() {
	auto res = faceEnginePtr->createBestShotQualityEstimator();
	return processResult(res, "to create best shot quality estimator");
}

//	warper
fsdk::IWarperPtr PyIFaceEngine::createWarper() {
	auto res = faceEnginePtr->createWarper();
	return processResult(res, "to create warper");
}

fsdk::IHumanWarperPtr PyIFaceEngine::createHumanWarper() {
	auto res = faceEnginePtr->createHumanWarper();
	return processResult(res, "to create human warper");
}

//	descriptor
fsdk::IDescriptorPtr PyIFaceEngine::createDescriptor(const uint32_t version) {
	auto res = faceEnginePtr->createDescriptor(version);
	return processResult(res, "to create descriptor");
}

fsdk::IDescriptorBatchPtr PyIFaceEngine::createDescriptorBatch(int32_t size, int32_t version) {
	auto res = faceEnginePtr->createDescriptorBatch(size, version);
	return processResult(res, "to create descriptor batch");
	
}

fsdk::IDescriptorExtractorPtr PyIFaceEngine::createExtractor(const uint32_t version) {
	auto res = faceEnginePtr->createExtractor(version);
	return processResult(res, "to create extractor");
}

fsdk::IDescriptorMatcherPtr PyIFaceEngine::createMatcher(const uint32_t version) {
	auto res = faceEnginePtr->createMatcher(version);
	return processResult(res, "to create matcher");
}

//	second part of estimators
fsdk::IHeadPoseEstimatorPtr PyIFaceEngine::createHeadPoseEstimator() {
	auto res = faceEnginePtr->createHeadPoseEstimator();
	return processResult(res, "to create head pose estimator");
}

fsdk::Ref<fsdk::IBlackWhiteEstimator> PyIFaceEngine::createBlackWhiteEstimator() {
	auto res = faceEnginePtr->createBlackWhiteEstimator();
	return processResult(res, "to create black-white estimator");
	
}

fsdk::ILivenessDepthEstimatorPtr PyIFaceEngine::createDepthEstimator() {
	auto res = faceEnginePtr->createDepthEstimator();
	return processResult(res, "to create depth liveness estimator");
}

fsdk::ILivenessIREstimatorPtr PyIFaceEngine::createIREstimator() {
	auto res = faceEnginePtr->createIREstimator();
	return processResult(res, "to create liveness ir estimator");
}

fsdk::ILivenessFlyingFacesEstimatorPtr PyIFaceEngine::createLivenessFlyingFacesEstimator() {
	auto res = faceEnginePtr->createLivenessFlyingFacesEstimator();
	return processResult(res, "to create liveness flying faces estimator");
}

fsdk::ILivenessFPREstimatorPtr PyIFaceEngine::createLivenessFPREstimator() {
	auto res = faceEnginePtr->createLivenessFPREstimator();
	return processResult(res, "to create liveness FPR estimator");
}

fsdk::ILivenessOneShotRGBEstimatorPtr PyIFaceEngine::createLivenessOneShotRGBEstimator() {
	auto res = faceEnginePtr->createLivenessOneShotRGBEstimator();
	return processResult(res, "to create liveness oneshot rgb estimator");
}

fsdk::ILivenessRGBMEstimatorPtr PyIFaceEngine::createLivenessRGBMEstimator() {
	auto res = faceEnginePtr->createLivenessRGBMEstimator();
	return processResult(res, "to create liveness rgbm estimator");
}

fsdk::ILivenessFlowEstimatorPtr PyIFaceEngine::createFaceFlowEstimator() {
	auto res = faceEnginePtr->createFaceFlowEstimator();
	return processResult(res, "to create liveness flow estimator");
}

fsdk::IEyeEstimatorPtr PyIFaceEngine::createEyeEstimator(fsdk::SensorType mode/* = fsdk::SensorType::Visible*/) {
	auto res = faceEnginePtr->createEyeEstimator(mode);
	return processResult(res, "to create eye estimator");
}

fsdk::IEmotionsEstimatorPtr PyIFaceEngine::createEmotionsEstimator() {
	auto res = faceEnginePtr->createEmotionsEstimator();
	return processResult(res, "to create emotions estimator");
}

fsdk::IGazeEstimatorPtr PyIFaceEngine::createGazeEstimator(fsdk::SensorType mode /* = fsdk::SensorType::Visible*/) {
	auto res = faceEnginePtr->createGazeEstimator(mode);
	return processResult(res, "to create gaze estimator");
}

fsdk::IGlassesEstimatorPtr PyIFaceEngine::createGlassesEstimator() {
	auto res = faceEnginePtr->createGlassesEstimator();
	return processResult(res, "to create glasses estimator");
}

fsdk::IIndexBuilderPtr PyIFaceEngine::createIndexBuilder(
	const uint32_t capacity,
	const uint32_t version) {

	auto res = faceEnginePtr->createIndexBuilder(capacity, version);
	return processResult(res, "to create indexBuilder");
}

fsdk::Ref<fsdk::IHumanDetector> PyIFaceEngine::createHumanDetector() {
	auto res = faceEnginePtr->createHumanDetector();
	return processResult(res, "to create human detector");
}

fsdk::Ref<fsdk::IOrientationEstimator> PyIFaceEngine::createOrientationEstimator() {
	auto res = faceEnginePtr->createOrientationEstimator();
	return processResult(res, "to create orientation estimator");
}

fsdk::ResultValue<fsdk::FSDKError, fsdk::IDenseIndexPtr> PyIFaceEngine::loadDenseIndex(
	const char* indexPath) {
	return faceEnginePtr->loadDenseIndex(indexPath);
}

fsdk::ResultValue<fsdk::FSDKError, fsdk::IDynamicIndexPtr> PyIFaceEngine::loadDynamicIndex(
	const char* indexPath) {
	return faceEnginePtr->loadDynamicIndex(indexPath);
}

fsdk::IAGSEstimatorPtr PyIFaceEngine::createAGSEstimator() {
	auto res = faceEnginePtr->createAGSEstimator();
	return processResult(res, "to create ags estimator");
}

fsdk::IPPEEstimatorPtr PyIFaceEngine::createPPEEstimator() {
	auto res = faceEnginePtr->createPPEEstimator();
	return processResult(res, "to create PPE estimator");
}

fsdk::IMouthEstimatorPtr PyIFaceEngine::createMouthEstimator() {
	auto res = faceEnginePtr->createMouthEstimator();
	return processResult(res, "to create mouth estimator");
}

fsdk::IMedicalMaskEstimatorPtr PyIFaceEngine::createMedicalMaskEstimator() {
	auto res = faceEnginePtr->createMedicalMaskEstimator();
	return processResult(res, "to create medical mask estimator");
	
}

fsdk::IOverlapEstimatorPtr PyIFaceEngine::createOverlapEstimator() {
	auto res = faceEnginePtr->createOverlapEstimator();
	return processResult(res, "to create overlap estimator");
}

fsdk::ICredibilityCheckEstimatorPtr PyIFaceEngine::createCredibilityCheckEstimator() {
	auto res = faceEnginePtr->createCredibilityCheckEstimator();
	return processResult(res, "to create credibility estimator");
}

void PyIFaceEngine::setSettingsProvider(PyISettingsProvider& provider) {
	faceEnginePtr->setSettingsProvider(provider.settingsProviderPtr);
}

PyISettingsProvider PyIFaceEngine::getSettingsProvider() {
	fsdk::ISettingsProviderPtr config{ faceEnginePtr->getSettingsProvider() };
	if (!config) {
		throw py::cast_error(getErrorMessage("", "to get settings provider").c_str());
	}
	
	return PyISettingsProvider(config);
}

PyISettingsProvider PyIFaceEngine::getRuntimeSettingsProvider() {
	fsdk::ISettingsProviderPtr config{ faceEnginePtr->getRuntimeSettingsProvider() };
	if (!config)
		throw py::cast_error(getErrorMessage("", "to get runtime provider").c_str());
	
	return PyISettingsProvider(config);
}

void PyIFaceEngine::setRuntimeSettingsProvider(PyISettingsProvider& provider) {
	faceEnginePtr->setRuntimeSettingsProvider(provider.settingsProviderPtr);
}

fsdk::ILicensePtr PyIFaceEngine::getLicense() {
	return fsdk::ILicensePtr{faceEnginePtr->getLicense()};
}

fsdk::Result<fsdk::FSDKError> PyIFaceEngine::activateLicense(const fsdk::ILicensePtr& license, const char* configPath) {
	return fsdk::activateLicense(license.get(), configPath);
}
