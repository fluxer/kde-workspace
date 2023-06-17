add_executable(kiconfinder kiconfinder.cpp)

target_link_libraries(kiconfinder ${KDE4_KDEUI_LIBS})

install(
    TARGETS kiconfinder
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)
