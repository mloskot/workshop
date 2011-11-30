cmake_minimum_required(VERSION 2.8)
project(workshop-python)

find_package(PythonLibs)
if (PYTHONLIBS_FOUND)
  include_directories(${PYTHON_INCLUDE_DIRS})
  
  if (PYTHON_DEBUG_LIBRARIES)
    message(STATUS "Found Python Debug libraries: ${PYTHON_DEBUG_LIBRARIES}")
  endif()
else()
	message(FATAL_ERROR "Python not found but required")
endif()

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
  add_definitions(-D_SCL_SECURE_NO_DEPRECATE)
endif()

add_subdirectory(emb)
