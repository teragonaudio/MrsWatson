cmake_minimum_required(VERSION 2.6)
project(MrsWatsonMain)

set(mrswatsonmain_SOURCES MrsWatsonMain.c)

add_executable(mrswatson ${mrswatsonmain_SOURCES})
target_link_libraries(mrswatson
  mrswatsoncore
  audiofile
)

add_executable(mrswatson64 ${mrswatsonmain_SOURCES})
target_link_libraries(mrswatson64
  mrswatsoncore64
  audiofile64
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set_target_properties(mrswatson PROPERTIES COMPILE_FLAGS "-m32")
  set_target_properties(mrswatson PROPERTIES LINK_FLAGS "-m32")
  target_link_libraries(mrswatson dl)

  set_target_properties(mrswatson64 PROPERTIES COMPILE_FLAGS "-m64")
  set_target_properties(mrswatson64 PROPERTIES LINK_FLAGS "-m64")
  target_link_libraries(mrswatson64 dl)
elseif(APPLE)
  set_target_properties(mrswatson PROPERTIES OSX_ARCHITECTURES "i386")
  set_target_properties(mrswatson64 PROPERTIES OSX_ARCHITECTURES "x86_64")
elseif(MSVC)
  # On Unix, we can build both the 32/64 bit versions at once. However with
  # Visual Studio we need to generate two separate out-of-source build dirs,
  # one for each architecture.
  # TODO: There must be some way to fix this, Juce manages to do it...
  if(${PLATFORM_BITS} EQUAL 32)
    set_target_properties(mrswatson PROPERTIES COMPILE_FLAGS "/D WIN32=1")
  elseif(${PLATFORM_BITS} EQUAL 64)
    set_target_properties(mrswatson64 PROPERTIES COMPILE_FLAGS "/MACHINE:X64 /D WIN64=1")
  endif()
endif()

