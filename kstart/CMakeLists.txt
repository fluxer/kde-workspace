
set(kstart_SRCS kstart.cpp)

add_executable(kstart ${kstart_SRCS})

target_link_libraries(kstart
    ${KDE4_KDEUI_LIBS}
    ${X11_X11_LIB}
)

install(
    TARGETS kstart
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)

