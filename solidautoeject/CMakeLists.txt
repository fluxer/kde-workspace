########### next target ###############

set(kded_solidautoeject_SRCS
   solidautoeject.cpp
)

kde4_add_plugin(kded_solidautoeject ${kded_solidautoeject_SRCS})

target_link_libraries(kded_solidautoeject ${KDE4_SOLID_LIBS} ${KDE4_KDECORE_LIBS})

install(TARGETS kded_solidautoeject DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})


########### install files ###############

install(FILES solidautoeject.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kded)

