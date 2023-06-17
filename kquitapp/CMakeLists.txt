set(kquitapp_SRCS kquitapp.cpp)

add_executable(kquitapp ${kquitapp_SRCS})
target_link_libraries(kquitapp KDE4::kdecore)

install(
    TARGETS kquitapp
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)
