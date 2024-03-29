include_directories(${CMAKE_CURRENT_BINARY_DIR}/kbookmarkmodel)

add_subdirectory(kbookmarkmodel)

########### next target ###############

add_executable(kbookmarkmerger kbookmarkmerger.cpp)

target_link_libraries(kbookmarkmerger KDE4::kio)

install(
    TARGETS kbookmarkmerger
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
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

add_executable(keditbookmarks ${keditbookmarks_SRCS})

target_link_libraries(keditbookmarks
    KDE4::kparts
    kbookmarkmodel_private
    konq
)

install(
    TARGETS keditbookmarks
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)

########### install files ###############

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
