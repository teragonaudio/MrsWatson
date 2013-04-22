cmake_minimum_required(VERSION 2.6)
project(MrsWatson)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
include_directories(${CMAKE_SOURCE_DIR}/source ${localDir}/include ${CMAKE_SOURCE_DIR}/vendor/vstsdk2.4/pluginterfaces/vst2.x)

# Platform properties ##########################################

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(platform_name "Linux")
  set(ARCH_FLAGS "-m32")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(platform_name "Mac OS X")
  set(ARCH_FLAGS "-arch i386")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  if(${platform_bits} EQUAL 32)
    set(platform_name "Windows 32-bit")
  else()
    set(platform_name "Windows 64-bit")
  endif()
else()
  set(platform_name "Unknown")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${platform_name})

# Third-party dependencies #####################################
include(ExternalProject)
set(local_INCLUDE ${localDir}/include)
set(local_LIB ${localDir}/lib)

set(generic_CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> CFLAGS=${ARCH_FLAGS} CXXFLAGS=${ARCH_FLAGS} LDFLAGS=${ARCH_FLAGS} --enable-static --enable-shared=no)

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

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_definitions(-DLINUX=1)
  set(CMAKE_C_FLAGS "-fmessage-length=0 -m32 -pipe -Wno-trigraphs -std=c99 -O3 -Wmissing-field-initializers -Wall -Wreturn-type -Wunused-variable -pedantic -Wshadow -Wsign-compare -D__cdecl=\"\" -D_POSIX_C_SOURCE=200809L")
  set(CMAKE_CXX_FLAGS "-fmessage-length=0 -m32 -pipe -Wno-trigraphs -O3 -Wmissing-field-initializers -Wall -Wreturn-type -Wunused-variable -pedantic -Wshadow -Wsign-compare -D__cdecl=\"\"")
  set(CMAKE_EXE_LINKER_FLAGS "-m32")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  add_definitions(-DMACOSX=1)
  set(CMAKE_C_FLAGS "-arch i386 -fmessage-length=0 -pipe -Wno-trigraphs -fpascal-strings -std=c99 -Wmissing-field-initializers -Wall -Wmissing-prototypes -Wreturn-type -Wunused-variable -pedantic -Wshadow -Wsign-compare -Wnewline-eof -Wshorten-64-to-32 -fasm-blocks -mmacosx-version-min=10.4")
  set(CMAKE_CXX_FLAGS "-arch i386 -fmessage-length=0 -pipe -Wno-trigraphs -fpascal-strings -Wmissing-field-initializers -Wall -Wmissing-prototypes -Wreturn-type -Wunused-variable -pedantic -Wshadow -Wsign-compare -Wnewline-eof -Wshorten-64-to-32 -fasm-blocks -mmacosx-version-min=10.4")
  set(CMAKE_EXE_LINKER_FLAGS "-arch i386 -framework Carbon -framework CoreFoundation")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(CMAKE_C_FLAGS "")
  set(CMAKE_CXX_FLAGS "")
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
  add_definitions("-O0")
  add_definitions("-g")
  add_definitions("-DDEBUG=1")
else()
  add_definitions("-O2")
endif()

add_subdirectory(source)
add_subdirectory(main)
add_subdirectory(test)

