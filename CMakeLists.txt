cmake_minimum_required(VERSION 2.6)
project(MrsWatson)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
include_directories(${CMAKE_SOURCE_DIR}/source ${localDir}/include ${CMAKE_SOURCE_DIR}/vendor/vstsdk2.4/pluginterfaces/vst2.x)

# Platform properties ##########################################

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(platform_bits 64)
else()
  set(platform_bits 32)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  if(${platform_bits} EQUAL 32)
    set(platform_name "Linux-i686")
    set(arch_flags "-m32")
  else()
    set(platform_name "Linux-x86_64")
    set(arch_flags "-m64")
  endif()
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(platform_name "Mac OS X")
  set(arch_flags "-arch i386")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  if(${platform_bits} EQUAL 32)
    set(platform_name "Windows 32-bit")
  else()
    set(platform_name "Windows 64-bit")
  endif()
else()
  set(platform_name "Unknown")
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
else()
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${platform_name})
endif()

# Third-party dependencies #####################################
include(ExternalProject)

set(generic_CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${CMAKE_SOURCE_DIR}/build CFLAGS=${arch_flags} CXXFLAGS=${arch_flags} LDFLAGS=${arch_flags} --enable-static)

# libaudiofile ################################################################
# libaudiofile doesn't make cleanly, so this must be done by hand

#ExternalProject_Add(libaudiofile
#  SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendor/audiofile
#  INSTALL_DIR ${localDir}
#  CONFIGURE_COMMAND ${libaudiofile_CONFIGURE_COMMAND}
#  PREFIX ${CMAKE_SOURCE_DIR}/build/audiofile
#  TMP_DIR ${CMAKE_SOURCE_DIR}/build/audiofile/tmp
#  BUILD_COMMAND ${MAKE}
#  BUILD_IN_SOURCE 1
#)

# AudioTestData ###############################################################

#ExternalProject_Add(AudioTestData
#  SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendor/AudioTestData
#  PREFIX ${CMAKE_SOURCE_DIR}/vendor/AudioTestData/build
#  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/vendor/AudioTestData/build
#  INSTALL_DIR ${localDir}
#)

# MrsWatson ###################################################################

set(common_gcc_flags "-fmessage-length=0 -pipe -Wno-trigraphs -Wno-trigraphs -Wmissing-field-initializers -Wall -Wreturn-type -Wunused-variable -Wshadow -Wsign-compare")
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_definitions(-DLINUX=1)
  set(CMAKE_C_FLAGS "${arch_flags} ${common_gcc_flags} -D__cdecl=\"\" -D_POSIX_C_SOURCE=200809L")
  set(CMAKE_CXX_FLAGS "${arch_flags} ${common_gcc_flags} -D__cdecl=\"\"")
  set(CMAKE_EXE_LINKER_FLAGS "${arch_flags}")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  add_definitions(-DMACOSX=1)
  set(CMAKE_C_FLAGS "${arch_flags} ${common_gcc_flags} -fpascal-strings -std=c99 -Wmissing-prototypes -Wnewline-eof -Wshorten-64-to-32 -fasm-blocks -mmacosx-version-min=10.4")
  set(CMAKE_CXX_FLAGS "${arch_flags} ${common_gcc_flags} -fpascal-strings -Wnewline-eof -Wshorten-64-to-32 -fasm-blocks -mmacosx-version-min=10.4")
  set(CMAKE_EXE_LINKER_FLAGS "${arch_flags} -framework Carbon -framework CoreFoundation")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(CMAKE_C_FLAGS "")
  set(CMAKE_CXX_FLAGS "")
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
  add_definitions("-O0")
  add_definitions("-g")
  add_definitions("-DDEBUG=1")
else()
  add_definitions("-O3")
endif()

add_subdirectory(source)
add_subdirectory(main)
add_subdirectory(test)

