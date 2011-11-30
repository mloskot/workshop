set(MY_PYTHON_LIBRARIES ${PYTHON_LIBRARIES})
if (PYTHON_DEBUG_LIBRARIES)
  set(MY_PYTHON_LIBRARIES ${PYTHON_DEBUG_LIBRARIES})
endif()

add_executable(emb emb.cpp)
target_link_libraries(emb ${MY_PYTHON_LIBRARIES})