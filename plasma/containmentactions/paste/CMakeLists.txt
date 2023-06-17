project(plasma-containmentactions-paste)

set(paste_SRCS
    paste.cpp
)

kde4_add_plugin(plasma_containmentactions_paste ${paste_SRCS})
target_link_libraries(plasma_containmentactions_paste KDE4::plasma KDE4::kio)

install(TARGETS plasma_containmentactions_paste DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-containmentactions-paste.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
