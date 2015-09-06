kde4_add_test(testkioarchive testkioarchive.cpp)
target_link_libraries(testkioarchive ${KDE4_KIO_LIBS} ${QT_QTTEST_LIBRARY})
