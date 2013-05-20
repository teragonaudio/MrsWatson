cmake_minimum_required(VERSION 2.6)
project(mrswatsontest)

include_directories(${CMAKE_SOURCE_DIR}/source)
link_directories(${CMAKE_SOURCE_DIR}/source)

file(GLOB mrswatsontest_SOURCES *.c */*.c)
add_executable(mrswatsontest ${mrswatsontest_SOURCES})

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  target_link_libraries(mrswatsontest mrswatsoncore dl)
else()
  target_link_libraries(mrswatsontest mrswatsoncore)
endif()

