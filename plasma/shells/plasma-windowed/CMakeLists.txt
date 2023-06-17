include_directories(
    ${CMAKE_SOURCE_DIR}/libs
    ${CMAKE_SOURCE_DIR}/libs/kworkspace
)


set(plasma-windowed_SRCS
    singleview.cpp
    main.cpp
    plasmaapp.cpp
    ${appletbrowserdialog_SRCS}
)

add_executable(plasma-windowed ${plasma-windowed_SRCS})

target_link_libraries(plasma-windowed
    ${KDE4_PLASMA_LIBS}
    ${KDE4_KIO_LIBS}
    ${X11_LIBRARIES}
    ${KDE4_KFILE_LIBS}
    kworkspace
)

install(
    TARGETS plasma-windowed
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)

