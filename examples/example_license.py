import sys

# pass path dir with FaceEngine*.so and add it to system directory
sys.path.append(sys.argv[1])
import FaceEngine as fe

def help():
    print("python example_license.py <path to FaceEngine*.so> <path to the folder with license.conf>")

def get_info():
    print(fe.getVersionHash())
    print(fe.getVersionString())
    print(fe.getBuildInfo())

def checkLicense(license):

    if license.isActivated():
        print("License is activated!")
    else:
        print("License is NOT activated!")

    freatures = [
        fe.LicenseFeature.Detection,
        fe.LicenseFeature.BestShot,
        fe.LicenseFeature.Attributes,
        fe.LicenseFeature.Emotions,
        fe.LicenseFeature.FaceFeatures,
        fe.LicenseFeature.Liveness,
        fe.LicenseFeature.Descriptor,
        fe.LicenseFeature.DescriptorIndex,
        fe.LicenseFeature.LivenessEngine,
        fe.LicenseFeature.TrackEngine,
        fe.LicenseFeature.HumanDetection
    ]

    for feature in freatures:
        if license.checkFeatureId(feature):
            print("{0} is available".format(feature))
        else:
            print("{0} is NOT available".format(feature))

    return license.isActivated()

if __name__ == "__main__":
    # correct path or put directory "data" with example.py
    faceEngine = fe.createFaceEngine(
        "data",
        "data/faceengine.conf")
    get_info()

    if len(sys.argv) != 3:
        help()
        exit(1)

    licenseConfPath = sys.argv[2]

    try:
        license = faceEngine.getLicense()

        if not checkLicense(license):
            # check facility for not activated license
            detector = faceEngine.createDetector(fe.FACE_DET_V1)
            if detector is None:
                print("Detector creation failed! License is not activated!")


        if faceEngine.activateLicense(license, licenseConfPath):
            print("License was sucessfully activated!")
        else:
            print("License activation failed! See debug logs for details.")

        if checkLicense(license):
            # check facility for activated license
            detector = faceEngine.createDetector(fe.FACE_DET_V1)
            if detector is not None:
                print("Detector creation success! License is activated!")

    except Exception as ex:
        print(type(ex).__name__, ex)
        exit(-1)
    finally:
        # do something here
        pass
