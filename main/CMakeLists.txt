cmake_minimum_required(VERSION 2.6)
project(mrswatsonmain)

file(GLOB mrswatsonmain_SOURCES *.c)
file(GLOB mrswatsonmain_HEADERS *.h)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_executable(mrswatson ${mrswatsonmain_SOURCES} ${mrswatsonmain_HEADERS})
  set_target_properties(mrswatson PROPERTIES COMPILE_FLAGS "-m32")
  set_target_properties(mrswatson PROPERTIES LINK_FLAGS "-m32")
  target_link_libraries(mrswatson mrswatsoncore dl)

  add_executable(mrswatson64 ${mrswatsonmain_SOURCES} ${mrswatsonmain_HEADERS})
  set_target_properties(mrswatson64 PROPERTIES COMPILE_FLAGS "-m64")
  set_target_properties(mrswatson64 PROPERTIES LINK_FLAGS "-m64")
  target_link_libraries(mrswatson64 mrswatsoncore64 dl)
elseif(APPLE)
  add_executable(mrswatson ${mrswatsonmain_SOURCES} ${mrswatsonmain_HEADERS})
  set_target_properties(mrswatson PROPERTIES OSX_ARCHITECTURES "i386")
  target_link_libraries(mrswatson mrswatsoncore)

  add_executable(mrswatson64 ${mrswatsonmain_SOURCES} ${mrswatsonmain_HEADERS})
  set_target_properties(mrswatson64 PROPERTIES OSX_ARCHITECTURES "x86_64")
  target_link_libraries(mrswatson64 mrswatsoncore64)
elseif(MSVC)
  # On unix, we can build both the 32/64 bit versions at once. However with
  # Visual Studio we need to generate two separate out-of-source build dirs,
  # one for each architecture.
  # TODO: There must be some way to fix this, Juce manages to do it...
  if(${PLATFORM_BITS} EQUAL 32)
    add_executable(mrswatson ${mrswatsonmain_SOURCES} ${mrswatsonmain_HEADERS})
    set_target_properties(mrswatson PROPERTIES COMPILE_FLAGS "/D WIN32=1")
    target_link_libraries(mrswatson mrswatsoncore)
  elseif(${PLATFORM_BITS} EQUAL 64)
    add_executable(mrswatson64 ${mrswatsonmain_SOURCES} ${mrswatsonmain_HEADERS})
    set_target_properties(mrswatson64 PROPERTIES COMPILE_FLAGS "/MACHINE:X64 /D WIN64=1")
    target_link_libraries(mrswatson64 mrswatsoncore64)
  endif()
endif()

