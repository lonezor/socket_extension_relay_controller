add_executable(rc_ctrl
    src/main.c
)

set_property(TARGET rc_ctrl PROPERTY C_STANDARD 99)

set_property(TARGET rc_ctrl PROPERTY POSITION_INDEPENDENT_CODE TRUE)

target_link_libraries(rc_ctrl
    PRIVATE
    relay_controller::relay
)

install(
    TARGETS
    rc_ctrl
)

