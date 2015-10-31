set(cursors_folders
    Oxygen_Black
    Oxygen_Blue
    Oxygen_White
    KDE_Classic
)

foreach(theme ${cursors_folders})
    install(DIRECTORY ${theme} DESTINATION ${ICON_INSTALL_DIR})
endforeach(theme)

