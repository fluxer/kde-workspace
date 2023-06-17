########### next target ###############

set(kwin_killer_helper_SRCS killer.cpp )


add_executable(kwin_killer_helper ${kwin_killer_helper_SRCS})

target_link_libraries(kwin_killer_helper KDE4::kdeui ${X11_X11_LIB})

install(TARGETS kwin_killer_helper DESTINATION ${KDE4_LIBEXEC_INSTALL_DIR} )
