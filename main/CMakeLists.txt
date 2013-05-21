cmake_minimum_required(VERSION 2.6)
project(mrswatsonmain)

# TODO: Probably can remove this
include_directories(${CMAKE_SOURCE_DIR}/source)
link_directories(${CMAKE_SOURCE_DIR}/source)
file(GLOB mrswatsonmain_SOURCES *.c)

# On unix, we can build both the 32/64 bit versions at once. However with
# Visual Studio we need to generate two separate out-of-source build dirs,
# one for each architecture.

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_executable(mrswatson ${mrswatsonmain_SOURCES})
  set_target_properties(mrswatson PROPERTIES COMPILE_FLAGS "-m32")
  set_target_properties(mrswatson PROPERTIES LINK_FLAGS "-m32")
  target_link_libraries(mrswatson mrswatsoncore dl)
elseif(APPLE)
  add_executable(mrswatson ${mrswatsonmain_SOURCES})
  set_target_properties(mrswatson PROPERTIES COMPILE_FLAGS "-arch i386")
  set_target_properties(mrswatson PROPERTIES LINK_FLAGS "-arch i386")
  target_link_libraries(mrswatson mrswatsoncore)
elseif(MSVC)
  if(${platform_bits} EQUAL 32)
    add_executable(mrswatson ${mrswatsonmain_SOURCES})
    set_target_properties(mrswatson PROPERTIES COMPILE_FLAGS "/D WIN32=1")
    target_link_libraries(mrswatson mrswatsoncore)
  endif()
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_executable(mrswatson64 ${mrswatsonmain_SOURCES})
  set_target_properties(mrswatson64 PROPERTIES COMPILE_FLAGS "-m64")
  set_target_properties(mrswatson64 PROPERTIES LINK_FLAGS "-m64")
  target_link_libraries(mrswatson64 mrswatsoncore64 dl)
elseif(APPLE)
  add_executable(mrswatson64 ${mrswatsonmain_SOURCES})
  set_target_properties(mrswatson64 PROPERTIES COMPILE_FLAGS "-arch x86_64")
  set_target_properties(mrswatson64 PROPERTIES LINK_FLAGS "-arch x86_64")
  target_link_libraries(mrswatson64 mrswatsoncore64)
elseif(MSVC)
  if(${platform_bits} EQUAL 64)
    add_executable(mrswatson64 ${mrswatsonmain_SOURCES})
    set_target_properties(mrswatson64 PROPERTIES COMPILE_FLAGS "/MACHINE:X64 /D WIN64=1")
    target_link_libraries(mrswatson64 mrswatsoncore64)
  endif()
endif()

