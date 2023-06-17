include_directories( ToolTips )

set( systemsettings_SRCS
    SystemSettingsApp.cpp
    SettingsBase.cpp
    ToolTips/ktooltip.cpp
    ToolTips/ktooltipwindow.cpp
    ToolTips/tooltipmanager.cpp
    main.cpp
    configDialog.ui
)

KDE4_ADD_KCFG_FILES( systemsettings_SRCS BaseConfig.kcfgc )
QT4_ADD_DBUS_ADAPTOR( systemsettings_SRCS org.kde.systemsettings.xml SystemSettingsApp.h SystemSettingsApp  )

add_executable(systemsettings ${systemsettings_SRCS})

target_link_libraries(systemsettings
    KDE4::plasma
    KDE4::kio
    KDE4::kcmutils
    systemsettingsview
)

install(
    TARGETS systemsettings
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)
install(
    FILES systemsettingsui.rc systemsettings.kcfg
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/systemsettings
)
install(
    PROGRAMS kdesystemsettings.desktop systemsettings.desktop
    DESTINATION ${KDE4_XDG_APPS_INSTALL_DIR}
)
