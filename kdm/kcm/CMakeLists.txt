set(kcm_kdm_PART_SRCS
   kdm-gen.cpp
   kdm-theme.cpp
   kdm-shut.cpp
   kdm-users.cpp
   kdm-conv.cpp
   main.cpp
   kbackedcombobox.cpp
   positioner.cpp)


kde4_add_plugin(kcm_kdm ${kcm_kdm_PART_SRCS})


target_link_libraries(kcm_kdm ${KDE4_KIO_LIBS} ${X11_LIBRARIES} kworkspace)

macro_add_file_dependencies(kdm-shut.cpp ${confci})
add_dependencies(kcm_kdm ConfigCi)

install(TARGETS kcm_kdm  DESTINATION ${PLUGIN_INSTALL_DIR})

#### KAuth helper and actions ####
add_executable(kcmkdmhelper helper.cpp)
target_link_libraries(kcmkdmhelper ${KDE4_KDECORE_LIBS})
install(TARGETS kcmkdmhelper DESTINATION ${LIBEXEC_INSTALL_DIR})

kde4_install_auth_helper_files(kcmkdmhelper org.kde.kcontrol.kcmkdm root)

########### install files ###############

install( FILES kdm.desktop  DESTINATION  ${SERVICES_INSTALL_DIR} )

install( FILES anchor.png  DESTINATION  ${DATA_INSTALL_DIR}/kcontrol/pics )
install( FILES kdm.knsrc DESTINATION ${CONFIG_INSTALL_DIR} )
