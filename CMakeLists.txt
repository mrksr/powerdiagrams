########################
#  Project definition  #
########################
# This minimum required is a bit of a lie, it might be much lower actually.
cmake_minimum_required (VERSION 2.8.7)
project(POWERDIAGRAMS)

# Default to Release Builds
if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type selected, default to Release.")
    set(CMAKE_BUILD_TYPE "Release")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Prefer bundled versions of dependenceis.
set(DEPS_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/.deps/usr" CACHE PATH "Path prefix for finding dependencies")
list(INSERT CMAKE_PREFIX_PATH 0 ${DEPS_PREFIX})
set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${DEPS_PREFIX}/lib/pkgconfig")
link_directories(${DEPS_PREFIX}/lib)
#############
#  OPTIONS  #
#############
# option(WITH_CGAL "Add CGAL convex hull algorithm" OFF)
option(WITH_QHULL "Add qhull convex hull algorithm" ON)

###############
#  Libraries  #
###############
set(LIB_INCS)
set(LIB_LIBS)

find_package(Eigen3 REQUIRED)
list(APPEND LIB_INCS ${EIGEN3_INCLUDE_DIR})
list(APPEND LIB_LIBS ${EIGEN3_LIBRARIES})

find_package(Threads REQUIRED)
find_package(gflags REQUIRED)
list(APPEND LIB_LIBS gflags ${CMAKE_THREAD_LIBS_INIT})
if(WIN32)
    list(APPEND LIB_LIBS shlwapi)
endif()

#NOTE: CGAL convex hull is not implemented right now.
# if(WITH_CGAL)
#     find_package(CGAL REQUIRED)
#     list(APPEND LIB_INCS ${CGAL_INCLUDE_DIRS})
#     list(APPEND LIB_LIBS ${CGAL_LIBRARY})

#     add_definitions(-DHAVE_CGAL)
# endif()

if(WITH_QHULL)
    # FIXME: Non-static build breaks qhull
    set(QHULL_USE_STATIC 1)

    find_package(Qhull REQUIRED)
    list(APPEND LIB_INCS ${QHULL_INCLUDE_DIRS})
    list(APPEND LIB_LIBS ${QHULL_LIBRARY_CPP})
    list(APPEND LIB_LIBS ${QHULL_LIBRARY})

    add_definitions(-DHAVE_QHULL)
endif()

####################
#  Compiler Flags  #
####################
if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions("-std=c++11")
    add_definitions("-Wall -Wextra -Wno-deprecated-declarations -Wno-unused-parameter -pedantic")
elseif(__COMPILER_CLANG)
    add_definitions("-std=c++11")
    add_definitions("-Wall -Wno-deprecated-declarations -pedantic")
endif()

###############################
#  Source Files, Executables  #
###############################
include_directories(
    ${LIB_INCS}
    "${PROJECT_SOURCE_DIR}/include"
    )

set(OWN_SRC
    "src/powerdiagram/FromCSV.cpp"
    "src/powerdiagram/PowerDiagramDual.cpp"
    "src/powerdiagram/PowerDiagramNaive.cpp"
    "src/powerdiagram_main.cpp"
    )

if(WITH_QHULL)
    list(APPEND OWN_SRC "src/powerdiagram/ConvexHullQhull.cpp")
endif()

add_executable(powerdiagram
    ${OWN_SRC}
    )
target_link_libraries(powerdiagram
    ${LIB_LIBS}
    )
