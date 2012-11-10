include_directories(${MRSWATSON_SOURCE_DIR}/source)
link_directories(${MRSWATSON_BINARY_DIR}/source)

file(GLOB MrsWatsonTest_SOURCES *.c */*.c)
add_executable(mrswatsontest ${MrsWatsonTest_SOURCES})
add_dependencies(mrswatsontest AudioTestData)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  target_link_libraries(mrswatsontest MrsWatson dl)
else()
  target_link_libraries(mrswatsontest MrsWatson)
endif()

