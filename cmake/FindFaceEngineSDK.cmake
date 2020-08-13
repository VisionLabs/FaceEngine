# Find the Vision Labs Face SDK.
# Sets the usual variables expected for find_package scripts:
# FSDK_INCLUDE_DIRS - headers location
# FSDK_LIBRARIES - libraries to link against
# FSDK_RUNTIME_LIBRARIES - runtime libraries
# FSDK_FOUND - true if Face SDK was found.

# This is the directory where the Face SDK is located.
# By default FSDKDIR environment variable value is taken.
set(FSDK_ROOT "$ENV{FSDKDIR}" CACHE PATH "Vision Labs Face SDK root directory.")

if(ANDROID)
	set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
	set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
	set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
endif()

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

if(WIN32)
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".lib" ".dll")
elseif(APPLE AND NOT IOS) # mac os
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a" ".dylib")
else()
    SET(CMAKE_FIND_LIBRARY_SUFFIXES ".so" ".a")
endif()

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
		message(SEND_ERROR "[FindFSDK] Unsupported compiler: ${FSDK_COMPILER_NAME}")
	endif()

	if(ANDROID)
		set(FSDK_TARGET_NAME ${ANDROID_ABI})
	elseif(CMAKE_SIZEOF_VOID_P EQUAL 8)
		set(FSDK_TARGET_NAME x64)
	else()
		set(FSDK_TARGET_NAME x86)
	endif()

	# What version of Face SDK to use.
	set(FSDK_LIB_PREFIX ${FSDK_COMPILER_NAME}/${FSDK_TARGET_NAME})
endif()


if(IOS)
    set(FSDK_LIB_PATH_SUFFIX Frameworks)
    set(FSDK_BIN_PATH_SUFFIX Frameworks)
else()
    set(FSDK_LIB_PATH_SUFFIX lib/${FSDK_LIB_PREFIX})
    set(FSDK_BIN_PATH_SUFFIX bin/${FSDK_LIB_PREFIX})
endif()


macro(find_fsdk_library OUTPUT_LIST)
	foreach(LIBRARY_NAME ${ARGN})
		set(CURRENT_LIB LIBRARY_VAR-NOTFOUND)
		find_library(CURRENT_LIB
			NAMES ${LIBRARY_NAME}
			HINTS $ENV{FSDKDIR}
			PATHS ${FSDK_ROOT}
			PATH_SUFFIXES ${FSDK_LIB_PATH_SUFFIX}
			NO_DEFAULT_PATH
		)
		list(APPEND ${OUTPUT_LIST} ${CURRENT_LIB})
	endforeach()
endmacro()

if(IOS)
	find_fsdk_library(FSDK_LIBRARY_RELEASE fsdk)
	find_fsdk_library(FSDK_LIBRARY_DEBUG fsdkd)
elseif(EMSCRIPTEN)
	if(EXISTS "${FSDK_ROOT}/include/fsdk/FaceEngine.h")
		message(STATUS "[FindFSDK]:${FSDK_ROOT}/include/fsdk/FaceEngine.h exists!")
		set(FSDK_INCLUDE_DIRS ${FSDK_ROOT}/include)
	else()
		message(ERROR "[FindFSDK]:${FSDK_ROOT}/include/fsdk/FaceEngine.h not found...")
	endif()

	if(EXISTS "${FSDK_ROOT}/lib/clang/x86/libFaceEngineSDK.bc")
		message(STATUS "[FindFSDK]: ${FSDK_ROOT}/lib/clang/x86/libFaceEngineSDK.bc exists!")
		list(APPEND FSDK_LIBRARY_RELEASE "${FSDK_ROOT}/lib/clang/x86/libvlc.bc")
		list(APPEND FSDK_LIBRARY_RELEASE "${FSDK_ROOT}/lib/clang/x86/libflower.bc")
		list(APPEND FSDK_LIBRARY_RELEASE "${FSDK_ROOT}/lib/clang/x86/libSdkCore.bc")
		list(APPEND FSDK_LIBRARY_RELEASE "${FSDK_ROOT}/lib/clang/x86/libDetector.bc")
		list(APPEND FSDK_LIBRARY_RELEASE "${FSDK_ROOT}/lib/clang/x86/libEstimator.bc")
		list(APPEND FSDK_LIBRARY_RELEASE "${FSDK_ROOT}/lib/clang/x86/libLicensing.bc")
		list(APPEND FSDK_LIBRARY_RELEASE "${FSDK_ROOT}/lib/clang/x86/libMTCNN.bc")
		list(APPEND FSDK_LIBRARY_RELEASE "${FSDK_ROOT}/lib/clang/x86/libOpticalFlow.bc")
		list(APPEND FSDK_LIBRARY_RELEASE "${FSDK_ROOT}/lib/clang/x86/libWarper.bc")
		list(APPEND FSDK_LIBRARY_RELEASE "${FSDK_ROOT}/lib/clang/x86/libFaceEngineSDK.bc")
	else()
		message(ERROR "[FindFSDK]: ${FSDK_ROOT}/lib/clang/x86/libFaceEngineSDK.bc not found...")
	endif()
else()
	find_fsdk_library(FSDK_LIBRARY_RELEASE FaceEngineSDK SdkCore)
	find_fsdk_library(FSDK_LIBRARY_DEBUG FaceEngineSDKd SdkCored)
endif()


#link components
if(FSDK_LIBRARY_RELEASE  AND FSDK_LIBRARY_DEBUG)
    set(FSDK_LIBRARIES optimized ${FSDK_LIBRARY_RELEASE} debug ${FSDK_LIBRARY_DEBUG})
elseif(FSDK_LIBRARY_DEBUG)
	set(FSDK_LIBRARIES ${FSDK_LIBRARY_DEBUG})
	message(STATUS "[FindFSDK]: Release libraries were not found")
elseif(FSDK_LIBRARY_RELEASE)
	set(FSDK_LIBRARIES ${FSDK_LIBRARY_RELEASE})
	message(STATUS "[FindFSDK]: Debug libraries were not found")
else()
	message(FATAL_ERROR "[FindFSDK]: Face Engine link libraries were not found")
endif()

#runtime components
if(WIN32)
	#grab all runtime dependencies for windows build
	file(GLOB FSDK_RUNTIME_LIBRARIES
		LIST_DIRECTORIES FALSE
		${FSDK_ROOT}/${FSDK_BIN_PATH_SUFFIX}/*.dll
	)
	if(NOT FSDK_RUNTIME_LIBRARIES)
		message(FATAL_ERROR "[FindFSDK]: Face Engine runtime libraries were not found")
	endif()
else()
	list(APPEND FSDK_RUNTIME_LIBRARIES ${FSDK_LIBRARIES})
endif()

message(STATUS "[FindFSDK]: FSDK_INCLUDE_DIRS: ${FSDK_INCLUDE_DIRS}")
message(STATUS "[FindFSDK]: FSDK_LIBRARIES: ${FSDK_LIBRARIES}")
message(STATUS "[FindFSDK]: FSDK_RUNTIME_LIBRARIES: ${FSDK_RUNTIME_LIBRARIES}")

# Support the REQUIRED and QUIET arguments, and set FSDK_FOUND if found.
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(FSDK DEFAULT_MSG 
	FSDK_INCLUDE_DIRS
	FSDK_LIBRARIES
	FSDK_RUNTIME_LIBRARIES
)

# Don't show in GUI
mark_as_advanced(
	FSDK_INCLUDE_DIRS
	FSDK_LIBRARIES
	FSDK_COMPILER_NAME
	FSDK_TARGET_NAME
	FSDK_LIB_PREFIX
)
