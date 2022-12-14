set(kquitapp_SRCS kquitapp.cpp)

add_executable(kquitapp ${kquitapp_SRCS})
target_link_libraries(kquitapp ${KDE4_KDECORE_LIBS})

install(TARGETS kquitapp ${INSTALL_TARGETS_DEFAULT_ARGS})
