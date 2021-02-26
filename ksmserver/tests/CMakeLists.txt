include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/..)

set(testsh_SRCS
    test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../shutdowndlg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../logouteffect.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../curtaineffect.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../fadeeffect.cpp)

kde4_add_manual_test(testsh ${testsh_SRCS})

target_link_libraries(testsh
    kworkspace
    ${KDE4_KDEUI_LIBS}
    ${KDE4_SOLID_LIBS}
    ${KDE4_KDECLARATIVE_LIBS}
    ${KDE4_PLASMA_LIBS}
    ${X11_X11_LIB}
    ${QT_QTDECLARATIVE_LIBRARY}
)
