include_directories(${MRSWATSON_SOURCE_DIR}/source)
link_directories(${MRSWATSON_BINARY_DIR}/source)

add_executable(mrswatson main.c)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  target_link_libraries(mrswatson MrsWatson dl)
else()
  target_link_libraries(mrswatson MrsWatson)
endif()

