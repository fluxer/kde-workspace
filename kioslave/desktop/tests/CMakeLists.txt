#include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. )

set(testdesktop_SRCS kio_desktop_test.cpp)

kde4_add_test(testdesktop ${testdesktop_SRCS})

target_link_libraries(testdesktop  KDE4::kio KDE4::solid ${QT_QTTEST_LIBRARY})

