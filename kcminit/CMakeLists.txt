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
    CODE "MESSAGE(STATUS \"Symlinking: ${BIN_INSTALL_DIR}/kcminit to: ${BIN_INSTALL_DIR}/kcminit_startup\")"
    CODE "EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E create_symlink ${BIN_INSTALL_DIR}/kcminit ${BIN_INSTALL_DIR}/kcminit_startup)"
)
