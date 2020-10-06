# LUNA SDK python bindings

Welcome to the **Beta** of PythonBindings for VisionLabs LUNA SDK.
Thank you all for participating of Beta Test.
With all of your feedback and suggestions you can help us to create very useful feature - calling methods of LUNA SDK through python.

In pythonBindings there are some supplementary materials you may find useful.
Look into *cmake/* folder for a CMake find script for the SDK. You are not forced to use
CMake but we advise for it.

Currently we support Linux, Jetson and Windows.

Note, that the SDK is officially supported on RedHat
Linux families (RHEL, CentOS, Fedora). 

**Python version**
PythonBindings were tested with python 3.4, 3.5 and python 3.6.
Other versions may work as well(2.7 and higher), **x64**. 

Only basic methods and classes of LUNA SDK were binded.

**On Unix (Linux, Ubuntu 18, Jetson)**
* GCC 4.8.5 or higher
* A compiler with C++11 support
* CMake >= 3.6
* numpy for python
* associated python3.x-dev package.

**On Windows**
* We support only windows x64
* A compiler with C++11 support
* CMake >= 3.6
* Bindings were tested only with "Visual Studio 14 2015".
* numpy for python

## Building on Linux, Jetson
From source files of pythonBindings generated a shared library `FaceEngine*.so`.
This library can be installed in system or used without installing.
 
You should do such steps:

1. Download LUNA SDK.

2. Choose directory with pythonBindings within LUNA SDK.

```bash
cd pythonBindings
```
3. **For library building and installing, only for Linux**:

```bash
$ python3 setup.py install 
```
Library `FaceEngine*.so` will be generated and installed in system.  

to uninstall it:

```bash
$ pip uninstall FaceEngine 
```
Possible you'll need **sudo rights** or use python3 and pip3.

4. **For library building without installing**:

If you want to **build from source files by CMake without installing**:

```bash
$ mkdir build
$ cd build
$ cmake .. -DFSDK_ROOT=<path_to_LUNA_SDK>
$ cmake --build . --config Release
```

If you want to point not default version of python or custom path to LUNA SDK you can write 

**Example building command (from FSDK_ROOT/pythonBindings/build):**
for `python 3.6` for example:

```bash
$ cmake .. -DFSDK_ROOT=<FSDK_path> -DCMAKE_INSTALL_PREFIX=./install -DPYTHON_EXECUTABLE:FILEPATH=/usr/bin/python3.6
-DPYTHON_LIBRARY=$(python-config --prefix)/lib/libpython3.6.lib -DPYTHON_INCLUDE_DIR=$(python-config --prefix)/include/python3.6
```
**Example of building command (from FSDK_ROOT/pythonBindings/build):**
for python 2.7 for example:

```bash
$ cmake .. -DFSDK_ROOT=<FSDK_path> -DCMAKE_INSTALL_PREFIX=./install -DPYTHON_EXECUTABLE:FILEPATH=/usr/bin/python
-DPYTHON_LIBRARY=$(python-config --prefix)/lib/libpython2.7.lib -DPYTHON_INCLUDE_DIR=$(python-config --prefix)/include/python2.7
```

## Building on Windows

1. Download LUNA SDK.

2. Choose directory with pythonBindings within LUNA SDK.

```cmd
cd pythonBindings
```
Possible you'll need **system administrator rights**.

3. **For library building without installing in system**:

Default python.
```cmd
$ mkdir build
$ cd build
$ cmake -G "Visual Studio 14 2015 Win64" ../ -DFSDK_ROOT=../../ -DCMAKE_BUILD_TYPE=Release 
$ cmake --build . --config Release
$ copy ..\..\bin\vs2015\x64\*.dll release
```
Note: 
In a case of python3.8 and Windows10 was detected the problem with relative paths. Please pass absolute path to directory with dll, for example `-DFSDK_ROOT=<absolute_path>`.

If you want to point version of python or custom path to LUNA SDK you can write

```cmd
$ mkdir build
$ cd build
$ cmake -G "Visual Studio 14 2015 Win64" ../ -DFSDK_ROOT=../../ -DCMAKE_BUILD_TYPE=Release -DPYTHON_EXECUTABLE:FILEPATH=<path_to_python.exe> -DPYTHON_LIBRARY=<path_to_file_python.lib>
$ cmake --build . --config Release
$ copy ..\..\bin\vs2015\x64\*.dll release
```

**Some advises for windows:**
* We support only Windows x64.
* In a case of python3.8 and Windows10 was detected the problem with relative paths. Please pass absolute path to directory with dll, for example `-DFSDK_ROOT=<absolute_path>`.
* Install Visual Studio with tools for C++ building
* You can use developer terminal built-in in Visual Studio.
* Please use `python x64`
* Please do not forget to copy all `*.dll` from `luna_sdk_path\bin` to `pythonBindings\build\release` or set your own enviroment paths.
* Usual path for built FaceEngine.lib is `pythonBindings\build\release`, but it can be different, please verify your VisualStudio settings.
* Please verify slash `/` or backslash `\` is supported in your windows terminal.


## Problem with shared libraries search. 

```Windows cmd
Import error: DLL load failed while importing FaceEngine
```
or 

```Linux bash
ImportError: libFaceEngineSDK.so.3: cannot open shared object file: No such file or directory
```

In directory `luna-sdk_*` you can find shared libraries 
- for `linux` the path is `luna-sdk_*/lib/gcc4/x64/`
- for `windows` they are placed in `luna-sdk_*/bin/vs2015/x64`.

If you built FaceEngine bindings please do not move this directories or you can try to do such steps:
1. Set system varible `FSDK_ROOT` in your system. 
- In a case of linux it should be the absolute path to `luna-sdk_linux*`
- In a case of windows < 10 and python < 3.8.0 it should be the absolute path to `luna-sdk_windows*`
- In a case of windows 10 and python >= 3.8.0 it should be the absolute path to `luna-sdk_windows*\bin\vs2015\x64\`
2. Before importing of FaceEngine to your sript please import `set_shared_lib_dir` from `luna-sdk_*/pythonBindings/examples/fsdk_utils.py`. 
Example:

```python
from fsdk_utils import set_shared_lib_dir

