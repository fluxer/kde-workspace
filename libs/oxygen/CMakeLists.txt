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
    KDE4::kdeui
)
target_link_libraries(oxygenstyle PRIVATE ${X11_LIBRARIES})

set_target_properties(oxygenstyle PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)

install(
    TARGETS oxygenstyle
    DESTINATION ${KDE4_LIB_INSTALL_DIR}
)

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
    KDE4::kdeui
    oxygenstyle
)
target_link_libraries(oxygenstyleconfig PRIVATE ${X11_LIBRARIES})

set_target_properties(oxygenstyleconfig PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)

install(
    TARGETS oxygenstyleconfig
    DESTINATION ${KDE4_LIB_INSTALL_DIR}
)

generate_export_header(oxygenstyle BASE_NAME oxygen)

# Note: no headers installed
