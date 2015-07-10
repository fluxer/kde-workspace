add_subdirectory(code)

install(DIRECTORY package/
        DESTINATION ${DATA_INSTALL_DIR}/kwin/decorations/kwin4_decoration_qml_plastik)

install(FILES package/metadata.desktop
        DESTINATION ${SERVICES_INSTALL_DIR}/kwin
        RENAME kwin4_decoration_qml_plastik.desktop)
