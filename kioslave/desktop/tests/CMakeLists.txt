#include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. )

set(testdesktop_SRCS kio_desktop_test.cpp)

kde4_add_test(testdesktop ${testdesktop_SRCS})

target_link_libraries(testdesktop  ${KDE4_KIO_LIBS} ${KDE4_SOLID_LIBS} ${QT_QTTEST_LIBRARY})

