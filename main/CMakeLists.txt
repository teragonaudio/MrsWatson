link_directories(${MRSWATSON_BINARY_DIR}/source ${MRSWATSON_BINARY_DIR}/local/lib)

file(GLOB MrsWatson_SOURCES *.c)
add_executable(mrswatson ${MrsWatson_SOURCES})

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Linux also requires this library for dlopen
  target_link_libraries(mrswatson MrsWatson dl)
else()
  target_link_libraries(mrswatson MrsWatson)
endif()

