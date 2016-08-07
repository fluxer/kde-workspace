########### install files ###############

install(
    FILES
    compiz-custom.desktop
    compiz.desktop
    marco.desktop
    metacity.desktop
    openbox.desktop
    DESTINATION ${DATA_INSTALL_DIR}/ksmserver/windowmanagers
)

if(OPENGLES_FOUND)
    install(
        FILES
        kwin_gles.desktop
        DESTINATION ${DATA_INSTALL_DIR}/ksmserver/windowmanagers
    )
endif()
