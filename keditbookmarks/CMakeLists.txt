include_directories(${CMAKE_CURRENT_BINARY_DIR}/kbookmarkmodel)

add_subdirectory(kbookmarkmodel)

########### next target ###############

add_executable(kbookmarkmerger kbookmarkmerger.cpp)

target_link_libraries(kbookmarkmerger ${KDE4_KIO_LIBS})

install(
    TARGETS kbookmarkmerger
    ${INSTALL_TARGETS_DEFAULT_ARGS}
)


########### next target ###############

set(keditbookmarks_SRCS 
    main.cpp
    toplevel.cpp
    globalbookmarkmanager.cpp
    actionsimpl.cpp
    importers.cpp
    bookmarkiterator.cpp
    testlink.cpp
    favicons.cpp
    faviconupdater.cpp
    exporters.cpp
    bookmarkinfowidget.cpp
    kebsearchline.cpp
    bookmarklistview.cpp
)

qt4_generate_dbus_interface(
    ${CMAKE_CURRENT_SOURCE_DIR}/toplevel.h
    org.kde.keditbookmarks.xml
)

qt4_add_dbus_interface(keditbookmarks_SRCS
    ${CMAKE_CURRENT_BINARY_DIR}/org.kde.keditbookmarks.xml
    toplevel_interface
)

qt4_add_dbus_interface(keditbookmarks_SRCS
    ${CMAKE_SOURCE_DIR}/libs/konq/favicons/org.kde.FavIcon.xml
    favicon_interface
)

kde4_add_kcfg_files(keditbookmarks_SRCS settings.kcfgc)

add_executable(keditbookmarks ${keditbookmarks_SRCS})

target_link_libraries(keditbookmarks
    ${KDE4_KPARTS_LIBS}
    kbookmarkmodel_private
    konq
)

install(
    TARGETS keditbookmarks
    ${INSTALL_TARGETS_DEFAULT_ARGS}
)

########### install files ###############

install(
    FILES keditbookmarks.kcfg
    DESTINATION ${KDE4_KCFG_INSTALL_DIR}
)

install(
    FILES
    keditbookmarksui.rc
    keditbookmarks-genui.rc
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/keditbookmarks
)

install(
    PROGRAMS keditbookmarks.desktop
    DESTINATION ${KDE4_XDG_APPS_INSTALL_DIR}
)
