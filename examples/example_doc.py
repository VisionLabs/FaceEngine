import sys

def help():
    print("python example.py <path to dir with FaceEngine*.so> ")

if len(sys.argv) != 2:
    help()
    exit(1)

# pass path dir with FaceEngine*.so and add it to system directory
sys.path.append(sys.argv[1])
import FaceEngine as fe

print(fe.__doc__)
print(fe.FaceEngineEdition.__doc__)
print(fe.createFaceEngine.__doc__)
print(fe.createSettingsProvider.__doc__)
print(fe.PyIFaceEngine.__doc__)
print(fe.PyIFaceEngine.getFaceEngineEdition.__doc__)
print(fe.PyIFaceEngine.createAttributeEstimator.__doc__)
print(fe.PyIFaceEngine.createQualityEstimator.__doc__)
print(fe.PyIFaceEngine.createEthnicityEstimator.__doc__)
print(fe.PyIFaceEngine.createLivenessFlyingFacesEstimator.__doc__)

print(fe.PyIFaceEngine.createHeadPoseEstimator.__doc__)
print(fe.PyIFaceEngine.createBlackWhiteEstimator.__doc__)
print(fe.PyIFaceEngine.createDepthEstimator.__doc__)
print(fe.PyIFaceEngine.createIREstimator.__doc__)
print(fe.PyIFaceEngine.createSmileEstimator.__doc__)
print(fe.PyIFaceEngine.createFaceFlowEstimator.__doc__)
print(fe.PyIFaceEngine.createEyeEstimator.__doc__)
print(fe.PyIFaceEngine.createEmotionsEstimator.__doc__)
print(fe.PyIFaceEngine.createGazeEstimator.__doc__)
print(fe.PyIFaceEngine.createGlassesEstimator.__doc__)
print(fe.PyIFaceEngine.createMouthEstimator.__doc__)
print(fe.PyIFaceEngine.createOverlapEstimator.__doc__)

print(fe.PyIFaceEngine.createDetector.__doc__)
print(fe.PyIFaceEngine.createWarper.__doc__)
print(fe.PyIFaceEngine.createDescriptor.__doc__)
print(fe.PyIFaceEngine.createDescriptorBatch.__doc__)
print(fe.PyIFaceEngine.createExtractor.__doc__)
print(fe.PyIFaceEngine.createMatcher.__doc__)

print(fe.PyIFaceEngine.createIndexBuilder.__doc__)
print(fe.PyIFaceEngine.loadDenseIndex.__doc__)
print(fe.PyIFaceEngine.loadDynamicIndex.__doc__)

print(fe.PyIFaceEngine.setSettingsProvider.__doc__)

print(fe.SettingsProviderValue.__doc__)
print(fe.SettingsProviderValue.__init__.__doc__)

print(fe.PyISettingsProvider.getDefaultPath.__doc__)
print(fe.PyISettingsProvider.load.__doc__)
print(fe.PyISettingsProvider.save.__doc__)
print(fe.PyISettingsProvider.clear.__doc__)
print(fe.PyISettingsProvider.isEmpty.__doc__)
print(fe.PyISettingsProvider.setValue.__doc__)
print(fe.PyISettingsProvider.getValue.__doc__)

print(fe.IQualityEstimatorPtr.__doc__)
print(fe.IQualityEstimatorPtr.estimate.__doc__)

print(fe.IEthnicityEstimatorPtr.__doc__)
print(fe.IEthnicityEstimatorPtr.estimate.__doc__)

print(fe.ILivenessFlyingFacesEstimatorPtr.__doc__)
print(fe.ILivenessFlyingFacesEstimatorPtr.estimate.__doc__)

print(fe.IOverlapEstimatorPtr.__doc__)
print(fe.IOverlapEstimatorPtr.estimate.__doc__)

print(fe.IAttributeEstimatorPtr.__doc__)
print(fe.IAttributeEstimatorPtr.estimate.__doc__)

print(fe.IGlassesEstimatorPtr.__doc__)
print(fe.IGlassesEstimatorPtr.estimate.__doc__)

print(fe.IMouthEstimatorPtr.__doc__)
print(fe.IMouthEstimatorPtr.estimate.__doc__)

