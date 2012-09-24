cmake_minimum_required(VERSION 2.6)
project(MRSWATSON)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
include_directories(${CMAKE_SOURCE_DIR}/source ${CMAKE_SOURCE_DIR}/vendor/local/include ${CMAKE_SOURCE_DIR}/vendor/vstsdk2.4/pluginterfaces/vst2.x)

#add_subdirectory(vendor)
#if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
#set(libaudiofile_CONFIGURE_FLAGS "<SOURCE_DIR>/configure --prefix=<INSTALL_DIR> CFLAGS=\"-m32\" CXXFLAGS=\"-m32\" LDFLAGS=\"-m32\" --enable-static")
#elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
#set(libaudiofile_CONFIGURE_FLAGS <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> CFLAGS=\"-arch i386\" CXXFLAGS=\"-arch i386\" LDFLAGS=\"-arch i386\" --enable-static)
#endif()
#include(ExternalProject)
#ExternalProject_Add(libaudiofile
#SOURCE_DIR ${CMAKE_SOURCE_DIR}/vendor/audiofile
#CONFIGURE_COMMAND ${libaudiofile_CONFIGURE_FLAGS}
#BUILD_COMMAND ${MAKE}
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
add_subdirectory(vendor)

