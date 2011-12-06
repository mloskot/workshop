add_executable(postgresql_binary_numeric
  postgresql_binary_numeric.cpp
  ${COMMON_HEADERS})
target_link_libraries(postgresql_binary_numeric
  ${PostgreSQL_LIBRARIES}
  ${COMMON_LIBRARIES})