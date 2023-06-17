
# OS Base includes
include_directories( 
    ${CMAKE_CURRENT_SOURCE_DIR}/../base
)

########### next target ###############

if(PCIUTILS_FOUND)
    MESSAGE(STATUS "Enabling PCI module based on pciutils library")

    add_definitions( -DHAVE_PCIUTILS -fpermissive)
    include_directories(${PCIUTILS_INCLUDE_DIR})

    set(KCM_PCI_PART_SRCS kcm_pci.cpp ../base/os_current.cpp kpci.cpp )

    kde4_add_plugin(kcm_pci ${KCM_PCI_PART_SRCS})

    target_link_libraries(kcm_pci KDE4::kdeui ${QT_QTGUI_LIBRARY} ${PCIUTILS_LIBRARIES})

    
else(PCIUTILS_FOUND)

    set(KCM_PCI_PART_SRCS kcm_pci.cpp ../base/os_current.cpp )

    kde4_add_plugin(kcm_pci ${KCM_PCI_PART_SRCS})
    
    target_link_libraries(kcm_pci  KDE4::kdeui ${QT_QTGUI_LIBRARY} )
endif(PCIUTILS_FOUND)

install(TARGETS kcm_pci  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES kcm_pci.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR})
