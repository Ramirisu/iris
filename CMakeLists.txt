cmake_minimum_required(VERSION 3.16)

project(iris VERSION 0.0.0 LANGUAGES CXX)

option(IRIS_ENABLE_TESTING "Build unit tests" ON)

if (NOT DEFINED CMAKE_CXX_STANDARD)
	set(CMAKE_CXX_STANDARD "17")
	set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

file(GLOB_RECURSE IRIS_HEADER_FILES "include/*.hpp")
if (WIN32)
	file(GLOB_RECURSE IRIS_SOURCE_FILES "source/*.win.cpp")
else()
	file(GLOB_RECURSE IRIS_SOURCE_FILES "source/*.unix.cpp")
endif()
add_library(iris STATIC ${IRIS_SOURCE_FILES} ${IRIS_HEADER_FILES})
target_include_directories(iris PUBLIC include)

if (IRIS_ENABLE_TESTING)
	enable_testing()
	add_subdirectory(test)
endif()
