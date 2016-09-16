macro(KEYBOARD_DAEMON_UNIT_TESTS _testname)
    kde4_add_test(kcm-keyboard-${_testname} ${_testname}_test.cpp ../${_testname}.cpp)
    target_link_libraries(kcm-keyboard-${_testname}
        ${KDE4_KDEUI_LIBS}
        ${QT_QTXML_LIBRARY}
        ${QT_QTTEST_LIBRARY}
        ${X11_Xkbfile_LIB}
        ${X11_LIBRARIES}
    )
endmacro(KEYBOARD_DAEMON_UNIT_TESTS)

macro(KEYBOARD_DAEMON_UNIT_TESTS2 _testname _src1 _src2 _src3)
    kde4_add_test(kcm-keyboard-${_testname}
        ${_testname}_test.cpp
        ../${_testname}.cpp
        ../${_src1}
        ../${_src2}
        ../${_src3}
    )
    target_link_libraries(kcm-keyboard-${_testname}
        ${KDE4_KDEUI_LIBS}
        ${QT_QTXML_LIBRARY}
        ${QT_QTTEST_LIBRARY}
        ${X11_Xkbfile_LIB}
    )
endmacro(KEYBOARD_DAEMON_UNIT_TESTS2)


KEYBOARD_DAEMON_UNIT_TESTS(xkb_rules)
KEYBOARD_DAEMON_UNIT_TESTS(iso_codes)

kde4_add_test(kcm-keyboard-flags_test
    flags_test.cpp
    ../flags.cpp
    ../x11_helper.cpp
    ../keyboard_config.cpp
    ../xkb_rules.cpp
)
target_link_libraries(kcm-keyboard-flags_test
    ${KDE4_KDEUI_LIBS}
    ${KDE4_PLASMA_LIBS}
    ${QT_QTXML_LIBRARY}
    ${QT_QTTEST_LIBRARY}
    ${X11_Xkbfile_LIB}
    ${X11_LIBRARIES}
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/base.1.1.xml ${CMAKE_CURRENT_BINARY_DIR}/config/base.1.1.xml COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/base.bad.xml ${CMAKE_CURRENT_BINARY_DIR}/config/base.bad.xml COPYONLY)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config/base.xml ${CMAKE_CURRENT_BINARY_DIR}/config/base.xml COPYONLY)

if(X11_XTest_FOUND)
    kde4_add_test(kcm-keyboard-keyboard_daemon_test
        keyboard_daemon_test.cpp
        ../keyboard_daemon.cpp
        ../layout_memory.cpp
        ../layout_memory_persister.cpp
        ../flags.cpp
        ../x11_helper.cpp
        ../xkb_helper.cpp
        ../xinput_helper.cpp
        ../layout_tray_icon.cpp
        ../keyboard_config.cpp
        ../xkb_rules.cpp
        ../keyboard_hardware.cpp
        ../bindings.cpp
        ../layouts_menu.cpp
        ../numlockx.c
    )
    target_link_libraries(kcm-keyboard-keyboard_daemon_test
        ${KDE4_KDEUI_LIBS}
        ${KDE4_PLASMA_LIBS}
        ${QT_QTXML_LIBRARY}
        ${QT_QTTEST_LIBRARY}
        ${X11_Xkbfile_LIB}
        ${X11_XTest_LIB}
        ${X11_LIBRARIES}
    )
    if(X11_Xinput_FOUND)
        target_link_libraries(kcm-keyboard-keyboard_daemon_test ${X11_Xinput_LIB})
    endif()
endif(X11_XTest_FOUND)
