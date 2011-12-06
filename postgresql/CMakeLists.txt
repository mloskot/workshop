cmake_minimum_required(VERSION 2.8)
project(workshop-postgresql)

find_package(PostgreSQL)
if (PostgreSQL_FOUND)
  include_directories(${PostgreSQL_INCLUDE_DIRS})
  link_directories(${PostgreSQL_LIBRARY_DIRS})
else()
	message(FATAL_ERROR "PostgreSQL not found but required")
endif()

set(COMMON_LIBRARIES)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
  add_definitions(-D_CRT_NONSTDC_NO_DEPRECATE)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
  add_definitions(-D_SCL_SECURE_NO_DEPRECATE)
  
  find_library(WSOCK32_LIBRARY wsock32)
  find_library(WS2_32_LIBRARY ws2_32) 
  if (WSOCK32_LIBRARY AND WS2_32_LIBRARY)
    set(COMMON_LIBRARIES ${COMMON_LIBRARIES} ${WSOCK32_LIBRARY} ${WS2_32_LIBRARY})
  endif()
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(COMMON_HEADERS
  ${CMAKE_CURRENT_SOURCE_DIR}/tinypq.hpp)

add_subdirectory(numeric)
