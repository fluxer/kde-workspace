########### next target ###############

set(kio_thumbnail_PART_SRCS thumbnail.cpp)

add_executable(kio_thumbnail ${kio_thumbnail_PART_SRCS})

target_link_libraries(kio_thumbnail ${KDE4_KIO_LIBS} ${KDE4_KDEUI_LIBS})

install(TARGETS kio_thumbnail DESTINATION ${KDE4_LIBEXEC_INSTALL_DIR})

########### next target ###############

set(imagethumbnail_PART_SRCS imagecreator.cpp)

kde4_add_kcfg_files(imagethumbnail_PART_SRCS imagecreatorsettings.kcfgc)
kde4_add_plugin(imagethumbnail ${imagethumbnail_PART_SRCS})

target_link_libraries(imagethumbnail ${KDE4_KIO_LIBS} ${KDE4_KEXIV2_LIBS})

install(TARGETS imagethumbnail DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES imagecreatorsettings.kcfg DESTINATION ${KDE4_KCFG_INSTALL_DIR})
install(TARGETS imagethumbnail DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

########### next target ###############

install( FILES directorythumbnail.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

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

if(X11_Xcursor_FOUND)
    set(cursorthumbnail_PART_SRCS cursorcreator.cpp)

    kde4_add_plugin(cursorthumbnail ${cursorthumbnail_PART_SRCS})

    target_link_libraries(cursorthumbnail ${X11_Xcursor_LIB} ${KDE4_KIO_LIBS})

    install(TARGETS cursorthumbnail DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
    install(FILES cursorthumbnail.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
endif()

########### next target ###############

set(windowsexethumbnail_SRCS windowsexecreator.cpp icoutils.cpp)
set(windowsimagethumbnail_SRCS windowsimagecreator.cpp icoutils.cpp)

kde4_add_plugin(windowsexethumbnail ${windowsexethumbnail_SRCS})
target_link_libraries( windowsexethumbnail ${KDE4_KIO_LIBS} )
install(TARGETS windowsexethumbnail DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )

kde4_add_plugin(windowsimagethumbnail ${windowsimagethumbnail_SRCS})
target_link_libraries(windowsimagethumbnail ${KDE4_KIO_LIBS})
install(TARGETS windowsimagethumbnail DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

########### next target ###############

set(comicbookthumbnail_SRCS comiccreator.cpp)
kde4_add_plugin(comicbookthumbnail ${comicbookthumbnail_SRCS})
target_link_libraries(comicbookthumbnail ${KDE4_KIO_LIBS} ${KDE4_KARCHIVE_LIBS})
install(TARGETS comicbookthumbnail DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

########### next target ###############

if(EPUB_FOUND)
    include_directories(${EPUB_INCLUDE_DIR})

    set(epubthumbnail_SRCS epubthumbnail.cpp)

    kde4_add_plugin(epubthumbnail ${epubthumbnail_SRCS})
    target_link_libraries(epubthumbnail ${KDE4_KIO_LIBS} ${EPUB_LIBRARIES})

    install(TARGETS epubthumbnail DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
    install(FILES epubthumbnail.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
endif()

########### install files ###############

install(
    FILES thumbcreator.desktop
    DESTINATION ${KDE4_SERVICETYPES_INSTALL_DIR}
)

install(
    FILES
    thumbnail.protocol
    imagethumbnail.desktop
    textthumbnail.desktop
    djvuthumbnail.desktop
    desktopthumbnail.desktop
    comicbookthumbnail.desktop
    windowsimagethumbnail.desktop
    windowsexethumbnail.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
