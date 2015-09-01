
set(kcmshell_SRCS main.cpp )

add_executable( kcmshell4 ${kcmshell_SRCS})

target_link_libraries(kcmshell4 ${KDE4_KCMUTILS_LIBS} ${KDE4_KDEUI_LIBS} )

install(TARGETS kcmshell4          ${INSTALL_TARGETS_DEFAULT_ARGS})
