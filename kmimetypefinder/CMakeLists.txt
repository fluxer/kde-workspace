add_executable(kmimetypefinder kmimetypefinder.cpp)

target_link_libraries(kmimetypefinder KDE4::kdecore)

install(
    TARGETS kmimetypefinder
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)
