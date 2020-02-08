install( FILES tabbox.qml DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/tabbox )

# packages
install( DIRECTORY clients/big_icons    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/tabbox )
install( DIRECTORY clients/compact      DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/tabbox )
install( DIRECTORY clients/informative  DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/tabbox )
install( DIRECTORY clients/present_windows  DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/tabbox )
install( DIRECTORY clients/small_icons  DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/tabbox )
install( DIRECTORY clients/text         DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/tabbox )
install( DIRECTORY clients/thumbnails   DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/tabbox )
install( DIRECTORY clients/window_strip DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/tabbox )

install( DIRECTORY desktops/informative DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/desktoptabbox )
install( DIRECTORY desktops/previews DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/desktoptabbox )

# service files
install( FILES clients/big_icons/metadata.desktop    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin RENAME kwin4_window_switcher_big_icons.desktop )
install( FILES clients/compact/metadata.desktop      DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin RENAME kwin4_window_switcher_compact.desktop )
install( FILES clients/informative/metadata.desktop  DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin RENAME kwin4_window_switcher_informative.desktop )
install( FILES clients/present_windows/metadata.desktop  DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin RENAME kwin4_window_switcher_present_windows.desktop )
install( FILES clients/small_icons/metadata.desktop  DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin RENAME kwin4_window_switcher_small_icons.desktop )
install( FILES clients/text/metadata.desktop         DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin RENAME kwin4_window_switcher_text.desktop )
install( FILES clients/thumbnails/metadata.desktop   DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin RENAME kwin4_window_switcher_thumbnails.desktop )
install( FILES clients/window_strip/metadata.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin RENAME kwin4_window_switcher_window_strip.desktop )

install( FILES desktops/informative/metadata.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin RENAME kwin4_desktop_switcher_informative.desktop )
install( FILES desktops/previews/metadata.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/kwin RENAME kwin4_desktop_switcher_previews.desktop )

install (FILES IconTabBox.qml ShadowedSvgItem.qml DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/tabbox/big_icons/contents/ui)
install (FILES IconTabBox.qml ShadowedSvgItem.qml DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/tabbox/small_icons/contents/ui)
install (FILES ShadowedSvgItem.qml DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/tabbox/)
install (FILES ShadowedSvgItem.qml DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/tabbox/compact/contents/ui)
install (FILES ShadowedSvgItem.qml DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/tabbox/informative/contents/ui)
install (FILES ShadowedSvgItem.qml DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/tabbox/present_windows/contents/ui)
install (FILES ShadowedSvgItem.qml DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/tabbox/thumbnails/contents/ui)
install (FILES ShadowedSvgItem.qml DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/tabbox/text/contents/ui)

install (FILES ShadowedSvgItem.qml DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/desktoptabbox/informative/contents/ui)
install (FILES ShadowedSvgItem.qml DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/desktoptabbox/previews/contents/ui)
