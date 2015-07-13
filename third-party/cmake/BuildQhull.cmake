include(CMakeParseArguments)

function(BuildQhull)
    cmake_parse_arguments(_qhull
        ""
        ""
        "CONFIGURE_COMMAND;BUILD_COMMAND;INSTALL_COMMAND"
        ${ARGN})

    ExternalProject_Add(qhull
        PREFIX ${DEPS_BUILD_DIR}
        URL ${QHULL_URL}
        URL_HASH SHA256=${QHULL_SHA256}
        DOWNLOAD_DIR ${DEPS_DOWNLOAD_DIR}/qhull
        CONFIGURE_COMMAND "${_qhull_CONFIGURE_COMMAND}"
        BUILD_IN_SOURCE 1
        BUILD_COMMAND "${_qhull_BUILD_COMMAND}"
        INSTALL_COMMAND "${_qhull_INSTALL_COMMAND}"
    )
endfunction()

# CMAKE_BUILD_TYPE is for Makefiles and such
set(QHULL_CONFIGURE_COMMAND ${CMAKE_COMMAND} ${DEPS_BUILD_DIR}/src/qhull
    -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_INSTALL_PREFIX=${DEPS_INSTALL_DIR})
# --config is for MSVC
set(GFLAGS_BUILD_COMMAND ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE})
set(GFLAGS_INSTALL_COMMAND ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE} --target install)

BuildQhull(CONFIGURE_COMMAND ${QHULL_CONFIGURE_COMMAND}
    BUILD_COMMAND ${QHULL_BUILD_COMMAND}
    INSTALL_COMMAND ${QHULL_INSTALL_COMMAND})

list(APPEND THIRD_PARTY_DEPS qhull)
