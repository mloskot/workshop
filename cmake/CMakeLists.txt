cmake_minimum_required(VERSION 2.8)
project(workshop-cmake)

configure_file(
	${CMAKE_SOURCE_DIR}/cmake_build_type.h.in
	${CMAKE_BINARY_DIR}/cmake_build_type.h
	@ONLY)

include_directories(${CMAKE_BINARY_DIR})

add_executable(cmake_ndebug_test cmake_ndebug_test.cpp)
