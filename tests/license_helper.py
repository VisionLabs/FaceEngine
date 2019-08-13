class ActivationLicenseError(RuntimeError):
    def __init__(self, arg):
        self.strerror = arg
        self.args = {arg}


def make_activation(_faceEngine, _licenseConfPath="./data"):
    license = _faceEngine.getLicense()
    return _faceEngine.activateLicense(license, _licenseConfPath)