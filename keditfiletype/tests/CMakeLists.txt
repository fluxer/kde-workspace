include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. ${KDE4_INCLUDES} )

########### filetypestest ###############

kde4_add_test(keditfiletype-filetypestest
    filetypestest.cpp
    ../mimetypedata.cpp
    ../mimetypewriter.cpp
)
target_link_libraries(keditfiletype-filetypestest
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KIO_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTTEST_LIBRARY}
)
