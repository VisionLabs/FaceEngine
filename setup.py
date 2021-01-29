import os
import re
import sys
import platform
import subprocess
import distutils.command.install as orig

from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from setuptools.command.install import install
from distutils.version import LooseVersion

build_te_bindings = False
build_le_bindings = False

class InstallCommand(install):
    user_options = install.user_options + [
        ('build-te-bindings', None, 'Build TrackEngine bindings.'),
        ('build-le-bindings', None, 'Build LivenessEngine bindings.')
    ]

    def __init__(self, dist):
        super().__init__(dist)
        self.build_le_bindings = False
        self.build_te_bindings = False

    def run(self):
        global build_te_bindings
        global build_le_bindings
        build_te_bindings = self.build_te_bindings
        build_le_bindings = self.build_le_bindings
        orig.install.run(self)


class CMakeExtension(Extension):
    def __init__(self, name, sourcedir=''):
        Extension.__init__(self, name, sources=[])
        self.sourcedir = os.path.abspath(sourcedir)


class CMakeBuild(build_ext):
    def run(self):
        try:
            out = subprocess.check_output(['cmake', '--version'])
        except OSError:
            raise RuntimeError("CMake must be installed to build the following extensions: " +
                               ", ".join(e.name for e in self.extensions))

        if platform.system() == "Windows":
            cmake_version = LooseVersion(re.search(r'version\s*([\d.]+)', out.decode()).group(1))
            if cmake_version < '3.1.0':
                raise RuntimeError("CMake >= 3.1.0 is required on Windows")

        for ext in self.extensions:
            self.build_extension(ext)

    def build_extension(self, ext):
        extdir = os.path.abspath(os.path.dirname(self.get_ext_fullpath(ext.name)))
        path_to_fsdk = os.environ.get('FSDK_ROOT')
        if path_to_fsdk is not None:
            print("FSDK_ROOT is set as environment variable: {0}".format(path_to_fsdk))
        else:
            path_to_fsdk = os.path.split(os.path.realpath(__file__))[0] + '/..'
            print("FSDK_ROOT is UNSET, default path to LUNA SDK: {0}, you could try to change it".format(path_to_fsdk))
        cmake_args = ['-DFSDK_ROOT=' + path_to_fsdk, '-DCMAKE_LIBRARY_OUTPUT_DIRECTORY=' + extdir,
                      '-DPYTHON_EXECUTABLE=' + sys.executable]

        if build_te_bindings:
            cmake_args.append('-DBUILD_TE_BINDINGS=ON')

        if build_le_bindings:
            cmake_args.append('-DBUILD_LE_BINDINGS=ON')

        cfg = 'Debug' if self.debug else 'Release'
        build_args = ['--config', cfg]

        if platform.system() == "Windows":
            cmake_args += ['-DCMAKE_LIBRARY_OUTPUT_DIRECTORY_{}={}'.format(cfg.upper(), extdir)]
            if sys.maxsize > 2 ** 32:
                cmake_args += ['-A', 'x64']
            build_args += ['--', '/m']
        else:
            cmake_args += ['-DCMAKE_BUILD_TYPE=' + cfg]
            build_args += ['--', '-j2']

        env = os.environ.copy()
        env['CXXFLAGS'] = '{} -DVERSION_INFO=\\"{}\\"'.format(env.get('CXXFLAGS', ''),
                                                              self.distribution.get_version())
        if not os.path.exists(self.build_temp):
            os.makedirs(self.build_temp)
        subprocess.check_call(['cmake', ext.sourcedir] + cmake_args, cwd=self.build_temp, env=env)
        subprocess.check_call(['cmake', '--build', '.'] + build_args, cwd=self.build_temp)
        print(cmake_args)


if __name__ == '__main__':
    setup(
        name='FaceEngine',
        version='5.0.0.1',
        author='VisionLabs',
        author_email='info@visionlabs.ru',
        description='Python bindings of FaceEngine using pybind11 and CMake',
        long_description='',
        ext_modules=[CMakeExtension('FaceEngine')],
        cmdclass=dict(install=InstallCommand, build_ext=CMakeBuild),
        zip_safe=False,
    )
