
# OS Base includes
include_directories( 
    ${CMAKE_CURRENT_SOURCE_DIR}/../base
)

# FreeBSD devinfo library
find_library(DEVINFO_LIBRARY NAMES devinfo)

########### next target ###############

set(kcm_info_PART_SRCS main.cpp info.cpp ../base/os_current.cpp)

kde4_add_plugin(kcm_info ${kcm_info_PART_SRCS})

target_link_libraries(kcm_info KDE4::kdeui ${QT_QTGUI_LIBRARY} ${X11_X11_LIB})

if(DEVINFO_LIBRARY)
    target_link_libraries(kcm_info ${DEVINFO_LIBRARY})
endif()

install(TARGETS kcm_info  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

########### install files ###############

install(
    FILES 
    dma.desktop
    interrupts.desktop
    ioports.desktop
    scsi.desktop
    xserver.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
