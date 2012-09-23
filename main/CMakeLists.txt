include_directories(${MRSWATSON_SOURCE_DIR}/source)
link_directories(${MRSWATSON_BINARY_DIR}/source)

file(GLOB MrsWatson_SOURCES *.c)
add_executable(mrswatson ${MrsWatson_SOURCES})

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  target_link_libraries(mrswatson MrsWatson dl)
else()
  target_link_libraries(mrswatson MrsWatson)
endif()

