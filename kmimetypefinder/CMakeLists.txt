add_executable(kmimetypefinder kmimetypefinder.cpp)

target_link_libraries(kmimetypefinder ${KDE4_KDECORE_LIBS})

install(
    TARGETS kmimetypefinder
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)
