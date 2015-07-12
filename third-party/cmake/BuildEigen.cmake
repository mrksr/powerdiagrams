include(CMakeParseArguments)

function(BuildEigen)
    cmake_parse_arguments(_eigen
        ""
        ""
        "CONFIGURE_COMMAND;BUILD_COMMAND;INSTALL_COMMAND"
        ${ARGN})

    ExternalProject_Add(eigen
        PREFIX ${DEPS_BUILD_DIR}
        URL ${EIGEN_URL}
        DOWNLOAD_DIR ${DEPS_DOWNLOAD_DIR}/eigen
        DOWNLOAD_COMMAND ${CMAKE_COMMAND}
            -DPREFIX=${DEPS_BUILD_DIR}
            -DDOWNLOAD_DIR=${DEPS_DOWNLOAD_DIR}/eigen
            -DURL=${EIGEN_URL}
            -DEXPECTED_SHA256=${EIGEN_SHA256}
            -DTARGET=eigen
            -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/DownloadAndExtractFile.cmake
        CONFIGURE_COMMAND "${_eigen_CONFIGURE_COMMAND}"
        BUILD_COMMAND "${_eigen_BUILD_COMMAND}"
        INSTALL_COMMAND "${_eigen_INSTALL_COMMAND}"
    )
endfunction()

set(EIGEN_CONFIGURE_COMMAND ${CMAKE_COMMAND} ${DEPS_BUILD_DIR}/src/eigen
    -DCMAKE_INSTALL_PREFIX=${DEPS_INSTALL_DIR})
set(EIGEN_BUILD_COMMAND ${CMAKE_COMMAND} --build .)
set(EIGEN_INSTALL_COMMAND ${CMAKE_COMMAND} --build . --target install)

BuildEigen(CONFIGURE_COMMAND ${EIGEN_CONFIGURE_COMMAND}
    BUILD_COMMAND ${EIGEN_BUILD_COMMAND}
    INSTALL_COMMAND ${EIGEN_INSTALL_COMMAND})

list(APPEND THIRD_PARTY_DEPS eigen)
