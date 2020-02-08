set(cursors_folders
    Oxygen_Black
    Oxygen_Blue
    Oxygen_White
    KDE_Classic
)

foreach(theme ${cursors_folders})
    install(
        DIRECTORY ${theme}
        DESTINATION ${KDE4_ICON_INSTALL_DIR}
    )
endforeach()
