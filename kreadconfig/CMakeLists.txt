########### next target ###############

set(kreadconfig_SRCS kreadconfig.cpp )

add_executable(kreadconfig ${kreadconfig_SRCS})

target_link_libraries(kreadconfig ${KDE4_KDECORE_LIBS})

install(
    TARGETS kreadconfig
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)

########### next target ###############

set(kwriteconfig_SRCS kwriteconfig.cpp )

add_executable(kwriteconfig ${kwriteconfig_SRCS})
target_link_libraries(kwriteconfig ${KDE4_KDECORE_LIBS})

install(
    TARGETS kwriteconfig
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)
