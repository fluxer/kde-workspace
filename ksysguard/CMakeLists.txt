add_subdirectory( gui )
add_subdirectory( pics )
add_subdirectory( example )

########### install files ###############

kde4_install_icons( ${KDE4_ICON_INSTALL_DIR}  )

add_subdirectory( ksysguardd )

