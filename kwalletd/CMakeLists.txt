project(kwalletd)

########### find needed packages ######
find_package(Gpgme)  # Called by FindQGpgme, but since we call some gpgme
                     # functions ourselves we need to link against it directly.
find_package(QGpgme) # provided by kdepimlibs

if (GPGME_FOUND AND QGPGME_FOUND)
    add_definitions(-DHAVE_QGPGME)
    include_directories(${GPGME_INCLUDES} ${QGPGME_INCLUDE_DIR})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")
endif(GPGME_FOUND AND QGPGME_FOUND)

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

if (GPGME_FOUND AND QGPGME_FOUND)
    set(kwalletd_SRCS
        ${kwalletd_SRCS}
        knewwalletdialog.cpp
    )
    kde4_add_ui_files(kwalletd_SRCS
        kwalletwizardpagepasswordgpg.ui
        kwalletwizardpagegpgkey.ui
        knewwalletdialogintro.ui
        knewwalletdialoggpg.ui
    )
endif(GPGME_FOUND AND QGPGME_FOUND)

find_file(kwallet_xml org.kde.KWallet.xml HINTS ${KDE4_DBUS_INTERFACES_DIR} )

qt4_add_dbus_adaptor( kwalletd_SRCS ${kwallet_xml} kwalletd.h KWalletD )

add_executable( kwalletd ${kwalletd_SRCS} )

target_link_libraries(kwalletd ${KDE4_KDEUI_LIBS} kwalletbackend )
if (GPGME_FOUND AND QGPGME_FOUND)
    target_link_libraries(kwalletd ${QGPGME_LIBRARIES} )
endif(GPGME_FOUND AND QGPGME_FOUND)

install(TARGETS kwalletd  ${INSTALL_TARGETS_DEFAULT_ARGS} )

########### install files ###############

install( FILES kwalletd.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )
install( FILES kwalletd.notifyrc DESTINATION  ${DATA_INSTALL_DIR}/kwalletd )

if(ENABLE_TESTING)
    add_subdirectory(tests)
    add_subdirectory(autotests)
endif()
