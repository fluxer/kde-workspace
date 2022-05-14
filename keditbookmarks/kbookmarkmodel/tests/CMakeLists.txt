include_directories(
    ${KDE4_KIO_INCLUDES}
    ../..
)

###### kbookmarkmodeltest ######

kde4_add_test(kbookmarkmodeltest kbookmarkmodeltest.cpp)
target_link_libraries(kbookmarkmodeltest
    kbookmarkmodel_private
    ${KDE4_KIO_LIBS}
    ${QT_QTTEST_LIBRARY}
)
