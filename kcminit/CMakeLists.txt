########### next target ###############

add_definitions(-DKDE_DEFAULT_DEBUG_AREA=90250)

add_executable(kcminit main.cpp)

target_link_libraries(kcminit
    KDE4::kcmutils
    KDE4::kdeui
    ${X11_LIBRARIES}
)

install(
    TARGETS kcminit
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)

# write a cmake script file which creates the symlink
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/make_kcminit_startup_symlink.cmake
    "EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E create_symlink kcminit \"\$ENV{DESTDIR}${KDE4_BIN_INSTALL_DIR}/kcminit_startup\")\n"
)
# and add it as post-install script for kcminit
set_target_properties(kcminit PROPERTIES
    POST_INSTALL_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/make_kcminit_startup_symlink.cmake
)
