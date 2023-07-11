project(fileviewgitplugin)

include_directories(
    ${LIBGIT2_INCLUDE_DIR}
)

set(fileviewgitplugin_SRCS
    fileviewgitplugin.cpp
    gitcommitdialog.cpp
)

kde4_add_plugin(fileviewgitplugin ${fileviewgitplugin_SRCS})

target_link_libraries(fileviewgitplugin
    KDE4::kdecore
    KDE4::kdeui
    KDE4::kpasswdstore
    KDE4::kio
    konq
    ${LIBGIT2_LIBRARIES}
)

install(
    FILES fileviewgitplugin.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)

install(
    TARGETS fileviewgitplugin
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)
