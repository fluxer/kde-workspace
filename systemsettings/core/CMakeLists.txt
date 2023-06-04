set(systemsettingsview_LIB_SRCS
    MenuItem.cpp
    MenuModel.cpp
    MenuProxyModel.cpp
    ModuleView.cpp
    BaseMode.cpp
    ExternalAppModule.cpp
    externalModule.ui
)

set(systemsettingsview_LIB_HDRS
    ${CMAKE_CURRENT_BINARY_DIR}/systemsettingsview_export.h
    MenuItem.h
    MenuModel.h
    MenuProxyModel.h
    BaseMode.h
    ModuleView.h
)

add_library(systemsettingsview SHARED ${systemsettingsview_LIB_SRCS})

target_link_libraries(systemsettingsview ${KDE4_KDECORE_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KCMUTILS_LIBS})

generate_export_header(systemsettingsview)

install( TARGETS systemsettingsview  ${INSTALL_TARGETS_DEFAULT_ARGS} )
install( FILES ${systemsettingsview_LIB_HDRS} DESTINATION ${KDE4_INCLUDE_INSTALL_DIR}/systemsettingsview COMPONENT Devel)
INSTALL( FILES systemsettingsview.desktop systemsettingsexternalapp.desktop DESTINATION ${KDE4_SERVICETYPES_INSTALL_DIR})
