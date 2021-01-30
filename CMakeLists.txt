cmake_minimum_required(VERSION 3.16.3)

project(socket_extension_relay_controller
    VERSION 0.50
)

include(GNUInstallDirs)
include(CTest)

add_subdirectory(relay_control)

install(
    TARGETS
        rc_relay
    EXPORT
        rc-targets
)

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
    rc-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY ExactVersion
)

set(ConfigPackageLocation ${CMAKE_INSTALL_LIBDIR}/cmake/rc)
install(
    EXPORT rc-targets
    NAMESPACE
        relay_controller::
    DESTINATION
        ${ConfigPackageLocation}
)

install(
    FILES
        cmake/rc-config.cmake
        "${CMAKE_CURRENT_BINARY_DIR}/rc-version.cmake"
    DESTINATION
        ${ConfigPackageLocation}
)
