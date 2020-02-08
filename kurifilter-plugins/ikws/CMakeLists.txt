add_subdirectory(searchproviders) 

set(kuriikwsfilter_PART_SRCS
    kuriikwsfiltereng.cpp
    kuriikwsfilter.cpp
    searchprovider.cpp
    ikwsopts.h
    ikwsopts.cpp
    ikwsopts_ui.ui
    searchproviderdlg.cpp
    searchproviderdlg.h
    searchproviderdlg_ui.ui
)

kde4_add_plugin(kuriikwsfilter ${kuriikwsfilter_PART_SRCS})

target_link_libraries(kuriikwsfilter ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS})

install(TARGETS kuriikwsfilter DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

set(kurisearchfilter_PART_SRCS 
    kurisearchfilter.cpp
    kuriikwsfiltereng.cpp
    ikwsopts.h
    ikwsopts.cpp
    searchproviderdlg.cpp
    searchprovider.cpp
    ikwsopts_ui.ui
    searchproviderdlg_ui.ui
)

kde4_add_plugin(kurisearchfilter ${kurisearchfilter_PART_SRCS})

target_link_libraries(kurisearchfilter ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS})

install(TARGETS kurisearchfilter DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES kuriikwsfilter.desktop kurisearchfilter.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )
install( FILES searchprovider.desktop  DESTINATION  ${KDE4_SERVICETYPES_INSTALL_DIR} )
