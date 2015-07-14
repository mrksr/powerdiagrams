# This is heavily inspired by the neovim cmake-files found at http://neovim.org
# This is not meant to be included by the top-level.
cmake_minimum_required (VERSION 2.8.7)
project(POWERDIAGRAMS_DEPS)

# Default to Release Builds
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release.")
    set(CMAKE_BUILD_TYPE "Release")
    mark_as_advanced(CMAKE_BUILD_TYPE)
endif()

# Point CMake at any custom modules we may ship
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(DEPS_INSTALL_DIR "${CMAKE_BINARY_DIR}/usr" CACHE PATH "Dependencies install directory.")
set(DEPS_BIN_DIR "${DEPS_INSTALL_DIR}/bin")
set(DEPS_LIB_DIR "${DEPS_INSTALL_DIR}/lib")
set(DEPS_BUILD_DIR "${CMAKE_BINARY_DIR}/build")
set(DEPS_DOWNLOAD_DIR "${DEPS_BUILD_DIR}/downloads")

option(USE_BUNDLED "Use bundled dependencies." ON)

option(USE_BUNDLED_EIGEN "Use the bundled version of Eigen." ${USE_BUNDLED})
option(USE_BUNDLED_QHULL "Use the bundled version of Qhull." ${USE_BUNDLED})
option(USE_BUNDLED_GFLAGS "Use the bundled version of gflags." ${USE_BUNDLED})

if(UNIX)
  find_program(MAKE_PRG NAMES gmake make)
  if(MAKE_PRG)
    execute_process(
      COMMAND "${MAKE_PRG}" --version
      OUTPUT_VARIABLE MAKE_VERSION_INFO)
    if(NOT "${OUTPUT_VARIABLE}" MATCHES ".*GNU.*")
      unset(MAKE_PRG)
    endif()
  endif()
  if(NOT MAKE_PRG)
    message(FATAL_ERROR "GNU Make is required to build the dependencies.")
  else()
    message(STATUS "Found GNU Make at ${MAKE_PRG}")
  endif()
endif()

if(CMAKE_GENERATOR MATCHES "Makefiles")
  set(MAKE_PRG "$(MAKE)")
endif()

include(ExternalProject)

set(EIGEN_URL http://bitbucket.org/eigen/eigen/get/3.2.5.tar.bz2)
set(EIGEN_SHA256 5f6e6cb88188e34185f43cb819d7dab9b48ef493774ff834e568f4805d3dc2f9)

set(QHULL_URL http://www.qhull.org/download/qhull-2012.1-src.tgz)
set(QHULL_SHA256 a35ecaa610550b7f05c3ce373d89c30cf74b059a69880f03080c556daebcff88)

set(GFLAGS_URL https://github.com/gflags/gflags/archive/v2.1.2.tar.gz)
set(GFLAGS_SHA256 d8331bd0f7367c8afd5fcb5f5e85e96868a00fd24b7276fa5fcee1e5575c2662)

if(USE_BUNDLED_EIGEN)
    include(BuildEigen)
endif()

if(USE_BUNDLED_QHULL)
    include(BuildQhull)
endif()

if(USE_BUNDLED_GFLAGS)
    include(BuildGflags)
endif()

add_custom_target(clean-shared-libraries
  COMMAND ${CMAKE_COMMAND}
    -DREMOVE_FILE_GLOB=${DEPS_INSTALL_DIR}/lib/${CMAKE_SHARED_LIBRARY_PREFIX}*${CMAKE_SHARED_LIBRARY_SUFFIX}*
    -P ${PROJECT_SOURCE_DIR}/cmake/RemoveFiles.cmake
  DEPENDS ${THIRD_PARTY_DEPS}
)

add_custom_target(third-party ALL
  COMMAND ${CMAKE_COMMAND} -E touch .third-party
  DEPENDS clean-shared-libraries)
