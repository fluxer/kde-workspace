########### next target ###############

set(soliduiserver_helper_srcs
    soliduiserver_helper.cpp
)

add_executable(soliduiserver_helper ${soliduiserver_helper_srcs})
target_link_libraries(soliduiserver_helper PUBLIC ${KDE4_KDECORE_LIBS})

########### next target ###############

set(kded_soliduiserver_SRCS
   soliduiserver.cpp
   deviceactionsdialog.cpp
   deviceaction.cpp
   devicenothingaction.cpp
   deviceserviceaction.cpp
   deviceactionsdialogview.ui
)

kde4_add_plugin(kded_soliduiserver ${kded_soliduiserver_SRCS})

target_link_libraries(kded_soliduiserver ${KDE4_SOLID_LIBS}  ${KDE4_KIO_LIBS})


########### install files ###############

install(TARGETS kded_soliduiserver DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

install(FILES soliduiserver.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kded)

install(TARGETS soliduiserver_helper DESTINATION ${KDE4_LIBEXEC_INSTALL_DIR})

kde4_install_auth_helper_files(soliduiserver_helper org.kde.soliduiserver.mountunmounthelper root)