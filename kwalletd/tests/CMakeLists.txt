# Tests don't need to go into toplevel/bin, they are fine in the current dir.
include_directories(
   ${CMAKE_CURRENT_SOURCE_DIR}/..
)

set(kwalletwizardtest_SRCS
   kwalletwizardtest.cpp
   ../kwalletwizard.cpp
   ../kbetterthankdialogbase.ui
   ../kwalletwizardpageexplanation.ui
   ../kwalletwizardpageintro.ui
   ../kwalletwizardpageoptions.ui
   ../kwalletwizardpagepassword.ui
   ../kwalletwizardpagegpgkey.ui
)

kde4_add_manual_test(kwalletwizardtest ${kwalletwizardtest_SRCS})
target_link_libraries(kwalletwizardtest ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${QT_QTGUI_LIBRARY})
if (QGPGME_FOUND)
    target_link_libraries(kwalletwizardtest ${QGPGME_LIBRARIES})
endif(QGPGME_FOUND)

include_directories( ${KDE4_KDEUI_INCLUDES} ) # for kwallet.h

########### next target ###############

set(kwalletasync_SRCS kwalletasync.cpp kwallettest.cpp )


kde4_add_manual_test(kwalletasync ${kwalletasync_SRCS})

target_link_libraries(kwalletasync ${KDE4_KDEUI_LIBS} )


########### next target ###############

set(kwalletsync_SRCS kwalletsync.cpp )


kde4_add_manual_test(kwalletsync ${kwalletsync_SRCS})

target_link_libraries(kwalletsync  ${KDE4_KDEUI_LIBS} )


########### next target ###############

set(kwalletpath_SRCS kwalletpath.cpp )


kde4_add_manual_test(kwalletpath ${kwalletpath_SRCS})

target_link_libraries(kwalletpath  ${KDE4_KDEUI_LIBS} )


########### next target ###############

set(kwalletboth_SRCS kwalletboth.cpp kwallettest.cpp )


kde4_add_manual_test(kwalletboth ${kwalletboth_SRCS})

target_link_libraries(kwalletboth  ${KDE4_KDEUI_LIBS} )


########### next target ###############

set(kwalletmany_SRCS kwalletmany.cpp)


kde4_add_manual_test(kwalletmany ${kwalletmany_SRCS})

target_link_libraries(kwalletmany  ${KDE4_KDEUI_LIBS} )


########### next target ###############

set(kwalletautoclose_SRCS kwalletautoclose.cpp)


kde4_add_manual_test(kwalletautoclose ${kwalletautoclose_SRCS})

target_link_libraries(kwalletautoclose  ${KDE4_KDEUI_LIBS} )


########### next target ###############

set(kwalletnoautoclose_SRCS kwalletnoautoclose.cpp)


kde4_add_manual_test(kwalletnoautoclose ${kwalletnoautoclose_SRCS})

target_link_libraries(kwalletnoautoclose  ${KDE4_KDEUI_LIBS} )


########### install files ###############

