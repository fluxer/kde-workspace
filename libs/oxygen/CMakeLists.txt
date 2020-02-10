set(oxygenstyle_LIB_SRCS
    oxygenhelper.cpp
    oxygenitemmodel.cpp
    oxygenshadowcache.cpp
    oxygentileset.cpp
    oxygenutil.cpp
)

kde4_add_kcfg_files(oxygenstyle_LIB_SRCS
    oxygenactiveshadowconfiguration.kcfgc
    oxygeninactiveshadowconfiguration.kcfgc
)

add_library(oxygenstyle SHARED ${oxygenstyle_LIB_SRCS})

target_link_libraries(oxygenstyle PUBLIC
    ${KDE4_KDEUI_LIBS}
)

if ( X11_FOUND )
   target_link_libraries(oxygenstyle PRIVATE ${X11_LIBRARIES})
endif( X11_FOUND )

set_target_properties(oxygenstyle PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)
install(TARGETS oxygenstyle ${INSTALL_TARGETS_DEFAULT_ARGS} )

# next target
set(oxygenstyleconfig_LIB_SRCS
    oxygenshadowconfigwidget.cpp
    oxygenshadowconfigurationui.ui
)

kde4_add_kcfg_files(oxygenstyleconfig_LIB_SRCS
    oxygenactiveshadowconfiguration.kcfgc
    oxygeninactiveshadowconfiguration.kcfgc
)

add_library(oxygenstyleconfig SHARED ${oxygenstyleconfig_LIB_SRCS})

target_link_libraries(oxygenstyleconfig PUBLIC
    ${KDE4_KDEUI_LIBS}
    oxygenstyle
)

if ( X11_FOUND )
   target_link_libraries(oxygenstyleconfig PRIVATE ${X11_LIBRARIES})
endif( X11_FOUND )

set_target_properties(oxygenstyleconfig PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)
install(TARGETS oxygenstyleconfig ${INSTALL_TARGETS_DEFAULT_ARGS})

generate_export_header(oxygenstyle BASE_NAME oxygen)

# Note: no headers installed
