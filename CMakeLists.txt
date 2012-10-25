cmake_minimum_required(VERSION 2.6)
project(MrsWatson)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(localDir ${CMAKE_SOURCE_DIR}/local)
include_directories(${CMAKE_SOURCE_DIR}/source ${localDir}/include ${CMAKE_SOURCE_DIR}/vendor/vstsdk2.4/pluginterfaces/vst2.x)

# Third-party dependencies ###################################################
include(ExternalProject)
set(local_INCLUDE ${localDir}/include)
set(local_LIB ${localDir}/lib)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(ARCH_FLAGS "-m32")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(ARCH_FLAGS "-arch i386")
endif()

set(generic_CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> CFLAGS=${ARCH_FLAGS} CXXFLAGS=${ARCH_FLAGS} LDFLAGS=${ARCH_FLAGS} --enable-static --enable-shared=no)

# zlib #######################################################################

set(libz_CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=${localDir} --archs=${ARCH_FLAGS} --static)

ExternalProject_Add(libz
  URL http://zlib.net/zlib-1.2.7.tar.gz
  URL_MD5 60df6a37c56e7c1366cca812414f7b85
  PREFIX ${CMAKE_SOURCE_DIR}/build/zlib
  CONFIGURE_COMMAND ${libz_CONFIGURE_COMMAND}
  INSTALL_DIR ${localDir}
  BUILD_COMMAND ${MAKE}
  BUILD_IN_SOURCE 1
)

# libiconv ####################################################################

ExternalProject_Add(libiconv
  URL http://ftp.gnu.org/pub/gnu/libiconv/libiconv-1.14.tar.gz
  URL_MD5 e34509b1623cec449dfeb73d7ce9c6c6
  PREFIX ${CMAKE_SOURCE_DIR}/build/libiconv
  CONFIGURE_COMMAND ${generic_CONFIGURE_COMMAND}
  INSTALL_DIR ${localDir}
  BUILD_COMMAND ${MAKE}
  BUILD_IN_SOURCE 1
)


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

# libarchive ##################################################################

set(libarchive_CFLAGS "${ARCH_FLAGS} -I${local_INCLUDE}")
set(libarchive_LDFLAGS "${ARCH_FLAGS} -L${local_LIB}")
set(libarchive_CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix=<INSTALL_DIR> CFLAGS=${libarchive_CFLAGS} CXXFLAGS=${libarchive_CFLAGS} LDFLAGS=${libarchive_LDFLAGS} --enable-static --enable-shared=no --without-lzmadec --without-lzma)

ExternalProject_Add(libarchive
  DEPENDS libz libiconv
  URL https://github.com/downloads/libarchive/libarchive/libarchive-3.0.4.tar.gz
  URL_MD5 af443ca9a10ddbcbf00f7ae34ca7fc16
  PREFIX ${CMAKE_SOURCE_DIR}/build/libarchive
  CONFIGURE_COMMAND ${libarchive_CONFIGURE_COMMAND}
  INSTALL_DIR ${localDir}
  BUILD_COMMAND ${MAKE}
  BUILD_IN_SOURCE 1
)

# MrsWatson ###################################################################

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  add_definitions(-DLINUX=1)
  set(CMAKE_C_FLAGS "-fmessage-length=0 -m32 -pipe -Wno-trigraphs -std=c99 -O3 -Wmissing-field-initializers -Wall -Wreturn-type -Wunused-variable -pedantic -Wshadow -Wsign-compare -D__cdecl=\"\"")
  set(CMAKE_CXX_FLAGS "-fmessage-length=0 -m32 -pipe -Wno-trigraphs -O3 -Wmissing-field-initializers -Wall -Wreturn-type -Wunused-variable -pedantic -Wshadow -Wsign-compare -D__cdecl=\"\"")
  set(CMAKE_EXE_LINKER_FLAGS "-m32")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  add_definitions(-DMACOSX=1)
  set(CMAKE_C_FLAGS "-arch i386 -fmessage-length=0 -pipe -Wno-trigraphs -fpascal-strings -std=c99 -Wmissing-field-initializers -Wall -Wmissing-prototypes -Wreturn-type -Wunused-variable -pedantic -Wshadow -Wsign-compare -Wnewline-eof -Wshorten-64-to-32 -fasm-blocks -mmacosx-version-min=10.4")
  set(CMAKE_CXX_FLAGS "-arch i386 -fmessage-length=0 -pipe -Wno-trigraphs -fpascal-strings -Wmissing-field-initializers -Wall -Wmissing-prototypes -Wreturn-type -Wunused-variable -pedantic -Wshadow -Wsign-compare -Wnewline-eof -Wshorten-64-to-32 -fasm-blocks -mmacosx-version-min=10.4")
  set(CMAKE_EXE_LINKER_FLAGS "-arch i386 -framework Carbon -framework CoreFoundation")
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

