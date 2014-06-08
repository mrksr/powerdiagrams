# set(CMAKE_BUILD_TYPE_INIT "Release")
project(PowerDiagrams)

cmake_minimum_required (VERSION 2.8.0)
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake/")

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

find_package(CGAL REQUIRED)
if (CGAL_FOUND)
    list(APPEND LIB_INCS ${CGAL_INCLUDE_DIRS})
    list(APPEND LIB_LIBS ${CGAL_LIBRARY})
endif()

####################
#  Compiler Flags  #
####################
if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions("-std=c++11")
    add_definitions("-Wall -Wextra -Wno-unused-parameter -pedantic")
endif()

##################
#  Source Files  #
##################
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

message("${LIB_INCS}")
