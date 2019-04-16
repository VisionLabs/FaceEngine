
SET(CMAKE_INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN/")

cmake_minimum_required(VERSION 3.5)
project(FaceEnginePythonBinding)

set(CMAKE_MODULE_PATH
	${CMAKE_MODULE_PATH}
	"${CMAKE_SOURCE_DIR}/cmake/"
	)
SET(CMAKE_INSTALL_RPATH "$ORIGIN/../lib:$ORIGIN/")

if (MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -DNOMINMAX)
else()
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

find_package(FaceEngineSDK REQUIRED)
include_directories(${FSDK_INCLUDE_DIRS})

if (FSDK_FOUND)
	if (NOT DEFINED LSDK_ROOT)
		# We can't just use FSDK_ROOT - it could be relative.
		set(LSDK_ROOT "${FSDK_INCLUDE_DIRS}/../." CACHE PATH "LSDK directory")
	endif()
endif()

find_package(LivenessEngineSDK REQUIRED)
include_directories(${LSDK_INCLUDE_DIRS})

if (FSDK_FOUND)
	if (NOT DEFINED TSDK_ROOT)
		# We can't just use FSDK_ROOT - it could be relative.
		set(TSDK_ROOT "${FSDK_INCLUDE_DIRS}/../." CACHE PATH "TSDK directory")
	endif()
endif()

find_package(TrackEngineSDK REQUIRED)
include_directories(${TSDK_INCLUDE_DIRS})

message("FSDK_LIBRARIES = ${FSDK_LIBRARIES}")
message("FSDK_INCLUDE_DIRS = ${FSDK_INCLUDE_DIRS}")


message("LSDK_LIBRARIES = ${LSDK_LIBRARIES}")
message("LSDK_INCLUDE_DIRS = ${LSDK_INCLUDE_DIRS}")

add_subdirectory(pybind11)

file(GLOB SOURCES src/*.cpp)

message("FaceEngine sources: ${SOURCES}")

pybind11_add_module(FaceEngine ${SOURCES})
target_link_libraries(FaceEngine PRIVATE pybind11::module ${FSDK_LIBRARIES} ${LSDK_LIBRARIES})

file(GLOB SOURCES src/TrackEngine/*.cpp)

message("TrackEngine sources: ${SOURCES}")

pybind11_add_module(TrackEngine ${SOURCES})
target_link_libraries(TrackEngine PRIVATE pybind11::module ${FSDK_LIBRARIES} ${LSDK_LIBRARIES} ${TSDK_LIBRARIES})

message(${CMAKE_INSTALL_PREFIX})

install(TARGETS FaceEngine
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT libs)

install(TARGETS TrackEngine
		LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT libs)

