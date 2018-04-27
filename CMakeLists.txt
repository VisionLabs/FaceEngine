cmake_minimum_required(VERSION 3.5)
project(FaceEnginePythonBinding)

set(CMAKE_MODULE_PATH
	${CMAKE_MODULE_PATH}
	"${CMAKE_SOURCE_DIR}/cmake/"
	)

if (MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -DNOMINMAX)
else ()
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif ()

find_package(FaceEngineSDK REQUIRED)
include_directories(${FSDK_INCLUDE_DIRS})

message(${FSDK_LIBRARIES})
message(${FSDK_INCLUDE_DIRS})

add_subdirectory(pybind11)

#set_target_properties(${FSDK_LIBRARIES}  PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
pybind11_add_module(FaceEngine src/FaceEngine.cpp src/FaceEngineAdapter.cpp src/SettingsProviderAdapter.cpp)
target_link_libraries(FaceEngine PRIVATE pybind11::module ${FSDK_LIBRARIES})

# install(TARGETS FaceEngine
#  	LIBRARY DESTINATION ${MAIN_INSTALL_PREFIX}/lib/${OUTPUT_PREFIX} COMPONENT libs)

