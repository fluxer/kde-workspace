include_directories( ToolTips )

SET( systemsettings_SRCS
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

add_executable( systemsettings ${systemsettings_SRCS})

TARGET_LINK_LIBRARIES( systemsettings ${KDE4_PLASMA_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KCMUTILS_LIBS} systemsettingsview )

INSTALL( TARGETS systemsettings ${INSTALL_TARGETS_DEFAULT_ARGS} )
INSTALL( FILES systemsettingsui.rc systemsettings.kcfg DESTINATION ${KDE4_DATA_INSTALL_DIR}/systemsettings )
INSTALL( PROGRAMS kdesystemsettings.desktop systemsettings.desktop DESTINATION ${KDE4_XDG_APPS_INSTALL_DIR} )
