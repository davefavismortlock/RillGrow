#########################################################################################
#
# CMakeLists.txt for RillGrow (SERIAL VERSION)
#
#########################################################################################
set (CMAKE_LEGACY_CYGWIN_WIN32 0) # Remove when CMake >= 2.8.4 is required
message ("Creating Makefile for RillGrow (serial)")
message ("")

message ("Checking compiler")
cmake_minimum_required (VERSION 2.8.12)
enable_language (CXX)
set (FIND_LIBRARY_USE_LIB64_PATHS true)

# We need c++11, first set the c++11 flag (note that later on we check the compiler for c++11 capability)
set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11")
set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -std=c++11")

#########################################################################################
project (RG)
file(GLOB RG_SOURCE_FILES *.cpp)
set (RG_EXECUTABLE rg)
set (RG_INSTALL_DIR "$ENV{HOME}/erosion/rg")
set (CMAKE_PREFIX_PATH "$ENV{HOME}")

#
# If build type not specified, assume a Debug build
#
if (NOT CMAKE_BUILD_TYPE)
   set (CMAKE_BUILD_TYPE Debug)
endif (NOT CMAKE_BUILD_TYPE)

#
# Set up the compiler flags and build directories for the various build types
#
if (CMAKE_BUILD_TYPE MATCHES Debug)
   set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_DEBUG})
   set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${RG_BINARY_DIR}/Debug CACHE PATH "Directory for Debug builds" FORCE)
   # For Debug builds on any platfor, add RillGrow's "internal" debug flag
   # (used for extra logging) etc.
   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DEBUG")
endif (CMAKE_BUILD_TYPE MATCHES Debug)

if (CMAKE_BUILD_TYPE MATCHES Release)
   set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_RELEASE})
   set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${RG_BINARY_DIR}/Release CACHE PATH "Directory for Release builds" FORCE)
endif (CMAKE_BUILD_TYPE MATCHES Release)

if (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
   set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_RELWITHDEBINFO})
   set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${RG_BINARY_DIR}/RelWithDebInfo CACHE PATH "Directory for RelWithDebInfo builds" FORCE)
endif (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)

if (CMAKE_BUILD_TYPE MATCHES MinSizeRel)
   set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS_MINSIZEREL})
   set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${RG_BINARY_DIR}/MinSizeRel CACHE PATH "Directory for MinSizeRel builds" FORCE)
endif (CMAKE_BUILD_TYPE MATCHES MinSizeRel)

#########################################################################################
#
# Continue setting up the compiler flags, next do those that are always set
#
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DCPU -Dx86")

#
# Now add different compiler flags for different platforms
# TODO this isn't finished
#
if (CMAKE_COMPILER_IS_GNUCC)
   set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")

   if (CMAKE_BUILD_TYPE MATCHES Debug)
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wextra -Wpointer-arith -Wconversion -Wcast-qual -Wcast-align -Wwrite-strings -Wredundant-decls -Wno-strict-overflow -Wshadow -Wuninitialized -Wnull-dereference -Wformat -Wformat-overflow -Wformat-signedness -Wuseless-cast -Wempty-body")

      if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.8")
         # Undefined behaviour sanitizer introduced with gcc 4.9, see http://developerblog.redhat.com/2014/10/16/gcc-undefined-behavior-sanitizer-ubsan/
#         set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fsanitize=shift -fsanitize=integer-divide-by-zero -fsanitize=unreachable -fsanitize=null -fsanitize=return -fsanitize=address")
      endif ()

      # TODO I think these were introduced in a later gcc version, need to check
      if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "5.0")
         # Undefined behaviour sanitizer introduced with gcc 4.9, see http://developerblog.redhat.com/2014/10/16/gcc-undefined-behavior-sanitizer-ubsan/
#         set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=vla-bound -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fsanitize=bounds -fsanitize=alignment -fsanitize=object-size -fsanitize=vptr")
         # TODO not yet working
#         set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmpx -fcheck-pointer-bounds -fsanitize-blacklist=blacklist.txt")
      endif ()

#      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weffc++")
   endif ()

