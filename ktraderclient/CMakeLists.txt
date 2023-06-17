add_executable(ktraderclient ktraderclient.cpp)
target_link_libraries(ktraderclient KDE4::kdecore)

install(
    TARGETS ktraderclient
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)
