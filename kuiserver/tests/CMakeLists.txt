########### next target ###############

set(kuiservertest_SRCS kuiservertest.cpp)

add_executable(kuiservertest TEST ${kuiservertest_SRCS})

target_link_libraries(kuiservertest ${KDE4_KIO_LIBS})

