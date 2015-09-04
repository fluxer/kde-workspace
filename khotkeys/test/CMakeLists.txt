MACRO(KHOTKEYS_UNIT_TESTS)
    FOREACH(_testname ${ARGN})
        kde4_add_test(${_testname} ${_testname}.cpp)
        target_link_libraries(${_testname} khotkeysprivate ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY})
    ENDFOREACH(_testname)
ENDMACRO(KHOTKEYS_UNIT_TESTS)

KHOTKEYS_UNIT_TESTS(
    kdedmodule_daemon_test )

