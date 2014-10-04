cmake_minimum_required(VERSION 2.6)
project(mrswatsontest)

file(GLOB mrswatsontest_SOURCES *.c */*.c)
file(GLOB mrswatsontest_HEADERS *.h */*.h)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_executable(mrswatsontest ${mrswatsontest_SOURCES} ${mrswatsontest_HEADERS})
  set_target_properties(mrswatsontest PROPERTIES COMPILE_FLAGS "-m32")
  set_target_properties(mrswatsontest PROPERTIES LINK_FLAGS "-m32")
  target_link_libraries(mrswatsontest mrswatsoncore dl)

  add_executable(mrswatsontest64 ${mrswatsontest_SOURCES} ${mrswatsontest_HEADERS})
  set_target_properties(mrswatsontest64 PROPERTIES COMPILE_FLAGS "-m64")
  set_target_properties(mrswatsontest64 PROPERTIES LINK_FLAGS "-m64")
  target_link_libraries(mrswatsontest64 mrswatsoncore64 dl)
elseif(APPLE)
  add_executable(mrswatsontest ${mrswatsontest_SOURCES} ${mrswatsontest_HEADERS})
  set_target_properties(mrswatsontest PROPERTIES OSX_ARCHITECTURES "i386")
  target_link_libraries(mrswatsontest mrswatsoncore)

  add_executable(mrswatsontest64 ${mrswatsontest_SOURCES} ${mrswatsontest_HEADERS})
  set_target_properties(mrswatsontest64 PROPERTIES OSX_ARCHITECTURES "x86_64")
  target_link_libraries(mrswatsontest64 mrswatsoncore64)
elseif(MSVC)
  # On unix, we can build both the 32/64 bit versions at once. However with
  # Visual Studio we need to generate two separate out-of-source build dirs,
  # one for each architecture.
  # TODO: There must be some way to fix this, Juce manages to do it...
  if(${PLATFORM_BITS} EQUAL 32)
    add_executable(mrswatsontest ${mrswatsontest_SOURCES} ${mrswatsontest_HEADERS})
    set_target_properties(mrswatsontest PROPERTIES COMPILE_FLAGS "/D WIN32=1")
    target_link_libraries(mrswatsontest mrswatsoncore)
  elseif(${PLATFORM_BITS} EQUAL 64)
    add_executable(mrswatsontest64 ${mrswatsontest_SOURCES} ${mrswatsontest_HEADERS})
    set_target_properties(mrswatsontest64 PROPERTIES COMPILE_FLAGS "/MACHINE:X64 /D WIN64=1")
    target_link_libraries(mrswatsontest64 mrswatsoncore64)
  endif()
endif()

