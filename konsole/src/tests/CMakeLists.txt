include_directories(
    ${KDE4_KPTY_INCLUDES}
    ${KDE4_KPARTS_INCLUDES}
    ${QT_QTTEST_INCLUDES}
    # for konsoleprivate_export.h
    ${CMAKE_CURRENT_BINARY_DIR}/..
)

set(KONSOLE_TEST_LIBS
    ${QT_QTTEST_LIBRARY}
    ${KDE4_KPARTS_LIBS}
    ${KDE4_KPTY_LIBS}
    konsoleprivate
)

kde4_add_manual_test(PartManualTest PartManualTest.cpp)
target_link_libraries(PartManualTest
    ${KONSOLE_TEST_LIBS}
)

kde4_add_test(konsole-CharacterColorTest CharacterColorTest.cpp)
target_link_libraries(konsole-CharacterColorTest ${KONSOLE_TEST_LIBS})

kde4_add_test(konsole-CharacterWidthTest CharacterWidthTest.cpp)
target_link_libraries(konsole-CharacterWidthTest ${KONSOLE_TEST_LIBS})

kde4_add_test(konsole-DBusTest DBusTest.cpp)
target_link_libraries(konsole-DBusTest ${KONSOLE_TEST_LIBS})

set(HistoryTest_SRCS
    HistoryTest.cpp
    ../History.cpp
)
kde4_add_test(konsole-HistoryTest ${HistoryTest_SRCS})
set_target_properties(konsole-HistoryTest PROPERTIES
    COMPILE_FLAGS -DKONSOLEPRIVATE_EXPORT=
)
target_link_libraries(konsole-HistoryTest ${KONSOLE_TEST_LIBS})

set(KeyboardTranslatorTest_SRCS
    KeyboardTranslatorTest.cpp
    ../KeyboardTranslator.cpp
)
kde4_add_test(konsole-KeyboardTranslatorTest ${KeyboardTranslatorTest_SRCS})
set_target_properties(konsole-KeyboardTranslatorTest PROPERTIES
    COMPILE_FLAGS -DKONSOLEPRIVATE_EXPORT=
)
target_link_libraries(konsole-KeyboardTranslatorTest ${KONSOLE_TEST_LIBS})

kde4_add_test(konsole-PartTest PartTest.cpp)
target_link_libraries(konsole-PartTest ${KONSOLE_TEST_LIBS})

kde4_add_test(konsole-ProfileTest ProfileTest.cpp)
target_link_libraries(konsole-ProfileTest ${KONSOLE_TEST_LIBS})

kde4_add_test(konsole-PtyTest PtyTest.cpp)
target_link_libraries(konsole-PtyTest ${KONSOLE_TEST_LIBS})

kde4_add_test(konsole-SessionTest SessionTest.cpp)
target_link_libraries(konsole-SessionTest ${KONSOLE_TEST_LIBS})

##kde4_add_test(konsole-SessionManagerTest SessionManagerTest.cpp)
##target_link_libraries(konsole-SessionManagerTest ${KONSOLE_TEST_LIBS})

kde4_add_test(konsole-ShellCommandTest ShellCommandTest.cpp)
target_link_libraries(konsole-ShellCommandTest ${KONSOLE_TEST_LIBS})

kde4_add_test(konsole-TerminalCharacterDecoderTest TerminalCharacterDecoderTest.cpp)
target_link_libraries(konsole-TerminalCharacterDecoderTest ${KONSOLE_TEST_LIBS})

kde4_add_test(konsole-TerminalTest TerminalTest.cpp)
target_link_libraries(konsole-TerminalTest ${KONSOLE_TEST_LIBS})

kde4_add_test(konsole-TerminalInterfaceTest TerminalInterfaceTest.cpp)
target_link_libraries(konsole-TerminalInterfaceTest ${KONSOLE_TEST_LIBS})
