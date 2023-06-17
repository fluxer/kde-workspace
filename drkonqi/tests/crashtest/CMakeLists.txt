set(crashtest_SRCS crashtest.cpp )

kde4_add_manual_test(crashtest ${crashtest_SRCS})

target_link_libraries(crashtest KDE4::kdeui)
