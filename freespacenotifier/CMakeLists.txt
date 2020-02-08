project(freespacenotifier)

include_directories (${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

########### next target ###############

set(kded_freespacenotifier_SRCS
    freespacenotifier.cpp
    module.cpp
)

kde4_add_kcfg_files(kded_freespacenotifier_SRCS
    settings.kcfgc
)

kde4_add_plugin(kded_freespacenotifier ${kded_freespacenotifier_SRCS})

target_link_libraries(kded_freespacenotifier ${KDE4_KIO_LIBS})

install(
    TARGETS kded_freespacenotifier
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES freespacenotifier.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kded
)
install(
    FILES freespacenotifier.notifyrc
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/freespacenotifier
)
install(
    FILES freespacenotifier.kcfg
    DESTINATION ${KDE4_KCFG_INSTALL_DIR}
)
