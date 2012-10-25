link_directories(${MRSWATSON_BINARY_DIR}/source ${MRSWATSON_BINARY_DIR}/local/lib)

file(GLOB MrsWatson_SOURCES *.c)
add_executable(mrswatson ${MrsWatson_SOURCES})
add_dependencies(mrswatson libarchive)

set(base_libraries MrsWatson archive z iconv)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Linux also requires this library for dlopen
  target_link_libraries(mrswatson ${base_libraries} dl)
else()
  target_link_libraries(mrswatson ${base_libraries})
endif()

file(COPY mrswatson DESTINATION ${MRSWATSON_BINARY_DIR}/local/bin)