#
# ONLY INCLUDE THIS FROM TIME TO TIME, MAKES RUNS MUCH SLOWER
# TODO need to get it to ignore gdal
#
#    # For Debug builds under gcc
#    if (CMAKE_BUILD_TYPE MATCHES Debug)
#       if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.8")
#          # Undefined behaviour sanitizer introduced with gcc 4.9, see http://developerblog.redhat.com/2014/10/16/gcc-undefined-behavior-sanitizer-ubsan/
#          set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=undefined -fsanitize=shift -fsanitize=integer-divide-by-zero -fsanitize=unreachable -fsanitize=null -fsanitize=return -fsanitize=address")
#       endif (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.8")
# 
#       # TODO I think these were introduced in a later gcc version, need to check
#       if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "5.0")
#          # Undefined behaviour sanitizer introduced with gcc 4.9, see http://developerblog.redhat.com/2014/10/16/gcc-undefined-behavior-sanitizer-ubsan/
#          set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=vla-bound -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fsanitize=bounds -fsanitize=alignment -fsanitize=object-size -fsanitize=vptr")
#          # TODO not yet working
# #         set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mmpx -fcheck-pointer-bounds")
#       endif (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "5.0")
#   if (CMAKE_BUILD_TYPE MATCHES Debug)
#      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weffc++")
#    endif (CMAKE_BUILD_TYPE MATCHES Debug)

   # For RelWithDebInfo builds under gcc, set flag for profiling with gprof
   if (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
   endif (CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)

   # For Release builds under gcc, add an extra flag
   if (CMAKE_BUILD_TYPE MATCHES Release)
      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fomit-frame-pointer")
   endif (CMAKE_BUILD_TYPE MATCHES Release)

   # Finally set some gcc-specific linker flags
   set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--as-needed -Wl,--no-undefined")
endif (CMAKE_COMPILER_IS_GNUCC)

if (MSVC)
  if (MSVC60)						# MS Visual Studio 6.0
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
  else()						# more recent MS Visual Studio
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Wall /Wp64")
  endif (MSVC60)
endif (MSVC)

#########################################################################################
#
# RG cannot be built without GDAL
#
message ("Finding libraries")
find_package (GDAL REQUIRED)
#message (STATUS "GDAL_LIBRARIES=${GDAL_LIBRARIES}")
#message (STATUS "GDAL_INCLUDE_DIRS=${GDAL_INCLUDE_DIRS}")
set (LIBS ${LIBS} ${GDAL_LIBRARIES})
set (CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH} ${GDAL_INCLUDE_DIRS})
#message (STATUS "LIBS=${LIBS}")
#message (STATUS "CMAKE_INCLUDE_PATH=${CMAKE_INCLUDE_PATH}")

#
# However, OpenMP is optional and should never be linked in a Debug build
#
#if (NOT CMAKE_BUILD_TYPE MATCHES Debug)
#   find_package(OpenMP)
#   if (OPENMP_FOUND)
#      set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#   endif (OPENMP_FOUND)
#endif (NOT CMAKE_BUILD_TYPE MATCHES Debug)

#########################################################################################
#
# Finally, the important bits
#
include_directories (${RG_SOURCE_DIR} ${CMAKE_INCLUDE_PATH})
add_executable (${RG_EXECUTABLE} ${RG_SOURCE_FILES})
target_link_libraries (${RG_EXECUTABLE} ${LIBS})
install (TARGETS ${RG_EXECUTABLE} RUNTIME DESTINATION ${RG_INSTALL_DIR})

# Check to see if the compiler supports c++11
set_property (TARGET ${RG_EXECUTABLE} PROPERTY CXX_STANDARD 11)
set_property (TARGET ${RG_EXECUTABLE} PROPERTY CXX_STANDARD_REQUIRED ON)

#########################################################################################
#
# Tell the user what has happened
#
message ("Serial Makefile written")
message (STATUS "CMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}")
message (STATUS "CMAKE_EXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}")
#message (STATUS "CMAKE_RUNTIME_OUTPUT_DIRECTORY=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message (STATUS "CMAKE_INCLUDE_PATH=${CMAKE_INCLUDE_PATH}")
message (STATUS "LIBS=${LIBS}")
message (STATUS "RG_INSTALL_DIR=${RG_INSTALL_DIR}")
message (STATUS "CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")
message (STATUS "CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}")