print(fe.IDetectorPtr.__doc__)
print(fe.IDetectorPtr.detect.__doc__)
print(fe.IDetectorPtr.detectOne.__doc__)
print(fe.IDetectorPtr.setDetectionComparer.__doc__)

print(fe.IDetectorPtr.redetectOne.__doc__)
print(fe.IDetectorPtr.redetect.__doc__)

print(fe.Face.__doc__)
print(fe.Face.isValid.__doc__)

print(fe.IHumanDetectorPtr.__doc__)
print(fe.IHumanDetectorPtr.detect.__doc__)

print(fe.IWarperPtr.__doc__)
print(fe.IWarperPtr.warp.__doc__)
print(fe.IWarperPtr.createTransformation.__doc__)

print(fe.IWarperPtr.__doc__)
print(fe.IWarperPtr.warp.__doc__)
print(fe.IWarperPtr.createTransformation.__doc__)

print(fe.IWarperPtr.__doc__)
print(fe.IWarperPtr.warp.__doc__)
print(fe.IWarperPtr.createTransformation.__doc__)

print(fe.IDescriptorPtr.__doc__)
print(fe.IDescriptorPtr.getModelVersion.__doc__)
print(fe.IDescriptorPtr.getDescriptorLength.__doc__)
print(fe.IDescriptorPtr.getDescriptor.__doc__)
print(fe.IDescriptorPtr.getData.__doc__)
print(fe.IDescriptorPtr.load.__doc__)
print(fe.IDescriptorPtr.save.__doc__)

print(fe.IDescriptorBatchPtr.__doc__)
print(fe.IDescriptorBatchPtr.add.__doc__)
print(fe.IDescriptorBatchPtr.removeFast.__doc__)
print(fe.IDescriptorBatchPtr.removeSlow.__doc__)
print(fe.IDescriptorBatchPtr.getMaxCount.__doc__)
print(fe.IDescriptorBatchPtr.getCount.__doc__)
print(fe.IDescriptorBatchPtr.getModelVersion.__doc__)
print(fe.IDescriptorBatchPtr.getDescriptorSize.__doc__)
print(fe.IDescriptorBatchPtr.getDescriptorSlow.__doc__)
print(fe.IDescriptorBatchPtr.getDescriptorFast.__doc__)
print(fe.IDescriptorBatchPtr.load.__doc__)
print(fe.IDescriptorBatchPtr.save.__doc__)

print(fe.DescriptorBatchError.__doc__)

print(fe.IDescriptorExtractorPtr.__doc__)
print(fe.IDescriptorExtractorPtr.extract.__doc__)
print(fe.IDescriptorExtractorPtr.extractFromWarpedImage.__doc__)
print(fe.IDescriptorExtractorPtr.extractFromWarpedImageBatch.__doc__)


print(fe.IDescriptorMatcherPtr.__doc__)
print(fe.IDescriptorMatcherPtr.match.__doc__)


print(fe.IHeadPoseEstimatorPtr.__doc__)
print(fe.IHeadPoseEstimatorPtr.estimate.__doc__)

print(fe.IBlackWhiteEstimatorPtr.__doc__)
print(fe.IBlackWhiteEstimatorPtr.estimate.__doc__)

print(fe.ILivenessDepthEstimatorPtr.__doc__)
print(fe.ILivenessDepthEstimatorPtr.estimate.__doc__)
print(fe.ILivenessDepthEstimatorPtr.setRange.__doc__)

print(fe.ILivenessIREstimatorPtr.__doc__)
print(fe.ILivenessIREstimatorPtr.estimate.__doc__)

print(fe.ISmileEstimatorPtr.__doc__)
print(fe.ISmileEstimatorPtr.estimate.__doc__)

print(fe.ILivenessFlowEstimatorPtr.__doc__)
print(fe.ILivenessFlowEstimatorPtr.estimate.__doc__)

print(fe.IEyeEstimatorPtr.__doc__)
print(fe.IEyeEstimatorPtr.estimate.__doc__)

