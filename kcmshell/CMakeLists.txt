add_definitions(-DKDE_DEFAULT_DEBUG_AREA=90220)

add_executable(kcmshell4 main.cpp)

target_link_libraries(kcmshell4
    KDE4::kcmutils
    KDE4::kdeui
)

install(
    TARGETS kcmshell4
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
    COMPONENT RUNTIME
)
