class ActivationLicenseError(RuntimeError):
    def __init__(self, arg):
        self.strerror = arg
        self.args = {arg}


def make_activation(_face_engine, _license_conf_path="data/license.conf"):
    _license = _face_engine.getLicense()
    return _face_engine.activateLicense(_license, _license_conf_path)