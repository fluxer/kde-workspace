
add_subdirectory( Applications ) 
add_subdirectory( Manpages ) 
add_subdirectory( Scrollkeeper ) 


########### install files ###############

install( FILES fundamentals.desktop plasma.desktop 
               onlinehelp.desktop 
               info.desktop kicmodules.desktop 
               kcontrolmodules.desktop 
               konquerorcontrolmodules.desktop
               browsercontrolmodules.desktop 
               filemanagercontrolmodules.desktop
               othercontrolmodules.desktop 
               kioslaves.desktop  DESTINATION  ${DATA_INSTALL_DIR}/khelpcenter/plugins )

