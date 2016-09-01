include_directories(
    ${CMAKE_SOURCE_DIR}/libs/oxygen
    ${KDE4_KDEUI_INCLUDES}
)


########### next target ###############
set(oxygen_shadow_demo_SOURCES
    oxygenshadowdemodialog.cpp
    oxygenshadowdemowidget.cpp
    main.cpp
)

add_executable(oxygen-shadow-demo ${oxygen_shadow_demo_SOURCES})

target_link_libraries(oxygen-shadow-demo
    ${KDE4_KDEUI_LIBS}
    ${X11_X11_LIB}
    ${KDE4_KIO_LIBS}
    oxygenstyle
)

########### install files ###############
install(TARGETS oxygen-shadow-demo ${INSTALL_TARGETS_DEFAULT_ARGS})
