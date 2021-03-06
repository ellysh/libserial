# Release build type:
# cmake -D CMAKE_BUILD_TYPE=Release CMakeLists.txt
#
# Debug build type (by default):
# cmake -D CMAKE_BUILD_TYPE=Debug CMakeLists.txt
# or
# cmake CMakeLists.txt

cmake_minimum_required(VERSION 2.6)
project(libserial)

if(UNIX)
  find_package( Boost 1.40 COMPONENTS system program_options REQUIRED )
  include_directories( ${Boost_INCLUDE_DIR} )
endif(UNIX)

if(UNIX)
  set(CMAKE_INSTALL_PREFIX /usr)
endif(UNIX)

set(COMMON_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/source
    ${LIBMYSQL_STATE_INCLUDE_DIRS})

set(COMMON_LIBDIR
    ${PROJECT_SOURCE_DIR}/source
    ${LIBMYSQL_STATE_LIBDIR})

# Build type by default is Debug
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_BUILD_TYPE)

message(STATUS "Build type: " ${CMAKE_BUILD_TYPE})

# CXXFLAGS and CMAKE_C_FLAGS
if(UNIX)
  set(CMAKE_CXX_FLAGS "-D__LINUX__ -Wvla -Wl,-z,defs -fPIC")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2 -fomit-frame-pointer -funroll-loops -pipe")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} -D__DEBUG__ -O0 -gstabs")
  set(CMAKE_C_FLAGS ${CMAKE_CXX_FLAGS_RELEASE})
endif(UNIX)

# Subdirs
add_subdirectory(source)
add_subdirectory(conf)

include(CMakeListsDeb.txt)
