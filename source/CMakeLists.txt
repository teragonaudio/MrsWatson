cmake_minimum_required(VERSION 2.6)
project(mrswatsoncore)

file(GLOB mrswatsoncore_SOURCES *.c */*.c */*.cpp)
file(GLOB mrswatsoncore_HEADERS *.h */*.h)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_library(mrswatsoncore ${mrswatsoncore_SOURCES} ${mrswatsoncore_HEADERS})
  set_target_properties(mrswatsoncore PROPERTIES COMPILE_FLAGS "-m32")
  set_target_properties(mrswatsoncore PROPERTIES LINK_FLAGS "-m32")

  add_library(mrswatsoncore64 ${mrswatsoncore_SOURCES} ${mrswatsoncore_HEADERS})
  set_target_properties(mrswatsoncore64 PROPERTIES COMPILE_FLAGS "-m64")
  set_target_properties(mrswatsoncore64 PROPERTIES LINK_FLAGS "-m64")
elseif(APPLE)
  add_library(mrswatsoncore ${mrswatsoncore_SOURCES} ${mrswatsoncore_HEADERS})
  set_target_properties(mrswatsoncore PROPERTIES OSX_ARCHITECTURES "i386")

  add_library(mrswatsoncore64 ${mrswatsoncore_SOURCES} ${mrswatsoncore_HEADERS})
  set_target_properties(mrswatsoncore64 PROPERTIES OSX_ARCHITECTURES "x86_64")
elseif(MSVC)
  # On unix, we can build both the 32/64 bit versions at once. However with
  # Visual Studio we need to generate two separate out-of-source build dirs,
  # one for each architecture.
  # TODO: There must be some way to fix this, Juce manages to do it...
  if(${PLATFORM_BITS} EQUAL 32)
    add_library(mrswatsoncore ${mrswatsoncore_SOURCES} ${mrswatsoncore_HEADERS})
    set_target_properties(mrswatsoncore PROPERTIES COMPILE_FLAGS "/D WIN32=1")
  elseif(${PLATFORM_BITS} EQUAL 64)
    add_library(mrswatsoncore64 ${mrswatsoncore_SOURCES} ${mrswatsoncore_HEADERS})
    set_target_properties(mrswatsoncore64 PROPERTIES COMPILE_FLAGS "/MACHINE:X64 /D WIN64=1")
  endif()
endif()

