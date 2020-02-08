set(apps_engine_SRCS
    appsengine.cpp
    appsource.cpp
    appservice.cpp
    appjob.cpp
)

kde4_add_plugin(plasma_engine_apps ${apps_engine_SRCS})
target_link_libraries(plasma_engine_apps ${KDE4_KDEUI_LIBS} ${KDE4_PLASMA_LIBS} ${KDE4_KIO_LIBS})

install(TARGETS plasma_engine_apps DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-dataengine-apps.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
install(FILES apps.operations DESTINATION ${KDE4_DATA_INSTALL_DIR}/plasma/services)
