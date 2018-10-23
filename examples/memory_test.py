import sys


# if FaceEngine is not installed pass the path to dir with FaceEngine*.so and add it to system paths
sys.path.append(sys.argv[1])
import FaceEngine as fe
from memory_profiler import profile


class FaceEngineWrap:
    """
    FaceEngine wraper

    Attributes:
        faceEngine (PyIFaceEngine): face engine factory
        detector (IDetectorPtr): detector
        warper (IWarperPtr): warper
        headPoseEstimator (IHeadPoseEstimatorPtr): head pose estimator
        qualityEstimator (IWarperPtr): quality estimator
    """

    def __init__(self, faceEngineData: str, faceEngineConf: str):
        """
        Init  FaceEngineWrap.

        Args:
            faceEngineData: path to the face engine data folder
            faceEngineConf:  path to the face engine config file
        """
        self.faceEngine = fe.createFaceEngine(faceEngineData, faceEngineConf)
        self.detector = self.faceEngine.createDetector(fe.ODT_MTCNN)
        self.warper = self.faceEngine.createWarper()
        self.headPoseEstimator = self.faceEngine.createHeadPoseEstimator()
        self.qualityEstimator = self.faceEngine.createQualityEstimator()


t = FaceEngineWrap("data",
                   "data/faceengine.conf")


@profile
def test():
    with open("/home/mar/projects/FaceEnginePythonBinding/build/testData/warp1.tiff", 'rb') as file:
        data = file.read()
    for i in range(100000):
        image = fe.Image()
        image.loadFromMemory(data, len(data))
        t.detector.detect(image, image.getRect(), 64)


if __name__ == '__main__':
    test()