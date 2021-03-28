project(kwalletd)

########### build backends #########
add_subdirectory(backend)

########### kwalletd ###############

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/backend)

add_definitions(-DKDE_DEFAULT_DEBUG_AREA=285)

set(kwalletd_SRCS
   main.cpp
   kbetterthankdialog.cpp
   kwalletd.cpp
   kwalletwizard.cpp
   ktimeout.cpp
   kwalletsessionstore.cpp
   kbetterthankdialogbase.ui
   kwalletwizardpageexplanation.ui
   kwalletwizardpageintro.ui
   kwalletwizardpageoptions.ui
   kwalletwizardpagepassword.ui
)

find_file(kwallet_xml org.kde.KWallet.xml HINTS ${KDE4_DBUS_INTERFACES_INSTALL_DIR} )

qt4_add_dbus_adaptor( kwalletd_SRCS ${kwallet_xml} kwalletd.h KWalletD )

add_executable( kwalletd ${kwalletd_SRCS} )

target_link_libraries(kwalletd ${KDE4_KDEUI_LIBS} kwalletbackend )

install(TARGETS kwalletd  ${INSTALL_TARGETS_DEFAULT_ARGS} )

########### install files ###############

install( FILES kwalletd.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )
install( FILES kwalletd.notifyrc DESTINATION  ${KDE4_DATA_INSTALL_DIR}/kwalletd )

if(ENABLE_TESTING)
    add_subdirectory(tests)
    add_subdirectory(autotests)
endif()
