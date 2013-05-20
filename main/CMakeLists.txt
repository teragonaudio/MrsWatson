cmake_minimum_required(VERSION 2.6)
project(mrswatsonmain)

link_directories(${CMAKE_SOURCE_DIR}/source)

file(GLOB mrswatsonmain_SOURCES *.c)
add_executable(mrswatson ${mrswatsonmain_SOURCES})

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Linux also requires this library for dlopen
  target_link_libraries(mrswatson mrswatsoncore dl)
else()
  target_link_libraries(mrswatson mrswatsoncore)
endif()

