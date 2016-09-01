set( testChromeBookmarks_SRCS testchromebookmarks.cpp 
  ../browsers/chrome.cpp
  ../faviconfromblob.cpp
  ../browsers/chromefindprofile.cpp
  ../bookmarkmatch.cpp
  ../favicon.cpp
  ../fetchsqlite.cpp
)
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. ${CMAKE_CURRENT_BINARY_DIR}/.. )
kde4_add_test(plasma-runner-bookmarks-TestChromeBookmarks ${testChromeBookmarks_SRCS} )

target_link_libraries(plasma-runner-bookmarks-TestChromeBookmarks
    ${KDE4_KDECORE_LIBS}
    ${QT_QTTEST_LIBRARY}
    ${QT_QTSQL_LIBRARY}
    ${KDE4_PLASMA_LIBS}
    ${KDE4_KIO_LIBS}
    ${QJSON_LIBRARIES} ${qjson_LIBRARIES} 
)

file(COPY chrome-config-home DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
