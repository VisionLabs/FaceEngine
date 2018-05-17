# FaceEngine python library


## Prerequisites

**On Unix (Linux, OS X)**

* A compiler with C++11 support
* CMake >= 2.8.12

**On Windows**

* Visual Studio 2015 (required for all Python versions, see notes below)
* CMake >= 3.1


## Installation

1. Download FSDK to <FSDK_path>
If you're testing on research servers, you could try
FSDK_path = /media/grinchuk/packages/FSDK/luna-sdk_linux_rel_77e6a500693783f563274e12a3341d42d29cbb0e

2. 

```bash
git clone --recursive http://git.visionlabs.ru/m.safina/FaceEnginePythonBinding.git
cd FaceEnginePythonBinding
git submodule init
git submodule update
```
3. For just python library:
Change FSDK_ROOT in setup.py to <FSDK_path>:

```bash
python setup.py install 
```

If you want to build from sources:
```bash
mkdir build
cd build
cmake .. -DFSDK_ROOT=<FSDK_path> -DCMAKE_INSTALL_PREFIX=./install
cmake --build . --config Release
```

## Usage

```bash
import FaceEngine as f
faceEnginePtr = f.createFaceEngine("data", "data/faceengine.conf")
```
Other usage examples could be found in FaceEnginePythonBinding/tests/


## Special notes for Windows

**Compiler requirements**

Pybind11 requires a C++11 compliant compiler, i.e Visual Studio 2015 on Windows.
This applies to all Python versions, including 2.7. Unlike regular C extension
modules, it's perfectly fine to compile a pybind11 module with a VS version newer
than the target Python's VS version. See the [FAQ] for more details.

**Runtime requirements**

The Visual C++ 2015 redistributable packages are a runtime requirement for this
project. It can be found [here][vs2015_runtime]. If you use the Anaconda Python
distribution, you can add `vs2015_runtime` as a platform-dependent runtime
requirement for you package: see the `conda.recipe/meta.yaml` file in this example.

## License

Pybind11 is provided under a BSD-style license that can be found in the LICENSE
file. By using, distributing, or contributing to this project, you agree to the
terms and conditions of this license.


## Test call

From root directory:

```bash
python3 tests/test_estimators.py --bind-path build
```

```python3
import FaceEngine as f
faceEnginePtr = f.createFaceEngine("data", "data/faceengine.conf")
```


[FAQ]: http://pybind11.rtfd.io/en/latest/faq.html#working-with-ancient-visual-studio-2009-builds-on-windows
[vs2015_runtime]: https://www.microsoft.com/en-us/download/details.aspx?id=48145
