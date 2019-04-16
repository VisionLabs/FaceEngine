import sys

# pass path dir with FaceEngine*.so and add it to system directory
sys.path.append(sys.argv[1])
import FaceEngine as fe

def help():
    print("python example_license.py <path to FaceEngine*.so> <path to license.conf>")

def get_info():
    print(fe.getVersionHash())
    print(fe.getVersionString())
    print(fe.getBuildInfo())

def checkLicense(license):

    if license.isActivated():
        print("License if activated!")
    else:
        print("License if NOT activated!")

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

        checkLicense(license)

        faceEngine.activateLicense(license, licenseConfPath)

        checkLicense(license)

    except Exception as ex:
        print(type(ex).__name__, ex)
        exit(-1)
    finally:
        # do something here
        pass
