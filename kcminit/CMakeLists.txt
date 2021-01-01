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

# write a cmake script file which creates the symlink
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/make_kcminit_startup_symlink.cmake
    "EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E create_symlink kcminit \"\$ENV{DESTDIR}${KDE4_BIN_INSTALL_DIR}/kcminit_startup\")\n"
)
# and add it as post-install script for kcminit
set_target_properties(kcminit PROPERTIES
    POST_INSTALL_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/make_kcminit_startup_symlink.cmake
)