# call set_shared_lib_dir before FaceEngine importing, system variable FSDK_ROOT should be set
set_shared_lib_dir()

import FaceEngine as fe
``` 
3. If such steps do not help please set next system variables by hands
for windows - `PATH`, for linux - `LD_LIBRARY_PATH`.

## Usage on Linux
 
### if library FaceEngine*.so was installed by setup.py

**run a script**:

```bash
$ python3 your_script.py
```

**usage example**

```python
import FaceEngine as f
faceEnginePtr = f.createFaceEngine("data")
```

### if library FaceEngine*.so was generated by CMake without installing

If FaceEngine*.so was built from sources by CMake pass the path to directory with FaceEngine*.so and add it to system paths:

**run a script**:

```bash
$ python3 your_script.py <path to dir with FaceEngine*.so>
```
**usage example**

```python
import sys
sys.path.append(sys.argv[1])
import FaceEngine as fe
```

Other usage examples could be found in `pythonBindings/examples`.

### Running examples on Linux
Note: data folder must be at \<LUNA_SDK_root\>/data on Linux.
From `FSDK_ROOT`:

```
# Extracting, Matching, this example only for full version (not for front-edition) of LUNS SDK
$ python3 pythonBindings/examples/example_extractor_matcher.py pythonBindings/build <some warped image1> <some warped image2> <some warped image3>

# Detecting, Landmarks, Estimating (Attributes, Quality, Eyes, Head pose and so on)
$ python3 pythonBindings/examples/example_estimators.py pythonBindings/build <some image>
 
# Detecting and warping
$ python3 pythonBindings/examples/example_detector_warper.py pythonBindings/build <some image>

