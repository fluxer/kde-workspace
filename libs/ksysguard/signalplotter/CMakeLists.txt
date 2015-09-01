set(ksignalplotter_LIB_SRCS
    ksignalplotter.cpp
    kgraphicssignalplotter.cpp
)

add_library(ksignalplotter SHARED ${ksignalplotter_LIB_SRCS})

target_link_libraries(ksignalplotter PRIVATE ${KDE4_KDEUI_LIBS})
target_link_libraries(ksignalplotter PUBLIC ${KDE4_KDEUI_LIBS})

set_target_properties(ksignalplotter
   PROPERTIES VERSION ${GENERIC_LIB_VERSION}
   SOVERSION ${GENERIC_LIB_SOVERSION}
   )

install(TARGETS ksignalplotter EXPORT kdeworkspaceLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS} )

#----------------------

kde4_add_widget_files(ksignalplotter.widgets)

kde4_add_plugin(ksignalplotterwidgets $<TARGET_OBJECTS:ksignalplotter_autowidgets> ${ksignalplotterwidgets_PART_SRCS})

target_link_libraries(ksignalplotterwidgets ${KDE4_KDEUI_LIBS} ksignalplotter)

install(TARGETS ksignalplotterwidgets DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/designer)

install(FILES ksignalplotter.h DESTINATION ${INCLUDE_INSTALL_DIR}/ksysguard COMPONENT Devel)
