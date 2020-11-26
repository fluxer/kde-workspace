if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()

########### next target ###############

set(kcm_icons_PART_SRCS
    iconthemes.cpp
    icons.cpp
    main.cpp
)

kde4_add_plugin(kcm_icons ${kcm_icons_PART_SRCS})

target_link_libraries(kcm_icons ${KDE4_KIO_LIBS})

########### install files ###############

install(
    TARGETS kcm_icons
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES icons.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
