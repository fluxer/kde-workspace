
add_subdirectory(pics)

########### next target ###############

set(kcm_locale_PART_SRCS kcmlocale.cpp)

kde4_add_plugin(kcm_locale ${kcm_locale_PART_SRCS})

target_link_libraries(kcm_locale
    KDE4::kio
    ${QT_QTGUI_LIBRARY}
)

install(
    TARGETS kcm_locale
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES language.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
