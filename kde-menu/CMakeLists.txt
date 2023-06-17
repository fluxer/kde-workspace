add_executable(kde4-menu kde-menu.cpp)

target_link_libraries(kde4-menu ${KDE4_KDECORE_LIBS})

install(
    TARGETS kde4-menu
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)
