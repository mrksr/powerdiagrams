########################
#  Project definition  #
########################
# set(CMAKE_BUILD_TYPE_INIT "Release")
project(POWERDIAGRAMS)

cmake_minimum_required (VERSION 2.8.7)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Prefer bunled versions of dependenceis.
set(DEPS_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/.deps/usr" CACHE PATH "Path prefix for finding dependencies")
list(INSERT CMAKE_PREFIX_PATH 0 ${DEPS_PREFIX})
set(ENV{PKG_CONFIG_PATH} "$ENV{PKG_CONFIG_PATH}:${DEPS_PREFIX}/lib/pkgconfig")

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

find_package(gflags REQUIRED)

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
file(GLOB_RECURSE ownSRC RELATIVE "${PROJECT_SOURCE_DIR}" "src/*.cpp")

add_executable(powerdiagram
    ${ownSRC}
    )
target_link_libraries(powerdiagram
    ${LIB_LIBS}
    gflags
    )
