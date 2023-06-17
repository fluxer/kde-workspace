set(krunner_recentdocuments_SRCS
    recentdocuments.cpp
)

kde4_add_plugin(krunner_recentdocuments ${krunner_recentdocuments_SRCS})
target_link_libraries(krunner_recentdocuments KDE4::plasma KDE4::kio)

install(TARGETS krunner_recentdocuments DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )

install(FILES recentdocuments.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

