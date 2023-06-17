########################################################
# Test TabBox::ClientModel
########################################################
set( testTabBoxClientModel_SRCS
     ../clientmodel.cpp
     ../desktopmodel.cpp
     ../tabboxconfig.cpp
     ../tabboxhandler.cpp
     test_tabbox_clientmodel.cpp
     mock_declarative.cpp
     mock_tabboxhandler.cpp
     mock_tabboxclient.cpp
)

kde4_add_test(kwin-testTabBoxClientModel ${testTabBoxClientModel_SRCS} )

target_link_libraries(kwin-testTabBoxClientModel KDE4::kdeui ${QT_QTDECLARATIVE_LIBRARY} ${X11_LIBRARIES} ${QT_QTTEST_LIBRARY} )

########################################################
# Test TabBox::TabBoxHandler
########################################################
set( testTabBoxHandler_SRCS
     ../clientmodel.cpp
     ../desktopmodel.cpp
     ../tabboxconfig.cpp
     ../tabboxhandler.cpp
     test_tabbox_handler.cpp
     mock_declarative.cpp
     mock_tabboxhandler.cpp
     mock_tabboxclient.cpp
)

kde4_add_test(kwin-testTabBoxHandler ${testTabBoxHandler_SRCS})

target_link_libraries(kwin-testTabBoxHandler
    KDE4::kdeui
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTTEST_LIBRARY}
    ${X11_LIBRARIES}
)

########################################################
# Test TabBox::TabBoxConfig
########################################################
set( testTabBoxConfig_SRCS
    ../tabboxconfig.cpp
    test_tabbox_config.cpp
)

kde4_add_test(kwin-testTabBoxConfig ${testTabBoxConfig_SRCS})
target_link_libraries(kwin-testTabBoxConfig ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})


########################################################
# Test TabBox::DesktopChainManager
########################################################
set( testDesktopChain_SRCS
    ../desktopchain.cpp
    test_desktopchain.cpp
)

kde4_add_test(kwin-testDesktopChain ${testDesktopChain_SRCS} )
target_link_libraries(kwin-testDesktopChain ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY})
