set(ksignalplotter_LIB_SRCS
    ksignalplotter.cpp
    kgraphicssignalplotter.cpp
)

add_library(ksignalplotter SHARED ${ksignalplotter_LIB_SRCS})

target_link_libraries(ksignalplotter PUBLIC ${KDE4_KDEUI_LIBS})

set_target_properties(ksignalplotter
    PROPERTIES VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)

install(
    TARGETS ksignalplotter
    EXPORT kdeworkspaceLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS}
)

#----------------------

kde4_add_widget(ksignalplotterwidgets_SRCS ksignalplotter.widgets)

kde4_add_plugin(ksignalplotterwidgets ${ksignalplotterwidgets_SRCS} ${ksignalplotterwidgets_PART_SRCS})

target_link_libraries(ksignalplotterwidgets ${KDE4_KDEUI_LIBS} ksignalplotter)

install(
    TARGETS ksignalplotterwidgets
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}/kde4/plugins/designer
)

install(
    FILES ksignalplotter.h
    DESTINATION ${KDE4_INCLUDE_INSTALL_DIR}/ksysguard
    COMPONENT Devel
)
