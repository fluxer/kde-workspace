set(ksystraycmd_SRCS ksystraycmd.cpp main.cpp )

add_executable(ksystraycmd ${ksystraycmd_SRCS})
target_link_libraries(ksystraycmd
    ${KDE4_KDEUI_LIBS}
    ${X11_X11_LIB}
)

install(
    TARGETS ksystraycmd
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)
