include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. )

########### next target ###############

set(testtrash_SRCS 
    testtrash.cpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/../trashimpl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../trashsizecache.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../discspaceutil.cpp
)

kde4_add_test(testtrash ${testtrash_SRCS})

target_link_libraries(testtrash KDE4::kio KDE4::solid ${QT_QTTEST_LIBRARY})
