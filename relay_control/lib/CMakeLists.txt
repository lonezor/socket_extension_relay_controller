add_library(rc_relay STATIC)
add_library(relay_controller::relay ALIAS rc_relay)

target_sources(rc_relay
    PRIVATE
    include/rc/relay.h
    src/relay.c
)

set_property(TARGET rc_relay PROPERTY C_STANDARD 99)

set_property(TARGET rc_relay PROPERTY POSITION_INDEPENDENT_CODE TRUE)

target_include_directories(rc_relay
   PUBLIC
   $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
   $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(
    TARGETS
    rc_relay
)
