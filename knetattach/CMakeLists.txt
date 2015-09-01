set(knetattach_SRCS main.cpp knetattach.cpp knetattach.ui )

add_executable(knetattach ${knetattach_SRCS})

target_link_libraries(knetattach ${KDE4_KIO_LIBS})

install(TARGETS knetattach DESTINATION ${LIBEXEC_INSTALL_DIR})


########### install files ###############

install(PROGRAMS knetattach.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

kde4_install_icons(${ICON_INSTALL_DIR})

