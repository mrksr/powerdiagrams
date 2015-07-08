########################
#  Project definition  #
########################
# set(CMAKE_BUILD_TYPE_INIT "Release")
project(PowerDiagrams)

cmake_minimum_required (VERSION 2.8.0)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

#############
#  OPTIONS  #
#############
option(WITH_CGAL "Add CGAL convex hull algorithm" OFF)
option(WITH_QHULL "Add qhull convex hull algorithm" ON)
option(VERBOSE "Compile with verbose output" OFF)

###############
#  Libraries  #
###############
set(LIB_INCS)
set(LIB_LIBS)

find_package(Eigen3 REQUIRED)
if(EIGEN3_FOUND)
    list(APPEND LIB_INCS ${EIGEN3_INCLUDE_DIR})
    list(APPEND LIB_LIBS ${EIGEN3_LIBRARIES})
endif()

if(WITH_CGAL)
    find_package(CGAL REQUIRED)
    if (CGAL_FOUND)
        list(APPEND LIB_INCS ${CGAL_INCLUDE_DIRS})
        list(APPEND LIB_LIBS ${CGAL_LIBRARY})
    endif()

    add_definitions(-DHAVE_CGAL)
endif()

if(WITH_QHULL)
    # FIXME: Non-static build breaks qhull
    set(QHULL_USE_STATIC 1)

    find_package(Qhull REQUIRED)
    if(QHULL_FOUND)
        list(APPEND LIB_INCS ${QHULL_INCLUDE_DIRS})
        list(APPEND LIB_LIBS ${QHULL_LIBRARY_CPP})
        list(APPEND LIB_LIBS ${QHULL_LIBRARY})
    endif()

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

if(VERBOSE)
    add_definitions("-D_VERBOSE_")
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
    )
