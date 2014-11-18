add_subdirectory( gui )
add_subdirectory( pics )
add_subdirectory( example )

########### install files ###############

kde4_install_icons( ${ICON_INSTALL_DIR}  )

add_subdirectory( ksysguardd )

