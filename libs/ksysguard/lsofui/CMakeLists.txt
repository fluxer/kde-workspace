
########### next target ###############

set(lsofui_LIB_SRCS
   lsof.cpp
   LsofSearchWidget.ui 
)

add_library(lsofui SHARED ${lsofui_LIB_SRCS})
target_link_libraries(lsofui KDE4::kio)

set_target_properties(lsofui PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)

install(
    TARGETS lsofui
    EXPORT kdeworkspaceTargets
    DESTINATION ${KDE4_LIB_INSTALL_DIR}
)

########### install files ###############

install(
    FILES lsof.h
    DESTINATION ${KDE4_INCLUDE_INSTALL_DIR}/ksysguard
)

kde4_add_widget(ksysguardlsofwidgets_SRCS ksysguardlsof.widgets)

kde4_add_plugin(ksysguardlsofwidgets ${ksysguardlsofwidgets_SRCS})

target_link_libraries(ksysguardlsofwidgets
    KDE4::kdecore
    ${QT_QTGUI_LIBRARY}
    lsofui
)

install(
    TARGETS ksysguardlsofwidgets
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}/plugins/designer
)
