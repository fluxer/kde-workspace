include_directories(
    ../..
)

###### kbookmarkmodeltest ######

kde4_add_test(kbookmarkmodeltest kbookmarkmodeltest.cpp)
target_link_libraries(kbookmarkmodeltest
    kbookmarkmodel_private
    KDE4::kio
    ${QT_QTTEST_LIBRARY}
)