print(fe.EyesRects.__doc__)
print(fe.EyesRects.leftEyeRect.__doc__)
print(fe.EyesRects.leftEyeRect.__doc__)
print(fe.EyeCropper.__doc__)
print(fe.EyeCropper.__init__.__doc__)
print(fe.EyeCropper.cropByLandmarks5.__doc__)
print(fe.EyeCropper.cropByLandmarks68.__doc__)

print(fe.IEmotionsEstimatorPtr.__doc__)
print(fe.IEmotionsEstimatorPtr.estimate.__doc__)

print(fe.IGazeEstimatorPtr.__doc__)
print(fe.IGazeEstimatorPtr.estimate.__doc__)

print(fe.IAGSEstimatorPtr.__doc__)
print(fe.IAGSEstimatorPtr.estimate.__doc__)

print(fe.MatchingResult.__doc__)

print(fe.Landmarks5.__doc__)
print(fe.Landmarks5.__len__.__doc__)
print(fe.Landmarks5.__getitem__.__doc__)
print(fe.Landmarks5.__setitem__.__doc__)
print(fe.Landmarks68.__doc__)
print(fe.Landmarks68.__len__.__doc__)
print(fe.Landmarks68.__getitem__.__doc__)
print(fe.Landmarks68.__setitem__.__doc__)
print(fe.IrisLandmarks.__doc__)
print(fe.IrisLandmarks.__len__.__doc__)
print(fe.IrisLandmarks.__getitem__.__doc__)
print(fe.IrisLandmarks.__setitem__.__doc__)
print(fe.EyelidLandmarks.__doc__)
print(fe.EyelidLandmarks.__len__.__doc__)
print(fe.EyelidLandmarks.__getitem__.__doc__)
print(fe.EyelidLandmarks.__setitem__.__doc__)

print(fe.Vector2f.__doc__)
print(fe.Vector2f.__init__.__doc__)
print(fe.Vector2f.__repr__.__doc__)

print(fe.Vector2i.__doc__)
print(fe.Vector2i.__init__.__doc__)
print(fe.Vector2i.__repr__.__doc__)

print(fe.FSDKErrorResult.__doc__)
print(fe.DescriptorBatchResult.__doc__)
print(fe.ImageErrorResult.__doc__)
print(fe.SettingsProviderErrorResult.__doc__)
print(fe.FSDKErrorValueInt.__doc__)
print(fe.FSDKErrorValueFloat.__doc__)
print(fe.FSDKErrorValueMatching.__doc__)

print(fe.Quality.__doc__)
print(fe.Quality.__init__.__doc__)
print(fe.Quality.getQuality.__doc__)

print(fe.EthnicityEstimation.__doc__)
print(fe.EthnicityEstimation.__init__.__doc__)
print(fe.EthnicityEstimation.__repr__.__doc__)
print(fe.EthnicityEstimation.getEthnicityScore.__doc__)
print(fe.EthnicityEstimation.getPredominantEthnicity.__doc__)

print(fe.HeadPoseEstimation.__doc__)
print(fe.HeadPoseEstimation.__init__.__doc__)
print(fe.HeadPoseEstimation.__repr__.__doc__)
print(fe.HeadPoseEstimation.getFrontalFaceType.__doc__)

print(fe.SmileEstimation.__doc__)
print(fe.SmileEstimation.__init__.__doc__)
print(fe.SmileEstimation.__repr__.__doc__)

print(fe.EyesEstimation.__doc__)
print(fe.EyesEstimation.__init__.__doc__)
print(fe.EyesEstimation.__repr__.__doc__)
print(fe.State.__doc__)
print(fe.EyeAttributes.__doc__)

print(fe.EmotionsEstimation.__doc__)
print(fe.EmotionsEstimation.__init__.__doc__)
print(fe.EmotionsEstimation.__repr__.__doc__)
print(fe.EmotionsEstimation.getPredominantEmotion.__doc__)
print(fe.EmotionsEstimation.getEmotionScore.__doc__)

print(fe.Emotions.__doc__)

print(fe.GazeEstimation.__doc__)
print(fe.GazeEstimation.__init__.__doc__)
print(fe.GazeEstimation.__repr__.__doc__)

print(fe.LivenessFlyingFacesEstimation.__doc__)
print(fe.LivenessFlyingFacesEstimation.__init__.__doc__)
print(fe.LivenessFlyingFacesEstimation.__repr__.__doc__)

