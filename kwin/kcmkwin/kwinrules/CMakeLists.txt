ADD_DEFINITIONS(-DKCMRULES)
########### next target ###############

set(kwinrules_SRCS
    ruleswidget.cpp
    ruleslist.cpp
    kwinsrc.cpp
    detectwidget.cpp
    yesnobox.h
    ../../client_machine.h
    ruleslist.ui
    detectwidget.ui
    editshortcut.ui
    ruleswidgetbase.ui
)

set(kwin_rules_dialog_SRCS main.cpp ${kwinrules_SRCS})

add_executable( kwin_rules_dialog ${kwin_rules_dialog_SRCS})

set(kwin_kcm_rules_XCB_LIBS
    ${XCB_XCB_LIBRARIES}
    ${X11_XCB_LIBRARIES}
)

target_link_libraries(kwin_rules_dialog KDE4::kdeui KDE4::kio ${X11_LIBRARIES} ${kwin_kcm_rules_XCB_LIBS})

install(TARGETS kwin_rules_dialog         DESTINATION ${KDE4_LIBEXEC_INSTALL_DIR} )

########### next target ###############

set(kcm_kwinrules_PART_SRCS kcm.cpp ${kwinrules_SRCS})


kde4_add_plugin(kcm_kwinrules ${kcm_kwinrules_PART_SRCS})

target_link_libraries(kcm_kwinrules KDE4::kdeui KDE4::kio ${X11_LIBRARIES} ${kwin_kcm_rules_XCB_LIBS})

install(TARGETS kcm_kwinrules  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )


########### next target ###############


########### install files ###############

install( FILES kwinrules.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )


