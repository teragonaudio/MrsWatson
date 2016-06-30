cmake_minimum_required(VERSION 3.0)
project(MrsWatson)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(cmake_SCRIPTS_DIR ${CMAKE_SOURCE_DIR}/cmake)
include_directories(${CMAKE_SOURCE_DIR}/source)

# Build options ################################################

option(WITH_AUDIOFILE "Use libaudiofile for reading/writing files" ON)
option(WITH_FLAC "Support for FLAC files (requires libaudiofile)" OFF)

# Feature configuration ########################################

if(WITH_AUDIOFILE)
  add_definitions(-DUSE_AUDIOFILE=1)
endif()

if(WITH_FLAC)
  if(NOT WITH_AUDIOFILE)
    message(FATAL_ERROR "FLAC support requires WITH_AUDIOFILE")
  endif()
  add_definitions(-DUSE_FLAC=1)
endif()

# Platform properties ##########################################

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(mw_PLATFORM_NAME "Linux")
  set(mw_PLATFORM_CONFIG_DIR "linux")
elseif(APPLE)
  set(mw_PLATFORM_NAME "Mac OS X")
  set(mw_PLATFORM_CONFIG_DIR "mac")
elseif(WIN32)
  set(mw_PLATFORM_NAME "Windows")
  set(mw_PLATFORM_CONFIG_DIR "windows")
else()
  message(FATAL_ERROR "Unknown or unsupported platform")
endif()

if(UNIX)
  set(mw_BUILD_32 ON)
  set(mw_BUILD_64 ON)
elseif(MSVC)
  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(mw_BUILD_32 OFF)
    set(mw_BUILD_64 ON)
  else()
    set(mw_BUILD_32 ON)
    set(mw_BUILD_64 OFF)
  endif()
endif()

# Build Flags ##################################################

if(MSVC)
  # We don't care about intdir, binary output path is set above
  set(CMAKE_CFG_INTDIR ".")

  set(CMAKE_C_FLAGS_DEBUG "/MP /DDEBUG=1 /D_DEBUG /MTd /Ob0 /Od /RTC1")
  set(CMAKE_C_FLAGS_MINSIZEREL "/MP /MT /O1 /Ob1 /Oi /D NDEBUG")
  set(CMAKE_C_FLAGS_RELEASE "/MP /MT /O2 /Ob2 /Oi /DNDEBUG")
  set(CMAKE_C_FLAGS_RELWITHDEBINFO "/MP /MT /Zi /O2 /Ob1 /DNDEBUG")

  set(CMAKE_CXX_FLAGS_DEBUG "/MP /DDEBUG=1 /D_DEBUG /MTd /Zi /Ob0 /Od /RTC1")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "/MP /MT /O1 /Ob1 /Oi /DNDEBUG")
  set(CMAKE_CXX_FLAGS_RELEASE "/MP /MT /O2 /Ob2 /Oi /DNDEBUG")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MP /MT /Zi /O2 /Ob1 /DNDEBUG")

  add_definitions("/W3 /D_CRT_SECURE_NO_WARNINGS=1 /DWINDOWS=1")
elseif(UNIX)
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

add_subdirectory(vendor)
add_subdirectory(source)
add_subdirectory(main)
add_subdirectory(test)

# Build summary ################################################

message(STATUS "Build configuration")
get_directory_property(preprocessor_flags COMPILE_DEFINITIONS)
message("   Preprocessor flags: ${preprocessor_flags}")
message("   C Compiler: ${CMAKE_C_COMPILER}")
message("   C Compiler flags: ${CMAKE_C_FLAGS}")
message("   C Compiler flags (Debug): ${CMAKE_C_FLAGS_DEBUG}")
message("   C Compiler flags (Release): ${CMAKE_C_FLAGS_RELEASE}")
message("   C++ Compiler: ${CMAKE_CXX_COMPILER}")
message("   C++ Compiler flags: ${CMAKE_CXX_FLAGS}")
message("   C++ Compiler flags (Debug): ${CMAKE_CXX_FLAGS_DEBUG}")
message("   C++ Compiler flags (Release): ${CMAKE_CXX_FLAGS_RELEASE}")
message("   Linker: ${CMAKE_LINKER}")
message("   Linker flags: ${CMAKE_SHARED_LINKER_FLAGS}")
message("   Linker flags (Debug): ${CMAKE_SHARED_LINKER_FLAGS_DEBUG}")
message("   Linker flags (Release): ${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
if(CMAKE_BUILD_TYPE)
  message("   Build type: ${CMAKE_BUILD_TYPE}")
else()
  message("   Build type: Set by IDE")
endif()
message("   Build 32-bit binary: ${mw_BUILD_32}")
message("   Build 64-bit binary: ${mw_BUILD_64}")
message("   Platform name: ${mw_PLATFORM_NAME}")
message(STATUS "Options")
message("   WITH_AUDIOFILE: ${WITH_AUDIOFILE}")
message("   WITH_FLAC: ${WITH_FLAC}")
