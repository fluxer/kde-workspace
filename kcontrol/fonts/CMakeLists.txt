if(FONTCONFIG_FOUND)
    include_directories(${FONTCONFIG_INCLUDE_DIR})
endif(FONTCONFIG_FOUND)
include_directories(${FREETYPE_INCLUDE_DIRS})

########### next target ###############

set(kcm_fonts_PART_SRCS
    ../krdb/krdb.cpp
    fonts.cpp
)

if(Q_WS_X11)
    set(kcm_fonts_PART_SRCS
        ${kcm_fonts_PART_SRCS}
        ${libkxftconfig_SRCS}
    )
endif(Q_WS_X11)

kde4_add_plugin(kcm_fonts ${kcm_fonts_PART_SRCS})

target_link_libraries(kcm_fonts
    ${KDE4_KDEUI_LIBS}
    ${FREETYPE_LIBRARIES}
    ${QT_QTXML_LIBRARY}
)

if(Q_WS_X11)
    if(FONTCONFIG_FOUND)
        target_link_libraries(kcm_fonts ${FONTCONFIG_LIBRARIES})
    endif(FONTCONFIG_FOUND)

    target_link_libraries(kcm_fonts ${X11_LIBRARIES})
endif(Q_WS_X11)

########### install files ###############

install(
    TARGETS kcm_fonts
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES fonts.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
