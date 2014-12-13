cmake_minimum_required(VERSION 2.6)
project(MrsWatsonMain)

set(mrswatsonmain_SOURCES MrsWatsonMain.c)
set(mrswatsonmain_LIBS mrswatsoncore)
set(mrswatsonmain_64_LIBS mrswatsoncore64)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(mrswatsonmain_LIBS ${mrswatsonmain_LIBS} dl)
  set(mrswatsonmain_64_LIBS ${mrswatsonmain_64_LIBS} dl)
endif()

if(WITH_AUDIOFILE)
  set(mrswatsonmain_LIBS ${mrswatsonmain_LIBS} audiofile)
  set(mrswatsonmain_64_LIBS ${mrswatsonmain_64_LIBS} audiofile64)

  if(WITH_FLAC)
    # TODO: 32/64 bit FLAC libraries
  endif()
endif()

# Main executable target and associated libraries
add_executable(mrswatson ${mrswatsonmain_SOURCES})
target_link_libraries(mrswatson ${mrswatsonmain_LIBS})
add_executable(mrswatson64 ${mrswatsonmain_SOURCES})
target_link_libraries(mrswatson64 ${mrswatsonmain_64_LIBS})

# Extra compiler/linker flags needed per-platform
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set_target_properties(mrswatson PROPERTIES COMPILE_FLAGS "-m32")
  set_target_properties(mrswatson PROPERTIES LINK_FLAGS "-m32")
  set_target_properties(mrswatson64 PROPERTIES COMPILE_FLAGS "-m64")
  set_target_properties(mrswatson64 PROPERTIES LINK_FLAGS "-m64")
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
