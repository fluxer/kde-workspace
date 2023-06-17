set(kurifiltertest_SRCS kurifiltertest.cpp)

kde4_add_test(kurifiltertest ${kurifiltertest_SRCS})

target_link_libraries(kurifiltertest KDE4::kio ${QT_QTTEST_LIBRARY})