# Human Detecting 
$ python3 pythonBindings/examples/example_human_detector.py pythonBindings/build <some image with human bodies>

# Gaze Estimation demo, requires connected video camera
$ python3 pythonBindings/examples/example_gaze_cv.py pythonBindings/build

# Human descriptor extracting, only for server platforms, not for Jetson
$ python3 pythonBindings/examples/example_human_extractor.py pythonBindings/build <some image with human bodies 1> <some image with human bodies 2>

# Depth example.
$ python3 pythonBindings/examples/example_depth.py --data data --bindPath pythonBindings/build --rsbindPath <absolute_path_to_realsense_python_bindings_libraries>
```

***Note for Linux users***
If you built realsense sdk from source make sure to tell the system where these libraries are located:

```
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:<absolute_path_to_realsense_libraries>
```

**usage of FaceEngine, example**
```

```python
import FaceEngine as fe
faceEnginePtr = fe.createFaceEngine("data")
```

## Usage on Windows**

Pass the path to directory with FaceEngine.lib and add it to system paths.

Note: In a case of python3.8 and Windows10 was detected the problem with relative paths. Please pass absolute paths, for example `<absolute path to directory with FaceEngine*.lib>`.

**run a script**:

```cmd
$ python3 your_script.py <path to directory with FaceEngine*.lib>
```
**usage example**

```python
import sys
sys.path.append(sys.argv[1])
import FaceEngine as fe
```
### Running examples on Windows

Please verify path with built FaceEngine.lib for your version of python. Usual path is `pythonBindings/build/release`.

```cmd
$ python3 pythonBindings/examples/example_estimators.py pythonBindings/build/release <some image>
$ python3 pythonBindings/examples/example_detector_warper.py pythonBindings/build/release <some image>
$ python3 pythonBindings/examples/example_human_detector.py pythonBindings/build/release <some image with human bodies>
$ python3 pythonBindings/examples/example_extractor_matcher.py pythonBindings/build/release <some warped image1> <some warped image2> <some warped image3>
$ python3 pythonBindings/examples/example_human_extractor.py pythonBindings/build/release <some image with human bodies 1> <some image with human bodies 2>
```
Note: In a case of python3.8 and Windows10 was detected the problem with relative paths. Please pass absolute path to directory with dll, for example `python3 pythonBindings/examples/example_estimators.py C:/Users/user/luna_sdk_*/pythonBindings/build/Release <some image>`.

## About pybind11
PythonBindings use pybind11. pybind11 is a lightweight header-only library that exposes C++ types in Python and vice versa, mainly to create Python bindings of existing C++ code. Its goals and syntax are similar to the excellent Boost.Python library by David Abrahams: to minimize boilerplate code in traditional extension modules by inferring type information using compile-time introspection. Think of this library as a tiny self-contained version of Boost.Python with everything stripped away that isn't relevant for binding generation. Without comments, the core header files only require ~4K lines of code and depend on Python (2.7 or 3.x) and the C++ standard library. This compact implementation was possible thanks to some of the new C++11 language features (specifically: tuples, lambda functions and variadic templates). Since its creation, this library has grown beyond Boost.Python in many ways, leading to dramatically simpler binding code in many common situations.

