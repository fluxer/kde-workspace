########### next target ###############

set(kgreet_classic_PART_SRCS kgreet_classic.cpp )

kde4_add_plugin(kgreet_classic ${kgreet_classic_PART_SRCS})
target_link_libraries(kgreet_classic  ${KDE4_KDEUI_LIBS})

install(
    TARGETS kgreet_classic
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### next target ###############

set(kgreet_generic_PART_SRCS kgreet_generic.cpp )

kde4_add_plugin(kgreet_generic ${kgreet_generic_PART_SRCS})
target_link_libraries(kgreet_generic ${KDE4_KDEUI_LIBS})

install(
    TARGETS kgreet_generic
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES kgreeterplugin.h
    DESTINATION ${KDE4_INCLUDE_INSTALL_DIR}
    COMPONENT Devel
)
