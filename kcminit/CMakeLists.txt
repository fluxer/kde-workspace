########### next target ###############

add_executable(kcminit main.cpp)

target_link_libraries(kcminit
    ${KDE4_KCMUTILS_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${X11_LIBRARIES}
)

install(
    TARGETS kcminit
    ${INSTALL_TARGETS_DEFAULT_ARGS}
)

########### next target ###############

install(
    CODE "MESSAGE(STATUS \"Symlinking: ${KDE4_BIN_INSTALL_DIR}/kcminit to: ${KDE4_BIN_INSTALL_DIR}/kcminit_startup\")"
    CODE "EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E create_symlink ${KDE4_BIN_INSTALL_DIR}/kcminit ${KDE4_BIN_INSTALL_DIR}/kcminit_startup)"
)