print(fe.Ethnicity.__doc__)
print(fe.GlassesEstimation.__doc__)
print(fe.OverlapEstimation.__doc__)
print(fe.MouthEstimation.__doc__)

print(fe.Transformation.__doc__)

print(fe.Detection.__doc__)
print(fe.Detection.isValid.__doc__)
print(fe.Detection.__repr__.__doc__)

print(fe.DetectionFloat.__doc__)
print(fe.DetectionFloat.isValid.__doc__)
print(fe.DetectionFloat.__repr__.__doc__)

print(fe.Human.__doc__)
print(fe.Human.isValid.__doc__)
print(fe.Human.__repr__.__doc__)

print(fe.FormatType.__doc__)

print(fe.Image.__doc__)
print(fe.Image.__init__.__doc__)
print(fe.Image.getFormat.__doc__)

print(fe.Image.getData.__doc__)
print(fe.Image.getChannelCount.__doc__)
print(fe.Image.getChannelStep.__doc__)
print(fe.Image.getBitDepth.__doc__)
print(fe.Image.getByteDepth.__doc__)
print(fe.Image.computePitch.__doc__)
print(fe.Image.isPadded.__doc__)
print(fe.Image.isBGR.__doc__)
print(fe.Image.isValidFormat.__doc__)
print(fe.Image.setData.__doc__)
print(fe.Image.getWidth.__doc__)
print(fe.Image.getHeight.__doc__)
print(fe.Image.isValid.__doc__)
print(fe.Image.save.__doc__)
print(fe.Image.load.__doc__)
print(fe.Image.loadFromMemory.__doc__)
print(fe.Image.getRect.__doc__)
print(fe.ImageType.__doc__)

print(fe.ImageError.__doc__)
print(fe.ImageCompression.__doc__)

print(fe.SettingsProviderError.__doc__)

print(fe.Rect.__doc__)
print(fe.RectFloat.__doc__)

print(fe.ObjectDetectorClassType.__doc__)

print(fe.DetectionComparerType.__doc__)

print(fe.DetectionType.__doc__)

print(fe.FSDKError.__doc__)

print(fe.FrontalFaceType.__doc__)

print(fe.DepthRange.__doc__)
print(fe.DepthRange.__repr__.__doc__)
print(fe.DepthEstimation.__doc__)
print(fe.DepthEstimation.__repr__.__doc__)
print(fe.loadImage.__doc__)

print(fe.IIndexPtr.__doc__)
print(fe.IIndexPtr.search.__doc__)

print(fe.IDenseIndexPtr.__doc__)
print(fe.IDenseIndexPtr.search.__doc__)
print(fe.IDenseIndexPtr.size.__doc__)
print(fe.IDenseIndexPtr.descriptorByIndex.__doc__)

print(fe.IDynamicIndexPtr.__doc__)
print(fe.IDynamicIndexPtr.saveToDenseIndex.__doc__)
print(fe.IDynamicIndexPtr.saveToDynamicIndex.__doc__)
print(fe.IDynamicIndexPtr.countOfIndexedDescriptors.__doc__)
print(fe.IDynamicIndexPtr.search.__doc__)
print(fe.IDynamicIndexPtr.size.__doc__)
print(fe.IDynamicIndexPtr.descriptorByIndex.__doc__)
print(fe.IDynamicIndexPtr.search.__doc__)
print(fe.IDynamicIndexPtr.appendDescriptor.__doc__)
print(fe.IDynamicIndexPtr.appendBatch.__doc__)
print(fe.IDynamicIndexPtr.removeDescriptor.__doc__)

print(fe.IIndexBuilderPtr.__doc__)
print(fe.IIndexBuilderPtr.buildIndex.__doc__)
print(fe.IIndexBuilderPtr.appendDescriptor.__doc__)
print(fe.IIndexBuilderPtr.appendBatch.__doc__)
print(fe.IIndexBuilderPtr.appendDescriptor.__doc__)
print(fe.IIndexBuilderPtr.removeDescriptor.__doc__)
print(fe.IIndexBuilderPtr.descriptorByIndex.__doc__)
