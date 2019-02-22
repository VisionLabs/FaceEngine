
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

find_package(LivenessEngineSDK REQUIRED)
include_directories(${LSDK_INCLUDE_DIRS})

message(${FSDK_LIBRARIES})
message(${FSDK_INCLUDE_DIRS})

add_subdirectory(pybind11)

file(GLOB SOURCES src/*.cpp)

message("${SOURCES}")

pybind11_add_module(FaceEngine ${SOURCES})
target_link_libraries(FaceEngine PRIVATE pybind11::module ${FSDK_LIBRARIES} ${LSDK_LIBRARIES})

message(${CMAKE_INSTALL_PREFIX})

install(TARGETS FaceEngine
	LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT libs)

