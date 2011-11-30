cmake_minimum_required(VERSION 2.8)
project(workshop-postgresql)

find_package(PostgreSQL)
if (PostgreSQL_FOUND)
  include_directories(${PostgreSQL_INCLUDE_DIRS})
  link_directories(${PostgreSQL_LIBRARY_DIRS})
else()
	message(FATAL_ERROR "PostgreSQL not found but required")
endif()

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
  add_definitions(-D_SCL_SECURE_NO_DEPRECATE)
endif()

add_subdirectory(numeric)
