add_executable(ktraderclient ktraderclient.cpp)
target_link_libraries(ktraderclient ${KDE4_KDECORE_LIBS})

install(
    TARGETS ktraderclient
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)
