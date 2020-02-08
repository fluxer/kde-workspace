########### next target ###############

set(krunner_bookmarksrunner_SRCS
    browserfactory.cpp
    bookmarkmatch.cpp
    faviconfromblob.cpp
    favicon.cpp
    fetchsqlite.cpp
    browsers/opera.cpp
    bookmarksrunner.cpp
    browsers/kdebrowser.cpp
    browsers/firefox.cpp
    browsers/chromefindprofile.cpp
    browsers/chrome.cpp
)

kde4_add_plugin(krunner_bookmarksrunner ${krunner_bookmarksrunner_SRCS})
target_link_libraries(krunner_bookmarksrunner
    ${KDE4_KIO_LIBS}
    ${KDE4_PLASMA_LIBS}
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTSQL_LIBRARY}
)

install(TARGETS krunner_bookmarksrunner DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )

########### install files ###############

install(FILES plasma-runner-bookmarks.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()
