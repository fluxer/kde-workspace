project(ktexteditor_autobrace)

add_subdirectory(icons)

########### next target ###############

set(ktexteditor_autobrace_PART_SRCS
    autobrace.cpp
    autobrace_config.cpp
)

kde4_add_plugin(ktexteditor_autobrace ${ktexteditor_autobrace_PART_SRCS})

target_link_libraries(ktexteditor_autobrace
    KDE4::kdecore
    KDE4::ktexteditor
)

install(
    TARGETS ktexteditor_autobrace
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES
    ktexteditor_autobrace.desktop
    ktexteditor_autobrace_config.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
