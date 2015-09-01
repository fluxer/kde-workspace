
set(kcm_smserver_PART_SRCS
    kcmsmserver.cpp
    smserverconfigimpl.cpp
    smserverconfigdlg.ui
)

kde4_add_plugin(kcm_smserver ${kcm_smserver_PART_SRCS})


target_link_libraries(kcm_smserver ${KDE4_KDEUI_LIBS})

install(TARGETS kcm_smserver DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install(FILES kcmsmserver.desktop DESTINATION ${SERVICES_INSTALL_DIR})

