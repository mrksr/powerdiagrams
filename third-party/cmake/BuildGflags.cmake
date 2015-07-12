include(CMakeParseArguments)

function(BuildGflags)
    cmake_parse_arguments(_gflags
        ""
        ""
        "CONFIGURE_COMMAND;BUILD_COMMAND;INSTALL_COMMAND"
        ${ARGN})

    ExternalProject_Add(gflags
        PREFIX ${DEPS_BUILD_DIR}
        URL ${GFLAGS_URL}
        URL_HASH SHA256=${GFLAGS_SHA256}
        DOWNLOAD_DIR ${DEPS_DOWNLOAD_DIR}/gflags
        CONFIGURE_COMMAND "${_gflags_CONFIGURE_COMMAND}"
        BUILD_IN_SOURCE 1
        BUILD_COMMAND "${_gflags_BUILD_COMMAND}"
        INSTALL_COMMAND "${_gflags_INSTALL_COMMAND}"
    )
endfunction()

set(GFLAGS_CONFIGURE_COMMAND ${CMAKE_COMMAND} ${DEPS_BUILD_DIR}/src/gflags
    -DCMAKE_INSTALL_PREFIX=${DEPS_INSTALL_DIR})
set(GFLAGS_BUILD_COMMAND ${CMAKE_COMMAND} --build .)
set(GFLAGS_INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install)

BuildGflags(CONFIGURE_COMMAND ${GFLAGS_CONFIGURE_COMMAND}
    BUILD_COMMAND ${GFLAGS_BUILD_COMMAND}
    INSTALL_COMMAND ${GFLAGS_INSTALL_COMMAND})

list(APPEND THIRD_PARTY_DEPS gflags)
