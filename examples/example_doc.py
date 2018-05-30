import sys

def help():
    print("python example.py <path to FaceEngine*.so> ")

if len(sys.argv) != 2:
    help()
    exit(1)

# pass path dir with FaceEngine*.so and add it to system directory
sys.path.append(sys.argv[1])
import FaceEngine as fe

print(fe.__doc__)
print(fe.createFaceEngine.__doc__)
print(fe.createSettingsProvider.__doc__)
print(fe.PyIFaceEngine.__doc__)
print(fe.PyIFaceEngine.createAttributeEstimator.__doc__)
print(fe.PyIFaceEngine.createQualityEstimator.__doc__)
print(fe.PyIFaceEngine.createEthnicityEstimator.__doc__)

print(fe.PyIFaceEngine.createHeadPoseEstimator.__doc__)
print(fe.PyIFaceEngine.createBlackWhiteEstimator.__doc__)
print(fe.PyIFaceEngine.createDepthEstimator.__doc__)
print(fe.PyIFaceEngine.createIREstimator.__doc__)
print(fe.PyIFaceEngine.createSmileEstimator.__doc__)
print(fe.PyIFaceEngine.createFaceFlowEstimator.__doc__)
print(fe.PyIFaceEngine.createEyeEstimator.__doc__)
print(fe.PyIFaceEngine.createEmotionsEstimator.__doc__)
print(fe.PyIFaceEngine.createGazeEstimator.__doc__)

print(fe.PyIFaceEngine.createDetector.__doc__)
print(fe.PyIFaceEngine.createWarper.__doc__)
print(fe.PyIFaceEngine.createDescriptor.__doc__)
print(fe.PyIFaceEngine.createDescriptorBatch.__doc__)
print(fe.PyIFaceEngine.createExtractor.__doc__)
print(fe.PyIFaceEngine.createMatcher.__doc__)
print(fe.PyIFaceEngine.createLSHTable.__doc__)
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

print(fe.IAttributeEstimatorPtr.__doc__)
print(fe.IAttributeEstimatorPtr.estimate.__doc__)

print(fe.IEthnicityEstimatorPtr.__doc__)
print(fe.IEthnicityEstimatorPtr.estimate.__doc__)

print(fe.IDetectorPtr.__doc__)
print(fe.IDetectorPtr.detect.__doc__)

print(fe.IWarperPtr.__doc__)
print(fe.IWarperPtr.warp.__doc__)
print(fe.IWarperPtr.createTransformation.__doc__)