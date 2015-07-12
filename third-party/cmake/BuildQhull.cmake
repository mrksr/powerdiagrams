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
        DOWNLOAD_DIR ${DEPS_DOWNLOAD_DIR}/qhull
        DOWNLOAD_COMMAND ${CMAKE_COMMAND}
            -DPREFIX=${DEPS_BUILD_DIR}
            -DDOWNLOAD_DIR=${DEPS_DOWNLOAD_DIR}/qhull
            -DURL=${QHULL_URL}
            -DEXPECTED_SHA256=${QHULL_SHA256}
            -DTARGET=qhull
            -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/DownloadAndExtractFile.cmake
        CONFIGURE_COMMAND "${_qhull_CONFIGURE_COMMAND}"
        BUILD_IN_SOURCE 1
        BUILD_COMMAND "${_qhull_BUILD_COMMAND}"
        INSTALL_COMMAND "${_qhull_INSTALL_COMMAND}"
    )
endfunction()

set(QHULL_CONFIGURE_COMMAND ${CMAKE_COMMAND} ${DEPS_BUILD_DIR}/src/qhull
    -DCMAKE_INSTALL_PREFIX=${DEPS_INSTALL_DIR})
set(QHULL_BUILD_COMMAND ${CMAKE_COMMAND} --build .)
set(QHULL_INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install)

BuildQhull(CONFIGURE_COMMAND ${QHULL_CONFIGURE_COMMAND}
    BUILD_COMMAND ${QHULL_BUILD_COMMAND}
    INSTALL_COMMAND ${QHULL_INSTALL_COMMAND})

list(APPEND THIRD_PARTY_DEPS qhull)
