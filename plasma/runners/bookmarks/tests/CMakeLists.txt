set( testChromeBookmarks_SRCS
    testchromebookmarks.cpp 
    ../browsers/chrome.cpp
    ../browsers/chromefindprofile.cpp
    ../bookmarkmatch.cpp
    ../favicon.cpp
)
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_BINARY_DIR}/..
)

kde4_add_test(plasma-runner-bookmarks-TestChromeBookmarks ${testChromeBookmarks_SRCS})

target_link_libraries(plasma-runner-bookmarks-TestChromeBookmarks
    KDE4::kdecore
    KDE4::plasma
    KDE4::kio
    ${QT_QTTEST_LIBRARY}
    ${QT_QTDBUS_LIBRARY}
)

file(COPY chrome-config-home DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
