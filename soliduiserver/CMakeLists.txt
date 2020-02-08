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

install(TARGETS kded_soliduiserver DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})


########### install files ###############

install(FILES soliduiserver.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kded)

