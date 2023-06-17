include_directories(
    ${CMAKE_SOURCE_DIR}/libs/oxygen
)


########### next target ###############
set(oxygen_shadow_demo_SOURCES
    oxygenshadowdemodialog.cpp
    oxygenshadowdemowidget.cpp
    main.cpp
)

add_executable(oxygen-shadow-demo ${oxygen_shadow_demo_SOURCES})

target_link_libraries(oxygen-shadow-demo
    KDE4::kdeui
    ${X11_X11_LIB}
    KDE4::kio
    oxygenstyle
)

########### install files ###############
install(
    TARGETS oxygen-shadow-demo
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)