Tutorial and reference documentation is provided at [doc](http://pybind11.readthedocs.org/en/master). A PDF version of the manual is available [here](https://media.readthedocs.org/pdf/pybind11/master/pybind11.pdf).

## pybind11 license

pybind11 is provided under a BSD-style license that can be found in the LICENSE
file. By using, distributing, or contributing to this project, you agree to the
terms and conditions of this license.

## Examples of code
As described before first you need to create main object of FaceEngine:

```python
# if FaceEngine was not installed pass path to directory with FaceEngine*.so and add it to system paths
sys.path.append(sys.argv[1])
import FaceEngine as fe

```
Using of basic methods you can see in examples. More detailed information about methods and classes you can see in `src/FaceEngine.cpp`.

### Creating of basic objects

```python
config = fe.createSettingsProvider("data/faceengine.conf")
attributeEstimator = faceEngine.createAttributeEstimator()
qualityEstimator = faceEngine.createQualityEstimator()
blackWhiteEstimator = faceEngine.createBlackWhiteEstimator()
detector = faceEngine.createDetector(fe.FACE_DET_V1)
warper = faceEngine.createWarper()
descriptor = faceEngine.createDescriptor()
descriptorExtractor = faceEngine.createExtractor()
```
and so on.

### Landmarks

Landmarks are special classes binded to python. They are similar on python lists. It is possible to use some standard python built-in functions for them: `__len__`, `__getitem__`. The method `__setitem__` is used only for testing purposes. The length of definity type of landmarks is always fixed. For example len(landmarks5) is 5.

```python
for i in range(len(landmarks)):
        print(landmarks[i])
```

### Arrays

In some cases C++ arrays are casted to python lists.
For example:

```
.def("detect",[](
    const fsdk::IDetectorPtr& det,
    const fsdk::Image& image,
    const fsdk::Rect& rect,
    uint32_t maxCount) {
      std::vector<fsdk::Detection> detections(maxCount);
        std::vector<fsdk::Landmarks5> landmarks(maxCount);
        std::vector<fsdk::Landmarks68> landmarks68(maxCount);
        fsdk::ResultValue<fsdk::FSDKError, int> err = det->detect(
            image,
            rect,
            detections.data(),
            landmarks.data(),
            landmarks68.data(),
            maxCount);
        
        if (err.isOk()) {
            const uint32_t size = err.getValue();
            auto detectionResultPyList = py::list(size);
            for (uint32_t i = 0; i < size; ++i) {
                detectionResultPyList[i] = std::make_tuple(detections[i], landmarks[i], landmarks68[i]);
            }
            return std::make_tuple(FSDKErrorValueInt(err), detectionResultPyList);
        } else {
            return std::make_tuple(FSDKErrorValueInt(err), py::list());
        }},
    "Detect faces and landmarks on the image\n"
    "\tArgs:\n"
    "\t\tparam1 (Image): input image. Format must be R8G8B8\n"
    "\t\tparam2 (Rect): rect of interest inside of the image\n"
    "\t\tparam3 (int): length of `detections` and `landmarks` arrays\n"
    "\tReturns:\n"
    "\t\t(tuple with FSDKErrorValueInt code and list of tuples): \n"
    "\t\t\ttuple with FSDKErrorValueInt code and list of tuples from\n"
    "\t\t\tDetection, Landmarks5, Landmarks68see FSDKErrorValueInt (see FSDKErrorValueInt)\n")
```

**usage example**
```python
    detector = faceEngine.createDetector(fe.FACE_DET_V1)
    max_detections = 3
    print("Image for detection: ", _image_det.getHeight(), _image_det.getWidth(), _image_det.isValid())
    detector_result = detector.detect(_image_det, _image_det.getRect(), max_detections)
    print("detector result = ", detector_result)
    print("Detections: ")
    for i, item in enumerate(detector_result, 1):
        print(i, item)
```

### Enums
```c++
py::enum_<fsdk::Format::Type>(f, "FormatType")
	.value("Unknown", fsdk::Format::Unknown)
	.value("B8G8R8X8", fsdk::Format::B8G8R8X8)
	.value("R8G8B8X8", fsdk::Format::R8G8B8X8)
	.value("B8G8R8", fsdk::Format::B8G8R8)
	.value("R8G8B8", fsdk::Format::R8G8B8)
	.value("R8", fsdk::Format::R8)
	.value("R16", fsdk::Format::R16)
		;
```
**usage example**
```python
print(fe.FormatType.R8)
```

### SettingsProvider
SettingsProvider has quite difficult structure. 
Usage example you can see in `example_detector_warper.py`

```python
    config = fe.createSettingsProvider("data/faceengine.conf")
    config_path = config.getDefaultPath()
    print("Config settings: DefaultPath {0}".format(config_path))
    config.setValue("system", "verboseLogging", value)
    faceEngine.setSettingsProvider(config)
    val = config.getValue("system", "verboseLogging")
    print("Config settings: \"system\", \"verboseLogging\" = {0}".format(val))

```

# Troubleshooting and diagnostics

## Diagnostics

The PythonBindings module for VisionLabs LUNA SDK is just a Python wrapper for C++ LUNA SDK library. The main functionality is implemented in the C++ LUNA SDK library - not in the PythonBindings.

In case of wrong behaviour of the some code with using the PythongBindings or in case of wrong results we recomend to diagnostic the problem with SDK library tools, but not with PythonBindings.

The next ways to make a diagnostics with the LUNA SDK tools are recomended:

1. Try to run examples from the LUNA SDK package (see the ExamplesGuide.pdf from the LUNA SDK package for details).

2. Try to enable verbose logging for the LUNA SDK and run the code again. See the logs for detailed information about execution (see the ConfigurationGuide.pdf from the LUNA SDK package for details).


This PythonBindings module also contains some tools for diagnostics:

1. Try to read examples for correct usage of the functionallity (see the "examples" folder in the PythonBindings package).


## Troubleshooting

Several known issues and way to fix them are presented here:

1. FaceEngine.createXXXX (for example, FaceEngine.createDetector) raises an exception.

Possible reasons:
- Wrong path to the "data" folder.
Please, check path to the "data" folder of the LUNA SDK. See the QuickStartGuide.pdf from the LUNA SDK package for details.

- Licensing issue - license is not activated or the License Management server if not responding.
The PythonBindins package does not require any license for now. So, all licensing issues are related to the LUNA SDK licensing only.
Please, check the license for the LUNA SDK (see the FaceEngine_Handbook.pdf from the LUNA SDK package for details).

- Wrong configuration file.
The LUNA SDK modules use the LUNA SDK configuration file during initialization. If this file is corrupted or contains invalid parameters, LUNA SDK modules could not be initialized correctly (See the ConfigurationGuide.pdf from the LUNA SDK package for details).

- Absense of one or a few configuration files such as faceengine.conf, runtime.conf, cnnXX_descriptor.conf and etc in "data" folder of the LUNA SDK.
The LUNA SDK modules use different configuration files during initialization. If this files are corrupted or contain invalid parameters, LUNA SDK modules could not be initialized correctly (See the ConfigurationGuide.pdf from the LUNA SDK package for details).

2. FaceEngine.createDescriptor or FaceEngine.createDescriptorBatch raises an exception.

Possible reasons:
- The same as 1.

- Current LUNA SDK package is a Frontend Edition which does not support the extraction and matching.

3. Problem during configuring and generating by CMake with relative pass. 

```
CMake Error: The following variables are used in this project, but they are set to NOTFOUND.
Please set them or make sure they are set and tested correctly in the CMake files:
FSDK_INCLUDE_DIRS (ADVANCED)
...
LSDK_INCLUDE_DIRS (ADVANCED)
...
TSDK_INCLUDE_DIRS (ADVANCED)
...
```
Please pass absolute path to directory LUNA SDK root, for example `-DFSDK_ROOT=<absolute_path>`.

4. Problem during importing module of FaceEngine. 
```
ModuleNotFoundError: No module named 'FaceEngine'
```
Please pass absolute path to directory with FaceEngine*.so and add it to 'sys.path'.
```bash
$ python3 your_script.py <absolute path to dir with built FaceEngine*.so>
```

5. Problem with system python libraries and headers on Linux-based systems:

```
fatal error: Python.h: No such file or directory
 #include <Python.h>
```
Try to install associated pythonx.x-dev package.
Try to use next advises:
[problems](https://stackoverflow.com/questions/21530577/fatal-error-python-h-no-such-file-or-directory)