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

set(MY_PYTHON_LIBRARIES ${PYTHON_LIBRARIES})
if (PYTHON_DEBUG_LIBRARIES)
  set(MY_PYTHON_LIBRARIES ${PYTHON_DEBUG_LIBRARIES})
endif()

add_executable(simple simple.cpp)
target_link_libraries(simple ${MY_PYTHON_LIBRARIES})

add_executable(tkinter tkinter.cpp)
target_link_libraries(tkinter ${MY_PYTHON_LIBRARIES})

add_executable(faq_incomplete_input faq_incomplete_input.cpp)
target_link_libraries(faq_incomplete_input ${MY_PYTHON_LIBRARIES})

add_subdirectory(emb)