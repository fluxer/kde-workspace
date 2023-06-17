include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_BUILD_DIR}/..
    ${KDE4_INCLUDES}
)

# KItemSetTest
set(kitemsettest_SRCS
    kitemsettest.cpp
    ../kitemviews/kitemset.cpp
)
kde4_add_test(dolphin-kitemsettest ${kitemsettest_SRCS})
target_link_libraries(dolphin-kitemsettest
    dolphinprivate
    KDE4::kio
    ${QT_QTTEST_LIBRARY}
)

# KItemRangeTest
kde4_add_test(dolphin-kitemrangetest kitemrangetest.cpp)
target_link_libraries(dolphin-kitemrangetest
    dolphinprivate
    KDE4::kio
    ${QT_QTTEST_LIBRARY}
)

# KItemListSelectionManagerTest
set(kitemlistselectionmanagertest_SRCS
    kitemlistselectionmanagertest.cpp
    ../kitemviews/kitemlistselectionmanager.cpp
    ../kitemviews/kitemmodelbase.cpp
    ../kitemviews/kitemset.cpp
)
kde4_add_test(dolphin-kitemlistselectionmanagertest ${kitemlistselectionmanagertest_SRCS})
target_link_libraries(dolphin-kitemlistselectionmanagertest
    dolphinprivate
    KDE4::kio
    ${QT_QTTEST_LIBRARY}
)

# KItemListControllerTest
set(kitemlistcontrollertest_SRCS
    kitemlistcontrollertest.cpp
    testdir.cpp
    ../kitemviews/kfileitemmodel.cpp
    ../kitemviews/kfileitemlistview.cpp
    ../kitemviews/kfileitemlistwidget.cpp
    ../kitemviews/kitemmodelbase.cpp
    ../kitemviews/kitemlistview.cpp
    ../kitemviews/kitemlistcontainer.cpp
    ../kitemviews/kitemlistwidget.cpp
    ../kitemviews/kitemset.cpp
    ../kitemviews/kstandarditemlistview.cpp
    ../kitemviews/kstandarditemlistwidget.cpp
)
kde4_add_test(dolphin-kitemlistcontrollertest ${kitemlistcontrollertest_SRCS})
target_link_libraries(dolphin-kitemlistcontrollertest
    dolphinprivate
    KDE4::kio
    ${QT_QTTEST_LIBRARY}
)

# KFileItemListViewTest
set(kfileitemlistviewtest_SRCS
    kfileitemlistviewtest.cpp
    testdir.cpp
    ../kitemviews/kfileitemmodel.cpp
    ../kitemviews/kfileitemlistview.cpp
    ../kitemviews/kitemmodelbase.cpp
    ../kitemviews/kitemlistview.cpp
    ../kitemviews/kitemlistcontainer.cpp
    ../kitemviews/kitemlistwidget.cpp
    ../kitemviews/kitemset.cpp
    ../kitemviews/kstandarditemlistview.cpp
    ../kitemviews/kstandarditemlistwidget.cpp
)
kde4_add_test(dolphin-kfileitemlistviewtest ${kfileitemlistviewtest_SRCS})
target_link_libraries(dolphin-kfileitemlistviewtest
    dolphinprivate
    KDE4::kio
    ${QT_QTTEST_LIBRARY}
)

# KFileItemModelTest
set(kfileitemmodeltest_SRCS
    kfileitemmodeltest.cpp
    testdir.cpp
    ../kitemviews/kfileitemmodel.cpp
    ../kitemviews/kitemmodelbase.cpp
    ../kitemviews/kitemset.cpp
)
kde4_add_test(dolphin-kfileitemmodeltest ${kfileitemmodeltest_SRCS})
target_link_libraries(dolphin-kfileitemmodeltest
    dolphinprivate
    KDE4::kio
    ${QT_QTTEST_LIBRARY}
)

# KFileItemModelBenchmark
set(kfileitemmodelbenchmark_SRCS
    kfileitemmodelbenchmark.cpp
    testdir.cpp
    ../kitemviews/kfileitemmodel.cpp
    ../kitemviews/kitemmodelbase.cpp
)
kde4_add_manual_test(dolphin-kfileitemmodelbenchmark ${kfileitemmodelbenchmark_SRCS})
target_link_libraries(dolphin-kfileitemmodelbenchmark
    dolphinprivate
    KDE4::kio
    ${QT_QTTEST_LIBRARY}
)

# KItemListKeyboardSearchManagerTest
set(kitemlistkeyboardsearchmanagertest_SRCS
    kitemlistkeyboardsearchmanagertest.cpp
    ../kitemviews/private/kitemlistkeyboardsearchmanager.cpp
)
kde4_add_test(dolphin-kitemlistkeyboardsearchmanagertest ${kitemlistkeyboardsearchmanagertest_SRCS})
target_link_libraries(dolphin-kitemlistkeyboardsearchmanagertest
    KDE4::kio
    ${QT_QTTEST_LIBRARY}
)

# KStandardItemModelTest
set(kstandarditemmodeltest_SRCS
    kstandarditemmodeltest.cpp
    ../kitemviews/kstandarditem.cpp
    ../kitemviews/kstandarditemmodel.cpp
    ../kitemviews/kitemmodelbase.cpp
)
kde4_add_test(dolphin-kstandarditemmodeltest ${kstandarditemmodeltest_SRCS})
target_link_libraries(dolphin-kstandarditemmodeltest
    dolphinprivate
    KDE4::kio
    ${QT_QTTEST_LIBRARY}
)

# ViewPropertiesTest
set(viewpropertiestest_SRCS
    viewpropertiestest.cpp
    testdir.cpp
    ../views/viewproperties.cpp
)
kde4_add_kcfg_files(viewpropertiestest_SRCS
  ../settings/dolphin_generalsettings.kcfgc
  ../settings/dolphin_directoryviewpropertysettings.kcfgc
)
kde4_add_test(dolphin-viewpropertiestest ${viewpropertiestest_SRCS})
target_link_libraries(dolphin-viewpropertiestest
    dolphinprivate
    KDE4::kio
    ${QT_QTTEST_LIBRARY}
)
