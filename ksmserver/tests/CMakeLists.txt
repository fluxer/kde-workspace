include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/..)

set(testsh_SRCS
    test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../shutdowndlg.cpp
)

kde4_add_manual_test(testsh ${testsh_SRCS})

target_link_libraries(testsh
    kworkspace
    KDE4::kdeui
    KDE4::solid
    KDE4::kdeclarative
    KDE4::plasma
    ${X11_X11_LIB}
    ${QT_QTDECLARATIVE_LIBRARY}
)
