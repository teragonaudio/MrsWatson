cmake_minimum_required(VERSION 2.6)
project(mrswatsoncore)

file(GLOB mrswatsoncore_SOURCES *.c */*.c */*.cpp)

add_library(mrswatsoncore32 ${mrswatsoncore_SOURCES})
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set_target_properties(mrswatsoncore32 PROPERTIES COMPILE_FLAGS "-m32")
  set_target_properties(mrswatsoncore32 PROPERTIES LINK_FLAGS "-m32")
elseif(APPLE)
  set_target_properties(mrswatsoncore32 PROPERTIES COMPILE_FLAGS "-arch i386")
  set_target_properties(mrswatsoncore32 PROPERTIES LINK_FLAGS "-arch i386")
elseif(MSVC)
  # TODO: Not tested
  set_target_properties(mrswatsoncore32 PROPERTIES COMPILE_FLAGS "/D WIN32=1")
endif()

add_library(mrswatsoncore64 ${mrswatsoncore_SOURCES})
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set_target_properties(mrswatsoncore64 PROPERTIES COMPILE_FLAGS "-m64")
  set_target_properties(mrswatsoncore64 PROPERTIES LINK_FLAGS "-m64")
elseif(APPLE)
  set_target_properties(mrswatsoncore64 PROPERTIES COMPILE_FLAGS "-arch x86_64")
  set_target_properties(mrswatsoncore64 PROPERTIES LINK_FLAGS "-arch x86_64")
elseif(MSVC)
  # TODO: Not tested
  set_target_properties(mrswatsoncore64 PROPERTIES COMPILE_FLAGS "/MACHINE:X64 /env x64 /D WIN64=1")
endif()

