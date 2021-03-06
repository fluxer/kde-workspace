########### next target ###############

set(kio_thumbnail_PART_SRCS thumbnail.cpp imagefilter.cpp)

kde4_add_plugin(kio_thumbnail ${kio_thumbnail_PART_SRCS})

target_link_libraries(kio_thumbnail ${KDE4_KIO_LIBS})

install(TARGETS kio_thumbnail DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

########### next target ###############

set(imagethumbnail_PART_SRCS imagecreator.cpp)

set_source_files_properties(${imagethumbnail_PART_SRCS} PROPERTIES
    COMPILE_FLAGS "-DHAVE_KEXIV2 ${KDE4_ENABLE_EXCEPTIONS}"
)

kde4_add_kcfg_files(imagethumbnail_PART_SRCS imagecreatorsettings.kcfgc)
kde4_add_plugin(imagethumbnail ${imagethumbnail_PART_SRCS})

target_link_libraries(imagethumbnail ${KDE4_KIO_LIBS} ${KDE4_KEXIV2_LIBS})

install(TARGETS imagethumbnail DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES imagecreatorsettings.kcfg DESTINATION ${KDE4_KCFG_INSTALL_DIR})
install(TARGETS imagethumbnail DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

########### next target ###############

install( FILES directorythumbnail.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

########### next target ###############

set(svgthumbnail_PART_SRCS svgcreator.cpp)

kde4_add_plugin(svgthumbnail ${svgthumbnail_PART_SRCS})

target_link_libraries(svgthumbnail ${KDE4_KIO_LIBS})

install(TARGETS svgthumbnail DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

########### next target ###############

set(textthumbnail_PART_SRCS textcreator.cpp)

kde4_add_plugin(textthumbnail ${textthumbnail_PART_SRCS})

target_link_libraries(textthumbnail ${KDE4_KIO_LIBS})

install(TARGETS textthumbnail DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

########### next target ###############

set(djvuthumbnail_PART_SRCS djvucreator.cpp)

kde4_add_plugin(djvuthumbnail ${djvuthumbnail_PART_SRCS})

target_link_libraries(djvuthumbnail ${KDE4_KIO_LIBS})

install(TARGETS djvuthumbnail DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

########### next target ###############

if(OPENEXR_FOUND)
   include_directories(${OPENEXR_INCLUDE_DIR})

   set(exrthumbnail_PART_SRCS exrcreator.cpp)

   kde4_add_plugin(exrthumbnail ${exrthumbnail_PART_SRCS})

   target_link_libraries(exrthumbnail ${KDE4_KIO_LIBS} ${OPENEXR_LIBRARIES})

   # OpenEXR headers use exceptions; at least clang refuses to build the target
   # when exceptions are not enabled.
   set_target_properties(exrthumbnail PROPERTIES COMPILE_FLAGS "${KDE4_ENABLE_EXCEPTIONS}")

   install(TARGETS exrthumbnail DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
   install(FILES exrthumbnail.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
endif()

########### next target ###############

if(X11_Xcursor_FOUND)
   set(cursorthumbnail_PART_SRCS cursorcreator.cpp)

   kde4_add_plugin(cursorthumbnail ${cursorthumbnail_PART_SRCS})

   target_link_libraries(cursorthumbnail ${X11_Xcursor_LIB} ${KDE4_KIO_LIBS})

   install(TARGETS cursorthumbnail DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
   install(FILES cursorthumbnail.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
endif()

########### next target ###############

set(windowsexethumbnail_SRCS windowsexecreator.cpp icoutils_common.cpp icoutils_wrestool.cpp)
set(windowsimagethumbnail_SRCS windowsimagecreator.cpp icoutils_common.cpp icoutils_wrestool.cpp)

kde4_add_plugin(windowsexethumbnail ${windowsexethumbnail_SRCS})
target_link_libraries( windowsexethumbnail ${KDE4_KIO_LIBS} )
install(TARGETS windowsexethumbnail DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )

kde4_add_plugin(windowsimagethumbnail ${windowsimagethumbnail_SRCS})
target_link_libraries(windowsimagethumbnail ${KDE4_KIO_LIBS})
install(TARGETS windowsimagethumbnail DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

########### next target ###############

set(comicbookthumbnail_SRCS comiccreator.cpp)
kde4_add_plugin(comicbookthumbnail ${comicbookthumbnail_SRCS})
target_link_libraries(comicbookthumbnail ${KDE4_KIO_LIBS})
install(TARGETS comicbookthumbnail DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

########### next target ###############

set(epubthumbnail_SRCS epub.cpp epubthumbnail.cpp)
kde4_add_plugin(epubthumbnail ${epubthumbnail_SRCS})
target_link_libraries(epubthumbnail ${KDE4_KIO_LIBS})
install(TARGETS epubthumbnail DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

########### install files ###############

install(
    FILES thumbcreator.desktop
    DESTINATION ${KDE4_SERVICETYPES_INSTALL_DIR}
)

install(
    FILES
    thumbnail.protocol
    svgthumbnail.desktop
    imagethumbnail.desktop
    textthumbnail.desktop
    djvuthumbnail.desktop
    desktopthumbnail.desktop
    comicbookthumbnail.desktop
    epubthumbnail.desktop
    windowsimagethumbnail.desktop
    windowsexethumbnail.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
