# LUNA SDK python bindings

Welcome to the **Beta** of PythonBindings for VisionLabs LUNA SDK.
Thank you all for participating of Beta Test.
Please send us your feedback and suggestions to email (info@visionlabs.ru).
With all of your feedback and suggestions you can help us to create very useful feature - calling methods of LUNA SDK through python.

IN pythonBindings there are some supplementary materials you may find useful.
Look into *cmake/* folder for a CMake find script for the SDK. You are not forced to use
CMake but we advise for it.

Currently we support Linux. On Windows everything should work with
Visual Studio 2015 but we do not test this platform. On Linux we tested this code with GCC 4.8.5.
Other versions may work as well. Note, that the SDK is officially supported on RedHat
Linux families (RHEL, CentOS, Fedora).

Only basic methods and classes of LUNA SDK were binded.

**On Unix (Linux, Ubuntu 14)**

* A compiler with C++11 support
* CMake >= 2.8.12

**On Windows**
was not tested.
* Visual Studio 2015 (required for all Python versions, see notes below)
* CMake >= 3.1

## Installation
From source files of pythonBindings generated a shared library `FaceEngine*.so`.
This library can be installed in system or used without installing.
 
You should do such steps:

1. Download LUNA SDK.

2. Choose directory with pythonBindings within LUNA SDK.

```bash
cd pythonBindings
```
3. **For library building and installing**:

```bash
$ python setup.py install 
```
Library `FaceEngine*.so` will be generated and installed in system.

to uninstall it:

```bash
$ pip uinstall FaceEngine 
```
Possible you'll need sudo rights or use python3 and pip3.

3. **For library building without installing**:

If you want to **build from source files by CMake without installing**:

```bash
$ mkdir build
$ cd build
$ cmake .. -DFSDK_ROOT=<path_to_LUNA_SDK>
$ cmake --build . --config Release
```

If you want to point not default version of python or custom path to LUNA SDK you can write for python 2.7:

**Example of building command (from FSDK_ROOT/build):**

```bash
$ cmake .. -DFSDK_ROOT=<FSDK_path> -DCMAKE_INSTALL_PREFIX=./install -DPYTHON_EXECUTABLE:FILEPATH=/usr/bin/python
-DPYTHON_LIBRARY=$(python-config --prefix)/lib/libpython2.7.lib -DPYTHON_INCLUDE_DIR=$(python-config --prefix)/include/python2.7
```
**Example building command (from FSDK_ROOT/build):**
or for `python 3.6`:

```bash
$ cmake .. -DFSDK_ROOT=<FSDK_path> -DCMAKE_INSTALL_PREFIX=./install -DPYTHON_EXECUTABLE:FILEPATH=/usr/bin/python3.6
-DPYTHON_LIBRARY=$(python-config --prefix)/lib/libpython3.6.lib -DPYTHON_INCLUDE_DIR=$(python-config --prefix)/include/python3.6
```

## Usage
 
### Library FaceEngine*.so was installed by setup.py

**run a script**:

```bash
$ python script.py
```

**usage example**

```python
import FaceEngine as f
faceEnginePtr = f.createFaceEngine("data", "data/faceengine.conf")
```

### Module FaceEngine*.so was generated by CMake without installing

**run a script**:

```bash
$ python script.py <path to FSDK_ROOT>
```

If FaceEngine*.so was built from sources by CMake pass the path to directory with FaceEngine*.so and add it to system paths:

**usage example**

```python
import sys
sys.path.append(sys.argv[1])
import FaceEngine as fe
```

Other usage examples could be found in `pythonBindings/tests` and `pythonBindings/examples`.

## Running tests

From `FSDK_ROOT/pythonBindings`:

```bash
$ python3 pythonBindings/tests/test_image.py --bind-path pythonBindings/build
$ python3 pythonBindings/tests/test_rect.py --bind-path pythonBindings/build
$ python3 pythonBindings/tests/test_estimators.py --bind-path pythonBindings/build
```
## Running examples
Note: data folder must be at \<LUNA_SDK_root\>/data on Windows and on Linux.

```
# Extracting, Matching
$ python3 pythonBindings/examples/example_extractor_matcher.py pythonBindings/build testData/photo_2017-03-30_14-47-43_p.ppm testData/warp1.ppm testData/warp2.ppm

# Detecting, Landmarks, Estimating (Attributes, Quality, Eyes, Head pose and so on)
$ python3 pythonBindings/examples/example.py pythonBindings/build testData/photo_2017-03-30_14-47-43_p.ppm
 
# Detecting and warping
$ python3 pythonBindings/examples/example_detector_warper.py pythonBindings/build testData/photo_2017-03-30_14-47-43_p.ppm
```
**usage of FaceEngine, example**

```python
import FaceEngine as fe
faceEnginePtr = fe.createFaceEngine("data", "data/faceengine.conf")
```

## Special notes for Windows

**Compiler requirements**

PythonBindings for Windows were not tested.
Pybind11 requires a C++11 compliant compiler, i.e Visual Studio 2015 on Windows.
This applies to all Python versions, including 2.7. 

## About pybind11
PythonBindings use pybind11. pybind11 is a lightweight header-only library that exposes C++ types in Python and vice versa, mainly to create Python bindings of existing C++ code. Its goals and syntax are similar to the excellent Boost.Python library by David Abrahams: to minimize boilerplate code in traditional extension modules by inferring type information using compile-time introspection. Think of this library as a tiny self-contained version of Boost.Python with everything stripped away that isn't relevant for binding generation. Without comments, the core header files only require ~4K lines of code and depend on Python (2.7 or 3.x) and the C++ standard library. This compact implementation was possible thanks to some of the new C++11 language features (specifically: tuples, lambda functions and variadic templates). Since its creation, this library has grown beyond Boost.Python in many ways, leading to dramatically simpler binding code in many common situations.

Tutorial and reference documentation is provided at [doc](http://pybind11.readthedocs.org/en/master). A PDF version of the manual is available [here](https://media.readthedocs.org/pdf/pybind11/master/pybind11.pdf).

## pybind11 license

pybind11 is provided under a BSD-style license that can be found in the LICENSE
file. By using, distributing, or contributing to this project, you agree to the
terms and conditions of this license.

## Examples of code
As described befor first you need to create main object of FaceEngine:

```python
# if FaceEngine was not installed pass path to directory with FaceEngine*.so and add it to system paths
sys.path.append(sys.argv[1])
import FaceEngine as fe

```
Using of basic methods you can see in examples or tests. More detailed information about methods and classes you can see in `src/FaceEngine.cpp`.

```c++
py::class_<fsdk::IEthnicityEstimatorPtr>(f, "IEthnicityEstimatorPtr")
	.def("estimate",[](
		const fsdk::IEthnicityEstimatorPtr& est,
		const fsdk::Image &warp) {
			fsdk::EthnicityEstimation out;
			fsdk::Result<fsdk::FSDKError> err = est->estimate(warp, out);
			if (err.isOk())
				return py::cast(out);
			else
				return py::cast(FSDKErrorResult(err)); })
			;
```

Here you can see class `fsdk::IEthnicityEstimatorPtr`, method `estimate`, its specification. You should pass image warp, the result will be `ethnicityEstimation` struct or error. 
How to use this from python, see `example.py`:

```python
import FaceEngine as fe
faceEngine = fe.createFaceEngine("data", "data/faceengine.conf")
image = fe.Image()
image.load("testData/Warp1.ppm")
ethnicityEstimator = faceEngine.createEthnicityEstimator()
ethnicity_result = ethnicityEstimator.estimate(image)
print("Ethnicity estimation result {0}".format(ethnicity_result))
```

###Creating of basic objects

```python
config = fe.createSettingsProvider("data/faceengine.conf")
attributeEstimator = faceEngine.createAttributeEstimator()
qualityEstimator = faceEngine.createQualityEstimator()
ethnicityEstimator = faceEngine.createEthnicityEstimator()
blackWhiteEstimator = faceEngine.createBlackWhiteEstimator()
smileEstimator = faceEngine.createSmileEstimator()
detector = faceEngine.createDetector(fe.ODT_MTCNN)
warper = faceEngine.createWarper()
descriptor = faceEngine.createDescriptor()
descriptorExtractor = faceEngine.createExtractor()
```
and so on.

###Landmarks

Landmarks are special classes binded to python. They are similar on python lists. It is possible to use some standard python built-in functions for them: `__len__`, `__getitem__`. The method `__setitem__` is used only for testing purposes. The length of definity type of landmarks is always fixed. For example len(landmarks5) is 5.

```python
for i in range(len(landmarks)):
        print(landmarks[i])
```

###Arrays

In some cases C++ arrays are casted to python lists.
For example:
```C++
py::class_<fsdk::IDetectorPtr>(f, "IDetectorPtr")
	.def("detect",[](
		const fsdk::IDetectorPtr& det,
		const fsdk::Image& image,
		const fsdk::Rect& rect,
		int maxCount) {
			fsdk::Detection detections[maxCount];
			fsdk::Landmarks5 landmarks[maxCount];
			fsdk::Landmarks68 landmarks68[maxCount];
			fsdk::ResultValue<fsdk::FSDKError, int> err = det->detect(
				image,
				rect,
				detections,
				landmarks,
				landmarks68,
				maxCount);
			auto detectionResultPyList = py::list();
			if (err.isOk()) {
				for (size_t i = 0; i < maxCount; ++i) {
					detectionResultPyList.append(std::make_tuple(detections[i], landmarks[i], landmarks68[i]));
				}
				return detectionResultPyList;
			}
			else {
				detectionResultPyList.append(py::cast(FSDKErrorValueInt(err)));
				return detectionResultPyList; } })
				;
```
**usage example**
```python
    detector = faceEngine.createDetector(fe.ODT_MTCNN)
    max_detections = 3
    print("Image for detection: ", _image_det.getHeight(), _image_det.getWidth(), _image_det.isValid())
    detector_result = detector.detect(_image_det, _image_det.getRect(), max_detections)
    print("detector result = ", detector_result)
    print("Detections: ")
    for i, item in enumerate(detector_result, 1):
        print(i, item)
```

###Enums
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

###SettingsProvider
SettingsProvider has quite difficult structure. 
Usage example you can see in `example_detector_warper.py`

```python
    config = fe.createSettingsProvider("data/faceengine.conf")
    config_path = config.getDefaultPath()
    print("Config settings: DefaultPath {0}".format(config_path))
    config.setValue("system", "verboseLogging", fe.SettingsProviderValue(value))
    faceEngine.setSettingsProvider(config)
    val = config.getValue("system", "verboseLogging")
    print("Config settings: \"system\", \"verboseLogging\" = {0}".format(val.asInt()))

```


