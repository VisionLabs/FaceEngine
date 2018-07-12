# Find the Vision Labs Face SDK.
# Sets the usual variables expected for find_package scripts:
# FSDK_INCLUDE_DIRS - headers location
# FSDK_LIBRARIES - libraries to link against
# FSDK_FOUND - true if Face SDK was found.

# This is the directory where the Face SDK is located.
# By default FSDKDIR environment variable value is taken.
set(FSDK_ROOT "$ENV{FSDKDIR}" CACHE PATH "Vision Labs Face SDK root directory.")

# Look for headers.
find_path(FSDK_INCLUDE_DIRS
          NAMES fsdk/FaceEngine.h fsdk/Types.h fsdk/Def.h
          HINTS $ENV{FSDKDIR}
          PATHS ${FSDK_ROOT}
          PATH_SUFFIXES include)

# iOS framework have different header structure: not flower/header.h, but Headers/header.h. But when you link against framework it's name is used as prefix for includes, so you still use its includes as flower/header.h in c++ code.
# Now the reason to set this variable is that its used for every other platform and its easier to fake it than rewrite most cmakelists 
if(IOS OR ANDROID)
    set(FSDK_INCLUDE_DIRS ${FSDK_ROOT}/include)
endif()

#message(STATUS "FSDK: FSDK_INCLUDE_DIRS = ${FSDK_INCLUDE_DIRS}.")

# Determine compiler version and architecture.
# ios has no architechture/compiler branching, because only can only use clang
# and if you need multiple architechtures there still compiled into single universal binary
if(NOT IOS)
	if(MSVC10)
		set(FSDK_COMPILER_NAME vs2010)
	elseif(MSVC11)
		set(FSDK_COMPILER_NAME vs2012)
	elseif(MSVC12)
		set(FSDK_COMPILER_NAME vs2013)
	elseif(MSVC14)
		set(FSDK_COMPILER_NAME vs2015)
	elseif(CMAKE_COMPILER_IS_GNUCXX)
		set(FSDK_COMPILER_NAME gcc4)
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(FSDK_COMPILER_NAME clang)
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
		set(FSDK_COMPILER_NAME clang)
	else()
		message(SEND_ERROR "Unsupported compiler: ${FSDK_COMPILER_NAME}")
	endif()

	if(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(FSDK_TARGET_NAME x64)
	else()
		set(FSDK_TARGET_NAME x86)
	endif()

	# What version of Face SDK to use.
	set(FSDK_LIB_PREFIX ${FSDK_COMPILER_NAME}/${FSDK_TARGET_NAME})
endif()

if(IOS)
	# List of all SDK libraries.
	set(FSDK_LIB_NAMES
		fsdk)
else()
	# List of all SDK libraries.
	set(FSDK_LIB_NAMES
		FaceEngineSDK)
endif()

if(IOS)
    set(FSDK_LIB_PATH_SUFFIX Frameworks)
    set(FSDK_BIN_PATH_SUFFIX Frameworks)
else()
    set(FSDK_LIB_PATH_SUFFIX lib/${FSDK_LIB_PREFIX})
    set(FSDK_BIN_PATH_SUFFIX bin/${FSDK_LIB_PREFIX})
endif()

# Find optimized libraries.
if(ANDROID)
	foreach(LIB ${FSDK_LIB_NAMES})
		list(APPEND FSDK_LIB "${FSDK_ROOT}/lib/${FSDK_COMPILER_NAME}/${ANDROID_ABI}/lib${LIB}.so")
	endforeach()
else()
set(FSDK_LIB)
foreach(LIB ${FSDK_LIB_NAMES})
	set(LIB_PATH ${LIB}-NOTFOUND)
	find_library(LIB_PATH
		NAMES ${LIB}
		HINTS $ENV{FSDKDIR}
		PATHS ${FSDK_ROOT}
		PATH_SUFFIXES ${FSDK_LIB_PATH_SUFFIX}
			${FSDK_BIN_PATH_SUFFIX}
		NO_DEFAULT_PATH)
	list(APPEND FSDK_LIB ${LIB_PATH})
endforeach()
endif()

# Find debug libraries (may be missing in your distribution).
set(FSDK_LIBD)
foreach(LIB ${FSDK_LIB_NAMES})
	set(LIB_PATH ${LIB}-NOTFOUND)
	find_library(LIB_PATH
		NAMES ${LIB}d
		HINTS $ENV{FSDKDIR}
		PATHS ${FSDK_ROOT}
		PATH_SUFFIXES	${FSDK_LIB_PATH_SUFFIX}
			${FSDK_BIN_PATH_SUFFIX}
		NO_DEFAULT_PATH)

	list(APPEND FSDK_LIBD ${LIB_PATH})
endforeach()

if(EMSCRIPTEN)
	if(EXISTS "${FSDK_ROOT}/include/fsdk/FaceEngine.h")
		message(STATUS "FindFSDK:${FSDK_ROOT}/include/fsdk/FaceEngine.h exists!")
		set(FSDK_INCLUDE_DIRS ${FSDK_ROOT}/include)
	else()
		message(ERROR "FindFSDK:${FSDK_ROOT}/include/fsdk/FaceEngine.h not found...")
	endif()

	if(EXISTS "${FSDK_ROOT}/lib/clang/x86/libFaceEngineSDK.bc")
		message(STATUS "FindFSDK: ${FSDK_ROOT}/lib/clang/x86/libFaceEngineSDK.bc exists!")
		set(FSDK_LIB)
		set(FSDK_LIB ${FSDK_LIB} "${FSDK_ROOT}/lib/clang/x86/libvlc.bc")
		set(FSDK_LIB ${FSDK_LIB} "${FSDK_ROOT}/lib/clang/x86/libflower.bc")
		set(FSDK_LIB ${FSDK_LIB} "${FSDK_ROOT}/lib/clang/x86/libCore.bc")
		set(FSDK_LIB ${FSDK_LIB} "${FSDK_ROOT}/lib/clang/x86/libDetector.bc")
		set(FSDK_LIB ${FSDK_LIB} "${FSDK_ROOT}/lib/clang/x86/libEstimator.bc")
		set(FSDK_LIB ${FSDK_LIB} "${FSDK_ROOT}/lib/clang/x86/libFlowerInterop.bc")
		set(FSDK_LIB ${FSDK_LIB} "${FSDK_ROOT}/lib/clang/x86/libLicensing.bc")
		set(FSDK_LIB ${FSDK_LIB} "${FSDK_ROOT}/lib/clang/x86/libMTCNN.bc")
		set(FSDK_LIB ${FSDK_LIB} "${FSDK_ROOT}/lib/clang/x86/libOpenCVInterop.bc")
		set(FSDK_LIB ${FSDK_LIB} "${FSDK_ROOT}/lib/clang/x86/libOpticalFlow.bc")
		set(FSDK_LIB ${FSDK_LIB} "${FSDK_ROOT}/lib/clang/x86/libWarper.bc")
		set(FSDK_LIB ${FSDK_LIB} "${FSDK_ROOT}/lib/clang/x86/libFaceEngineSDK.bc")
	else()
		message(ERROR "FindFSDK: ${FSDK_ROOT}/lib/clang/x86/libFaceEngineSDK.bc not found...")
	endif()
endif()

# Support the REQUIRED and QUIET arguments, and set FSDK_FOUND if found.
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FSDK DEFAULT_MSG 
                                  FSDK_LIB
                                  FSDK_INCLUDE_DIRS)

