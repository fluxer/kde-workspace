project(ktexteditor_lumen)

########### next target ###############

set(ktexteditor_lumen_SRCS
    lumen.cpp
    dcd.cpp
    completion.cpp
)

kde4_add_plugin(ktexteditor_lumen ${ktexteditor_lumen_SRCS})

target_link_libraries(ktexteditor_lumen
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KTEXTEDITOR_LIBS}
)

install(
    TARGETS ktexteditor_lumen
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES ktexteditor_lumen.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
