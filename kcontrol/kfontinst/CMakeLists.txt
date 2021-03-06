if (X11_Xft_FOUND)
    check_include_files(locale.h HAVE_LOCALE_H)
    configure_file(config-fontinst.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-fontinst.h)
    include_directories(
        ${KDE4_INCLUDES}
        ${FREETYPE_INCLUDE_DIRS}
        ${FONTCONFIG_INCLUDE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/lib
        ${CMAKE_CURRENT_BINARY_DIR}/lib
        ${CMAKE_CURRENT_SOURCE_DIR}/dbus
        ${CMAKE_CURRENT_BINARY_DIR}/dbus
        ${CMAKE_CURRENT_SOURCE_DIR}/viewpart
        ${CMAKE_CURRENT_SOURCE_DIR}/kcmfontinst
        ${CMAKE_CURRENT_BINARY_DIR}/kcmfontinst)
    add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})

    set(libkfontinstdbusiface_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/dbus/FontinstIface.cpp)
    set(libkfontinstview_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/viewpart/FontPreview.cpp
                              ${CMAKE_CURRENT_SOURCE_DIR}/viewpart/PreviewSelectAction.cpp
                              ${CMAKE_CURRENT_SOURCE_DIR}/viewpart/CharTip.cpp )
    set(libkfontinstactionlabel_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/kcmfontinst/ActionLabel.cpp)
    set(libkfontinstjobrunner_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/kcmfontinst/JobRunner.cpp
                                   ${libkfontinstactionlabel_SRCS}
                                   ${CMAKE_CURRENT_SOURCE_DIR}/kcmfontinst/FontsPackage.cpp)

    add_subdirectory( lib ) 
    add_subdirectory( dbus )
    add_subdirectory( kcmfontinst )
    if (STRIGI_FOUND)
        add_subdirectory( strigi-analyzer )
    endif()
    add_subdirectory( apps )
    add_subdirectory( kio ) 
    add_subdirectory( thumbnail )
    add_subdirectory( viewpart )

    kde4_install_icons( ${KDE4_ICON_INSTALL_DIR} )

endif (X11_Xft_FOUND)
