########################################################
# Test ScreenPaintData
########################################################
set( testScreenPaintData_SRCS test_screen_paint_data.cpp )
kde4_add_test(kwin-testScreenPaintData ${testScreenPaintData_SRCS} )

target_link_libraries(kwin-testScreenPaintData kwineffects ${QT_QTCORE_LIBRARY} ${QT_QTTEST_LIBRARY} )

########################################################
# Test WindowPaintData
########################################################
set( testWindowPaintData_SRCS test_window_paint_data.cpp )
kde4_add_test(kwin-testWindowPaintData ${testWindowPaintData_SRCS})

target_link_libraries(kwin-testWindowPaintData kwineffects ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTTEST_LIBRARY})

########################################################
# Test VirtualDesktopManager
########################################################
set( testVirtualDesktops_SRCS
     test_virtual_desktops.cpp
     ../virtualdesktops.cpp
)
kde4_add_test(kwin-testVirtualDesktops ${testVirtualDesktops_SRCS})

target_link_libraries(kwin-testVirtualDesktops
    ${KDE4_KDEUI_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTTEST_LIBRARY}
)

########################################################
# Test ClientMachine
########################################################
set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/../cursor.cpp
    PROPERTIES SKIP_UNITY_BUILD_INCLUSION TRUE
)

set( testClientMachine_SRCS
     test_client_machine.cpp
     ../client_machine.cpp
     ../utils.cpp
     ../atoms.cpp # needed by utils.cpp
     ../cursor.cpp # needed by utils.cpp
)
kde4_add_test(kwin-testClientMachine ${testClientMachine_SRCS})

target_link_libraries(kwin-testClientMachine
    ${QT_QTTEST_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    ${KDE4_KDEUI_LIBS}
    ${XCB_XCB_LIBRARIES}
    ${XCB_XFIXES_LIBRARIES}
    ${X11_XCB_LIBRARIES}
    ${X11_X11_LIB} # to make jenkins happy
    ${X11_Xcursor_LIB}
)

########################################################
# Test XcbWrapper
########################################################
set( testXcbWrapper_SRCS
     test_xcb_wrapper.cpp
)
kde4_add_test(kwin-testXcbWrapper ${testXcbWrapper_SRCS})

target_link_libraries(kwin-testXcbWrapper
    ${QT_QTTEST_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${XCB_XCB_LIBRARIES}
    ${X11_XCB_LIBRARIES}
)

########################################################
# Test XcbWindow
########################################################
set( testXcbWindow_SRCS
     test_xcb_window.cpp
)
kde4_add_test(kwin-testXcbWindow ${testXcbWindow_SRCS})

target_link_libraries(kwin-testXcbWindow
    ${QT_QTTEST_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${XCB_XCB_LIBRARIES}
    ${X11_XCB_LIBRARIES}
)
