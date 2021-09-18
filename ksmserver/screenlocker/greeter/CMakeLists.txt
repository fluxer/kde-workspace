include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_SOURCE_DIR}/kcheckpass
    ${CMAKE_SOURCE_DIR}/libs/kdm
    ${CMAKE_SOURCE_DIR}/libs/kworkspace
)

set(kscreenlocker_greet_SRCS
   greeter.cpp
   sessions.cpp
   greeterapp.cpp
   screensaverwindow.cpp
   main.cpp )

kde4_add_kcfg_files(kscreenlocker_greet_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/../kcfg/kscreensaversettings.kcfgc
)

add_executable(kscreenlocker_greet ${kscreenlocker_greet_SRCS})

target_link_libraries(kscreenlocker_greet
    ${KDE4_KDEUI_LIBS}
    ${KDE4_SOLID_LIBS}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${X11_LIBRARIES}
    ${KDE4_PLASMA_LIBS}
    ${KDE4_KDECLARATIVE_LIBS}
    ${SOCKET_LIBRARIES}
    kworkspace
)

install(TARGETS kscreenlocker_greet DESTINATION ${KDE4_LIBEXEC_INSTALL_DIR})

install(
    DIRECTORY themes/org.kde.passworddialog
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/ksmserver/screenlocker
)
