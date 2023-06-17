set(kfile_SRCS kfile.cpp)

add_executable(kfile4 ${kfile_SRCS})

target_link_libraries(kfile4 KDE4::kio)

install(
    TARGETS kfile4
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)
