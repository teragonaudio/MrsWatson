cmake_minimum_required(VERSION 2.6)
project(MRSWATSON)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
include_directories(${CMAKE_SOURCE_DIR}/source ${CMAKE_SOURCE_DIR}/vendor/local/include ${CMAKE_SOURCE_DIR}/vendor/vstsdk2.4/pluginterfaces/vst2.x)

# Third-party dependencies
add_subdirectory(vendor)
include(ExternalProject)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(ARCH_FLAGS "-m32")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(ARCH_FLAGS "-arch i386")
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(libz_CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/vendor/zlib/configure --prefix=${CMAKE_SOURCE_DIR}/vendor/local --archs=${ARCH_FLAGS})
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(libz_CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/vendor/zlib/configure --prefix=${CMAKE_SOURCE_DIR}/vendor/local --archs=${ARCH_FLAGS})
endif()

ExternalProject_Add(libz
  SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendor/zlib
  INSTALL_DIR ${CMAKE_SOURCE_DIR}/vendor/local
  CONFIGURE_COMMAND ${libz_CONFIGURE_COMMAND}
  PREFIX ${CMAKE_SOURCE_DIR}/build/zlib
  TMP_DIR ${CMAKE_SOURCE_DIR}/build/zlib/tmp
  BUILD_COMMAND ${MAKE}
  BUILD_IN_SOURCE 1
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(libaudiofile_CONFIGURE_FLAGS "<SOURCE_DIR>/configure --prefix=<INSTALL_DIR> CFLAGS=\"-m32\" CXXFLAGS=\"-m32\" LDFLAGS=\"-m32\" --enable-static")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(libaudiofile_CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> CFLAGS=${ARCH_FLAGS} CXXFLAGS=${ARCH_FLAGS} LDFLAGS=${ARCH_FLAGS} --enable-static)
endif()

# libaudiofile doesn't make cleanly, so this must be done by hand
#ExternalProject_Add(libaudiofile
#  SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendor/audiofile
#  INSTALL_DIR ${CMAKE_SOURCE_DIR}/vendor/local
#  CONFIGURE_COMMAND ${libaudiofile_CONFIGURE_COMMAND}
#  PREFIX ${CMAKE_SOURCE_DIR}/build/audiofile
#  TMP_DIR ${CMAKE_SOURCE_DIR}/build/audiofile/tmp
#  BUILD_COMMAND ${MAKE}
#  BUILD_IN_SOURCE 1
#)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(CMAKE_C_FLAGS "-fmessage-length=0 -m32 -pipe -Wno-trigraphs -std=c99 -O3 -Wmissing-field-initializers -Wall -Wreturn-type -Wunused-variable -pedantic -Wshadow -Wsign-compare -D__cdecl=\"\"")
  set(CMAKE_CXX_FLAGS "-fmessage-length=0 -m32 -pipe -Wno-trigraphs -O3 -Wmissing-field-initializers -Wall -Wreturn-type -Wunused-variable -pedantic -Wshadow -Wsign-compare -D__cdecl=\"\"")
  set(CMAKE_EXE_LINKER_FLAGS "-m32")
  add_definitions(-DLINUX=1)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_C_FLAGS "-arch i386 -fmessage-length=0 -pipe -Wno-trigraphs -fpascal-strings -std=c99 -O3 -Wmissing-field-initializers -Wall -Wmissing-prototypes -Wreturn-type -Wunused-variable -pedantic -Wshadow -Wsign-compare -Wnewline-eof -Wshorten-64-to-32 -fasm-blocks -mmacosx-version-min=10.4")
  set(CMAKE_CXX_FLAGS "-arch i386 -fmessage-length=0 -pipe -Wno-trigraphs -fpascal-strings -O3 -Wmissing-field-initializers -Wall -Wmissing-prototypes -Wreturn-type -Wunused-variable -pedantic -Wshadow -Wsign-compare -Wnewline-eof -Wshorten-64-to-32 -fasm-blocks -mmacosx-version-min=10.4")
  set(CMAKE_EXE_LINKER_FLAGS "-arch i386 -framework Carbon -framework CoreFoundation")
  add_definitions(-DMACOSX=1)
endif()

add_subdirectory(source)
add_subdirectory(main)
add_subdirectory(test)

