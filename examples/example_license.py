import sys


def help():
    print("python example_license.py <path to dir with FaceEngine*.so> <path to the folder with license.conf>")
    exit(1)


try:
    # pass path dir with FaceEngine*.so and add it to system directory
    sys.path.append(sys.argv[1])
except:
    help()


import FaceEngine as fe


def get_info():
    print(fe.getVersionHash())
    print(fe.getVersionString())
    print(fe.getBuildInfo())


def check_license(license):

    errCode, val = license.isActivated()
    if val:
        print("License is activated!")
    else:
        print("License is NOT activated!")
        print("check_license: failed, reason:", errCode.what)
        exit(-1)

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
        errCode, val = license.checkFeatureId(feature)
        if errCode.isError:
            print("check_license: failed, reason:", errCode.what)
            exit(-1)

        if val:
            print("{0} is available".format(feature))
        else:
            print("{0} is NOT available".format(feature))

    return license.isActivated()


def make_activation(_face_engine, _license_conf_path="data/license.conf"):
    _license = _face_engine.getLicense()
    return _face_engine.activateLicense(_license, _license_conf_path)


if __name__ == "__main__":
    # correct path or put directory "data" with example.py
    faceEngine = fe.createFaceEngine("data")
    get_info()

    if len(sys.argv) != 3:
        help()

    licenseConfPath = sys.argv[2]

    try:
        license = faceEngine.getLicense()

        if not check_license(license):
            # check facility for not activated license
            try:
                print("Trying to create detector before activation.")
                detector = faceEngine.createDetector(fe.FACE_DET_V1)
                print("Strange, but detector was created sucessfully!")
            except Exception as ex:
                print("Detector creation failed. Exception: {0} {1}".format(type(ex).__name__, ex))

        if faceEngine.activateLicense(license, licenseConfPath):
            print("License was sucessfully activated!")
        else:
            print("License activation failed! See debug logs for details.")

        if check_license(license):
            # check facility for activated license
            try:
                print("Trying to create detector after activation.")
                detector = faceEngine.createDetector(fe.FACE_DET_V1)
                print("Detector creation success! License is activated!")
            except Exception as ex:
                print("Strange, but detector creation failed. Exception: {0} {1}".format(type(ex).__name__, ex))

    except Exception as ex:
        print(type(ex).__name__, ex)
        exit(-1)
    finally:
        # do something here
        pass
