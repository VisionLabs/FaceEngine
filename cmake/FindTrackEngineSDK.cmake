# Find the Vision Labs TrackEngine SDK.
# Sets the usual variables expected for find_package scripts:
# TSDK_INCLUDE_DIRS - headers location
# TSDK_LIBRARIES - libraries to link against
# TSDK_FOUND - true if TrackEngine SDK was found.

# This is the directory where the TrackEngine SDK is located.
# By default TSDKDIR environment variable value is taken.

set(TSDK_ROOT "$ENV{TSDKDIR}" CACHE PATH "Vision Labs TrackEngine SDK root directory.")

# Look for headers.
find_path(TSDK_INCLUDE_DIRS
          NAMES trackEngine/ITrackEngine.h trackEngine/ITrackCallbacks trackEngine/IStream.h trackEngine/TrackEngineDefs.h trackEngine/TrackEngineTypes.h
          HINTS $ENV{TSDKDIR}
          PATHS ${TSDK_ROOT}
          PATH_SUFFIXES include)

#message(STATUS "TSDK: TSDK_INCLUDE_DIRS = ${TSDK_INCLUDE_DIRS}.")

# Determine compiler version and architecture.
# ios has no architechture/compiler branching, because only can only use clang
# and if you need multiple architechtures there still compiled into single universal binary
if(MSVC10)
	set(TSDK_COMPILER_NAME vs2010)
elseif(MSVC11)
	set(TSDK_COMPILER_NAME vs2012)
elseif(MSVC12)
	set(TSDK_COMPILER_NAME vs2013)
elseif(MSVC14)
	set(TSDK_COMPILER_NAME vs2015)
elseif(CMAKE_COMPILER_IS_GNUCXX)
	set(TSDK_COMPILER_NAME gcc4)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(TSDK_COMPILER_NAME clang)
elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
	set(TSDK_COMPILER_NAME clang)
else()
	message(SEND_ERROR "Unsupported compiler: ${TSDK_COMPILER_NAME}")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(TSDK_TARGET_NAME x64)
else()
	set(TSDK_TARGET_NAME x86)
endif()

# What version of TrackEngine SDK to use.
if(ANDROID)
	set(TSDK_LIB_PREFIX ${TSDK_COMPILER_NAME}/${ANDROID_ABI})
else()
	set(TSDK_LIB_PREFIX ${TSDK_COMPILER_NAME}/${TSDK_TARGET_NAME})
endif()

# List of all SDK libraries.
set(TSDK_LIB_NAMES
		TrackEngineSDK)

set(TSDK_LIB_PATH_SUFFIX lib/${TSDK_LIB_PREFIX})
set(TSDK_BIN_PATH_SUFFIX bin/${TSDK_LIB_PREFIX})

set(TSDK_LIB)
foreach(LIB ${TSDK_LIB_NAMES})
	set(LIB_PATH ${LIB}-NOTFOUND)
	find_library(LIB_PATH
		NAMES ${LIB}
		HINTS $ENV{TSDKDIR}
		PATHS ${TSDK_ROOT}
		PATH_SUFFIXES ${TSDK_LIB_PATH_SUFFIX}
			${TSDK_BIN_PATH_SUFFIX}
		NO_DEFAULT_PATH)
	list(APPEND TSDK_LIB ${LIB_PATH})
endforeach()

# Find debug libraries (may be missing in your distribution).
set(TSDK_LIBD)
foreach(LIB ${TSDK_LIB_NAMES})
	set(LIB_PATH ${LIB}-NOTFOUND)
	find_library(LIB_PATH
		NAMES ${LIB}d
		HINTS $ENV{TSDKDIR}
		PATHS ${TSDK_ROOT}
		PATH_SUFFIXES	${TSDK_LIB_PATH_SUFFIX}
			${TSDK_BIN_PATH_SUFFIX}
		NO_DEFAULT_PATH)

	list(APPEND TSDK_LIBD ${LIB_PATH})
endforeach()

# Support the REQUIRED and QUIET arguments, and set TSDK_FOUND if found.
include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(TSDK DEFAULT_MSG 
                                  TSDK_INCLUDE_DIRS)

set(TSDK_LIBRARIES)

if(TSDK_FOUND)
	if(TSDK_LIB)
		foreach(LIB ${TSDK_LIB})
			list(APPEND TSDK_LIBRARIES optimized ${LIB})
		endforeach()
		message(STATUS "TSDK [INFO]: Release libraries are available.")
	elseif(TSDK_LIBD)
		message(STATUS "TSDK [WARN]: Release libraries are NOT available.")
	else()
		message(FATAL_ERROR "TSDK [ERROR]: TrackEngine libraries are NOT available.")
	endif()

	if(TSDK_LIBD)
		foreach(LIB ${TSDK_LIBD})
			list(APPEND TSDK_LIBRARIES debug ${LIB})
		endforeach()
		message(STATUS "TSDK [INFO]: Debug libraries are available.")
	elseif(TSDK_LIB)
		message(STATUS "TSDK [WARN]: Debug libraries are NOT available.")
	else()
		message(FATAL_ERROR "TSDK [ERROR]: TrackEngine libraries are NOT available.")	
	endif()

	message(STATUS "TSDK [INFO]: Found TDK in ${TSDK_ROOT}.")
else()
	message(STATUS "TSDK [WARN]: TrackEngineSDK was NOT found.")
endif(TSDK_FOUND)

#message(STATUS "TSDK [DEBUG]: TSDK_LIBRARIES = ${TSDK_LIBRARIES}.")

# Don't show in GUI
mark_as_advanced(
	TSDK_INCLUDE_DIRS
	TSDK_LIBRARIES
	TSDK_COMPILER_NAME
	TSDK_TARGET_NAME
	TSDK_LIB_PREFIX
	TSDK_LIB_NAMES
	TSDK_LIB_NAMESD
	TSDK_LIB 
	TSDK_LIBD
	LIB_PATH
)
