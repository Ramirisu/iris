cmake_minimum_required(VERSION 3.16)

project(
  iris
  VERSION 0.0.0
  LANGUAGES CXX)

option(IRIS_BUILD_EXAMPLE "Build examples" OFF)
option(IRIS_BUILD_TESTING "Build unit tests" OFF)

if(NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD "20")
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(IRIS_COMPILE_FLAGS /W4)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(IRIS_COMPILE_FLAGS -Wall -Wextra -Wno-unused-local-typedefs)
endif()

file(GLOB_RECURSE IRIS_HEADER_FILES "include/*.hpp")
if(WIN32)
  file(GLOB_RECURSE IRIS_SOURCE_FILES "source/*_win.cpp")
else()
  file(GLOB_RECURSE IRIS_SOURCE_FILES "source/*_unix.cpp")
endif()
add_library(iris STATIC ${IRIS_SOURCE_FILES} ${IRIS_HEADER_FILES})
target_compile_options(iris PRIVATE ${IRIS_COMPILE_FLAGS})
target_include_directories(iris PUBLIC include)

if(IRIS_BUILD_EXAMPLE)
  add_subdirectory(example)
endif()

if(IRIS_BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()
