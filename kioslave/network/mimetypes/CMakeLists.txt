# shared-mime-info 0.40 is mandatory for generic-icon
find_package(SharedMimeInfo 0.40 REQUIRED)

install( FILES network.xml  DESTINATION ${KDE4_XDG_MIME_INSTALL_DIR} )
update_xdg_mimetypes( ${KDE4_XDG_MIME_INSTALL_DIR} )
