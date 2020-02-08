set(knetattach_SRCS main.cpp knetattach.cpp knetattach.ui )

add_executable(knetattach ${knetattach_SRCS})

target_link_libraries(knetattach ${KDE4_KIO_LIBS})

install(TARGETS knetattach DESTINATION ${KDE4_LIBEXEC_INSTALL_DIR})


########### install files ###############

install(PROGRAMS knetattach.desktop DESTINATION ${KDE4_XDG_APPS_INSTALL_DIR})

kde4_install_icons(${KDE4_ICON_INSTALL_DIR})

