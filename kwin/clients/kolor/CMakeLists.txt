########### next target ###############

set(kwin_kolor_SRCS
     kolorbutton.cpp
     kolorclient.cpp
     kolorfactory.cpp
)

kde4_add_plugin(kwin3_kolor ${kwin_kolor_SRCS})
target_link_libraries(kwin3_kolor
    KDE4::kdeui
    kdecorations
)

install(
    TARGETS kwin3_kolor
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############
install(
    FILES kolorclient.desktop
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin
)
