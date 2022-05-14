include_directories(
    ${CMAKE_SOURCE_DIR}/kate/part
    ${CMAKE_SOURCE_DIR}/kate/part/buffer
    ${CMAKE_SOURCE_DIR}/kate/part/completion
    ${CMAKE_SOURCE_DIR}/kate/part/dialogs
    ${CMAKE_SOURCE_DIR}/kate/part/document
    ${CMAKE_SOURCE_DIR}/kate/part/mode
    ${CMAKE_SOURCE_DIR}/kate/part/render
    ${CMAKE_SOURCE_DIR}/kate/part/search
    ${CMAKE_SOURCE_DIR}/kate/part/syntax
    ${CMAKE_SOURCE_DIR}/kate/part/undo
    ${CMAKE_SOURCE_DIR}/kate/part/utils
    ${CMAKE_SOURCE_DIR}/kate/part/view
    ${CMAKE_SOURCE_DIR}/kate/part/kte5 # KDE5 remove
    ${CMAKE_BINARY_DIR}/kate/part
    ${KDE4_KIO_INCLUDES}
)

set(KATE_TEST_LINK_LIBS
    ${KDE4_KDECORE_LIBS}
    ${QT_QTTEST_LIBRARY}
    ${KDE4_KCMUTILS_LIBS}
    ktexteditor
    katepartinterfaces
)

# buffer test
kde4_add_test(kate-katetextbuffertest katetextbuffertest.cpp katetextbuffertest.h)
target_link_libraries(kate-katetextbuffertest ${KATE_TEST_LINK_LIBS})

########### range test ###############

kde4_add_test(kate-range_test range_test.cpp)

target_link_libraries(kate-range_test
    ${KDE4_KDEUI_LIBS}
    ${QT_QTTEST_LIBRARY}
    ${KATE_TEST_LINK_LIBS}
    katepartinterfaces
)

########### undomanager test ###############

kde4_add_test(kate-undomanager_test undomanager_test.cpp)

target_link_libraries(kate-undomanager_test ${KATE_TEST_LINK_LIBS})

########### plaintextsearch test ###############

kde4_add_test(kate-plaintextsearch_test plaintextsearch_test.cpp)

target_link_libraries(kate-plaintextsearch_test ${KATE_TEST_LINK_LIBS})

########### regexpsearch test ###############

kde4_add_test(kate-regexpsearch_test regexpsearch_test.cpp)

target_link_libraries(kate-regexpsearch_test ${KATE_TEST_LINK_LIBS})

########### completion test ###############

set(completion_test_SRCS
    completion_test.cpp
    codecompletiontestmodel.cpp
)

kde4_add_test(kate-completion_test ${completion_test_SRCS})

target_link_libraries(kate-completion_test ${KATE_TEST_LINK_LIBS})

########### word completion test ###############

kde4_add_test(kate-wordcompletion_test wordcompletiontest.cpp)

target_link_libraries(kate-wordcompletion_test ${KATE_TEST_LINK_LIBS})

########### searchbar test ###############

kde4_add_test(kate-searchbar_test searchbar_test.cpp)

target_link_libraries(kate-searchbar_test
    ${KDE4_KDEUI_LIBS}
    ${QT_QTTEST_LIBRARY}
    ${KATE_TEST_LINK_LIBS}
    katepartinterfaces
)

########### cursor test ###############

kde4_add_test(kate-movingcursor_test movingcursor_test.cpp)

target_link_libraries(kate-movingcursor_test
    ${KDE4_KDEUI_LIBS}
    ${QT_QTTEST_LIBRARY}
    ${KATE_TEST_LINK_LIBS}
    katepartinterfaces
)

########### moving range and feedback test ###############

kde4_add_test(kate-movingrange_test movingrange_test.cpp)

target_link_libraries(kate-movingrange_test
    ${KDE4_KDEUI_LIBS}
    ${QT_QTTEST_LIBRARY}
    ${KATE_TEST_LINK_LIBS}
    katepartinterfaces
)

########### document test ###############

kde4_add_test(kate-katedocument_test katedocument_test.cpp)

target_link_libraries(kate-katedocument_test
    ${KDE4_KDEUI_LIBS}
    ${QT_QTTEST_LIBRARY}
    ${KATE_TEST_LINK_LIBS}
    katepartinterfaces
)

########### view test ###############

kde4_add_test(kate-kateview_test kateview_test.cpp)

target_link_libraries(kate-kateview_test
    ${KDE4_KDEUI_LIBS}
    ${QT_QTTEST_LIBRARY}
    ${KATE_TEST_LINK_LIBS}
    katepartinterfaces
)

########### revision test ###############

kde4_add_test(kate-revision_test revision_test.cpp)

target_link_libraries(kate-revision_test
    ${KDE4_KDEUI_LIBS}
    ${QT_QTTEST_LIBRARY}
    ${KATE_TEST_LINK_LIBS}
    katepartinterfaces
)

########### line modification test ###############

kde4_add_test(kate-modificationsystem_test modificationsystem_test.cpp)

target_link_libraries(kate-modificationsystem_test
    ${KATE_TEST_LINK_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTTEST_LIBRARY}
    ${KATE_TEST_LINK_LIBS}
    katepartinterfaces
)

########### text folding test ###############

kde4_add_test(kate-codefoldingtest katefoldingtest.cpp)

target_link_libraries(kate-codefoldingtest
    ${KDE4_KDEUI_LIBS}
    ${QT_QTTEST_LIBRARY}
    ${KATE_TEST_LINK_LIBS}
    katepartinterfaces
)

########### bug 286887 test ###############

kde4_add_test(kate-bug286887_test bug286887.cpp)
set_tests_properties(kate-bug286887_test PROPERTIES TIMEOUT 4)

target_link_libraries(kate-bug286887_test
    ${KDE4_KDEUI_LIBS}
    ${QT_QTTEST_LIBRARY}
    ${KATE_TEST_LINK_LIBS}
    katepartinterfaces
)

########### bug 313769 test ###############

kde4_add_test(kate-bug313769_test bug313769.cpp)

target_link_libraries(kate-bug313769_test
    ${KDE4_KDEUI_LIBS}
    ${QT_QTTEST_LIBRARY}
    ${KATE_TEST_LINK_LIBS}
    katepartinterfaces
)

########### KTextEditor::DocumentCursor test ###############

kde4_add_test(kate-documentcursor_test kte_documentcursor.cpp)

target_link_libraries(kate-documentcursor_test
    ${KDE4_KDEUI_LIBS}
    ${QT_QTTEST_LIBRARY}
    ${KATE_TEST_LINK_LIBS}
    katepartinterfaces
)

########### KTextEditor::MessageInterface test ###############

kde4_add_test(kate-messageinterface messagetest.cpp)

target_link_libraries(kate-messageinterface
    ${KDE4_KDEUI_LIBS}
    ${QT_QTTEST_LIBRARY}
    ${KATE_TEST_LINK_LIBS}
    katepartinterfaces
)

# encoding tests
add_subdirectory (encoding)
