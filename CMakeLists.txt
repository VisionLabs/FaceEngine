cmake_minimum_required(VERSION 3.5)
project(FaceEngine)

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

set(FSDK_ROOT /home/mar/Desktop/luna-sdk)

find_package(FaceEngineSDK REQUIRED)
include_directories(${FSDK_INCLUDE_DIRS})

message(${FSDK_LIBRARIES})
message(${FSDK_INCLUDE_DIRS})
#set(SOURCES src/main.cpp)
#add_executable(fe_example ${SOURCES})
#target_link_libraries(fe_example PRIVATE ${FSDK_LIBRARIES})

add_subdirectory(pybind11)


#set_target_properties(${FSDK_LIBRARIES}  PROPERTIES POSITION_INDEPENDENT_CODE TRUE)
pybind11_add_module(fe src/main.cpp)
target_link_libraries(fe PRIVATE pybind11::module ${FSDK_LIBRARIES})


#TARGET_LINK_LIBRARIES(cmake_example )