set(FSDK_LIBRARIES)


if(FSDK_FOUND)
	if(FSDK_LIB)
		foreach(LIB ${FSDK_LIB})
			list(APPEND FSDK_LIBRARIES optimized ${LIB})
		endforeach()
	endif()
	if(FSDK_LIBD)
		foreach(LIB ${FSDK_LIBD})
			list(APPEND FSDK_LIBRARIES debug ${LIB})
		endforeach()
		message(STATUS "FSDK [INFO]: Debug libraries are available.")
	elseif(FSDK_LIB)
		foreach(LIB ${FSDK_LIB})
			list(APPEND FSDK_LIBRARIES debug ${LIB})
		endforeach()
		message(STATUS "FSDK [WARN]: Debug libraries are NOT available.")
	endif()

	message(STATUS "FSDK [INFO]: Found SDK in ${FSDK_ROOT}.")
else()
	message(STATUS "FSDK [WARN]: SDK was NOT found.")
endif(FSDK_FOUND)

#message(STATUS "FSDK [DEBUG]: FSDK_LIBRARIES = ${FSDK_LIBRARIES}.")

# Don't show in GUI
mark_as_advanced(
	FSDK_INCLUDE_DIRS
	FSDK_LIBRARIES
	FSDK_COMPILER_NAME
	FSDK_TARGET_NAME
	FSDK_LIB_PREFIX
	FSDK_LIB_NAMES
	FSDK_LIB_NAMESD
	FSDK_LIB 
	FSDK_LIBD
	LIB_PATH
)
