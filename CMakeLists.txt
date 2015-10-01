cmake_minimum_required(VERSION 2.8.11)
project(MrsWatson)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(cmake_SCRIPTS_DIR ${CMAKE_SOURCE_DIR}/cmake)
include_directories(${CMAKE_SOURCE_DIR}/source)
include_directories(${CMAKE_SOURCE_DIR}/vendor/vstsdk2.4/pluginterfaces/vst2.x)

# Build options ################################################

option(WITH_AUDIOFILE "Use libaudiofile for reading/writing files" OFF)
option(WITH_FLAC "Support for FLAC files (requires libaudiofile)" OFF)

# Platform properties ##########################################

# TODO: Rename to platform_XYZ to conform to coding standard
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(PLATFORM_NAME "Linux")
  set(PLATFORM_CONFIG_DIR "linux")
elseif(APPLE)
  set(PLATFORM_NAME "Mac OS X")
  set(PLATFORM_CONFIG_DIR "mac")
elseif(WIN32)
  set(PLATFORM_NAME "Windows")
  set(PLATFORM_CONFIG_DIR "windows")
else()
  set(PLATFORM_NAME "Unknown")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(PLATFORM_BITS 64)
else()
  set(PLATFORM_BITS 32)
endif()
add_definitions(-DPLATFORM_BITS=${PLATFORM_BITS})

# Build Flags ##################################################

if(WITH_AUDIOFILE)
  add_definitions(-DUSE_AUDIOFILE=1)
endif()

if(WITH_FLAC)
  if(NOT WITH_AUDIOFILE)
    message(FATAL_ERROR "FLAC support requires audiofile to be built")
  endif()
  add_definitions(-DUSE_FLAC=1)
endif()

if(MSVC)
  # We don't care about intdir, binary output path is set above
  set(CMAKE_CFG_INTDIR ".")

  set(CMAKE_C_FLAGS_DEBUG "/D DEBUG=1 /D _DEBUG /MTd /Ob0 /Od /RTC1")
  set(CMAKE_C_FLAGS_MINSIZEREL "/MT /O1 /Ob1 /Oi /D NDEBUG")
  set(CMAKE_C_FLAGS_RELEASE "/MT /O2 /Ob2 /Oi /D NDEBUG")
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "/MT /Zi /O2 /Ob1 /D NDEBUG")

  set(CMAKE_CXX_FLAGS_DEBUG "/D DEBUG=1 /D _DEBUG /MTd /Zi /Ob0 /Od /RTC1")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "/MT /O1 /Ob1 /Oi /D NDEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /Ob2 /Oi /D NDEBUG")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MT /Zi /O2 /Ob1 /D NDEBUG")

  add_definitions("/W3 /D _CRT_SECURE_NO_WARNINGS=1 /D WINDOWS=1")
endif()

if(UNIX)
  add_definitions("-DUNIX=1")

  # GCC flags common to all Unix platforms
  set(SHARED_GCC_FLAGS_LIST
    "-fmessage-length=0"
    "-pipe"

    "-Wno-trigraphs"
    "-Wmissing-field-initializers"
    "-Wreturn-type"
    "-Wunused-variable"
    "-Wshadow"
    "-Wsign-compare"
    "-Wswitch"
    "-Wswitch-default"

    "-Waddress"
    "-Wchar-subscripts"
    "-Wcomment"
    "-Wformat"
    "-Wmaybe-uninitialized"
    "-Wnonnull"
    "-Wparentheses"
    "-Wreturn-type"
    "-Wsequence-point"
    "-Wstrict-aliasing"
    "-Wstrict-overflow=1"
    "-Wswitch"
    "-Wtrigraphs"
    "-Wuninitialized"
    "-Wunused-function"
    "-Wunused-label"
    "-Wunused-value"
    "-Wunused-variable"
    "-Wvolatile-register-var"
  )

  set(SHARED_GCC_CFLAGS_LIST
    "-Wmain"
    "-Wenum-compare"
    "-Wmissing-braces"
    "-Wimplicit-int"
    "-Wimplicit-function-declaration"
    "-Wpointer-sign"
  )

  set(SHARED_GCC_CPPFLAGS_LIST
    "-Wsign-compare"
    "-Weffc++"
    "-Wc++11-compat"
    "-Wreorder"
  )

  # Linux-specific GCC stuff
  if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(PLATFORM_GCC_FLAGS_LIST
      "-std=gnu99"
      "-Wuninitialized"
    )

    find_package(X11)
    find_library(X11 REQUIRED)

    add_definitions("-DLINUX=1")
    add_definitions("-D_POSIX_C_SOURCE=200809L")
    add_definitions("-D__cdecl=")
  endif()

  # Mac OSX GCC stuff
  if(APPLE)
    set(PLATFORM_GCC_FLAGS_LIST
      "-fpascal-strings"
      "-Wnewline-eof"
      "-Wshorten-64-to-32"
      "-fasm-blocks"
      "-mmacosx-version-min=10.5"
    )

    set(PLATFORM_LINKER_FLAGS_LIST
      "-framework AppKit"
      "-framework Carbon"
      "-framework CoreFoundation"
      "-framework Foundation"
    )

    add_definitions("-DMACOSX=1")
    # Homebrew places installed library files in /usr/local, but no major
    # Linux distro does that anymore (last time I checked...)
    include_directories("/usr/local/include")
  endif()

  # CMake doesn't really support multi-line strings, so the
  # compiler flags are in lists above to make them easier to
  # manage. However, we must build strings from the lists in
  # order to set the respective CMake variables which they
  # correspond to.
  string(REPLACE ";" " " SHARED_GCC_FLAGS "${SHARED_GCC_FLAGS_LIST}")
  string(REPLACE ";" " " SHARED_GCC_CFLAGS "${SHARED_GCC_CFLAGS_LIST}")
  string(REPLACE ";" " " SHARED_GCC_CPPFLAGS "${SHARED_GCC_CPPFLAGS_LIST}")
  string(REPLACE ";" " " PLATFORM_GCC_FLAGS "${PLATFORM_GCC_FLAGS_LIST}")
  string(REPLACE ";" " " PLATFORM_LINKER_FLAGS "${PLATFORM_LINKER_FLAGS_LIST}")

  # Set compiler flags from shared & platform-specific lists
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} ${SHARED_GCC_CFLAGS} ${PLATFORM_GCC_FLAGS}")
  set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} ${SHARED_GCC_CFLAGS} ${PLATFORM_GCC_FLAGS}")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} ${SHARED_GCC_CPPFLAGS} ${PLATFORM_GCC_FLAGS}")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${SHARED_GCC_CPPFLAGS} ${PLATFORM_GCC_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${PLATFORM_LINKER_FLAGS}")
endif()

# Subdirectories ###############################################

add_subdirectory(source)
add_subdirectory(main)
add_subdirectory(test)
add_subdirectory(vendor)

# Build summary ################################################

message("-- Build configuration")
message("   C Compiler: ${CMAKE_C_COMPILER}")
message("   C++ Compiler: ${CMAKE_CXX_COMPILER}")
message("   Build type: ${CMAKE_BUILD_TYPE}")
message("   Platform name: ${PLATFORM_NAME}")
message("   Platform bitness: ${PLATFORM_BITS}-bit")
message("-- Build options")
message("   WITH_AUDIOFILE: ${WITH_AUDIOFILE}")
message("   WITH_FLAC: ${WITH_FLAC}")
