add_subdirectory(pixmaps)

set(kmenueditcommon_STAT_SRCS
    preferencesdlg.cpp
    basictab.cpp
    treeview.cpp
    kmenuedit.cpp
    menufile.cpp
    menuinfo.cpp
)

########### next target ###############

qt4_add_dbus_adaptor(
    kmenueditcommon_STAT_SRCS
    org.kde.kmenuedit.xml
    kmenuedit.h
    KMenuEdit
)

set(kmenuedit_SRCS main.cpp ${kmenueditcommon_STAT_SRCS})

add_executable(kmenuedit ${kmenuedit_SRCS})

target_link_libraries(kmenuedit
    KDE4::kio
    ${QT_QTXML_LIBRARY}
    ${QT_QTDBUS_LIBRARY}
)

install(
    TARGETS kmenuedit
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)

########### install files ###############

install(
    PROGRAMS kmenuedit.desktop
    DESTINATION ${KDE4_XDG_APPS_INSTALL_DIR}
)
install(
    FILES kmenueditui.rc
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kmenuedit
)

kde4_install_icons(${KDE4_ICON_INSTALL_DIR})
