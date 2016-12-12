cmake_minimum_required(VERSION 2.8)
project(virus)

SET(CMAKE_CXX_COMPILER             "g++")
SET(CMAKE_CXX_FLAGS                "-Wall -std=c++14")
SET(CMAKE_CXX_FLAGS_DEBUG          "-g")
SET(CMAKE_CXX_FLAGS_RELEASE        "-O2 -DNDEBUG")

set(HEADER_FILES ./virus_genealogy.h)
include_directories(.)

add_executable(example_test tests/example_test.cpp ${HEADER_FILES})

enable_testing()
add_test(example example_test)
