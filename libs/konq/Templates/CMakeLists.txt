########### install files ###############

install( FILES
    linkProgram.desktop
    linkURL.desktop
    linkPath.desktop
    linkHD.desktop
    linkCDROM.desktop
    Directory.desktop
    TextFile.desktop
    HTMLFile.desktop
    linkDVDROM.desktop
    linkCAMERA.desktop
    linkCDWRITER.desktop
    DESTINATION ${KDE4_TEMPLATES_INSTALL_DIR}
)


install(
    FILES
    Program.desktop
    URL.desktop
    HD.desktop
    CDROM-Device.desktop
    TextFile.txt
    HTMLFile.html
    DVDROM-Device.desktop
    CAMERA-Device.desktop
    CDWRITER-Device.desktop
    DESTINATION ${KDE4_TEMPLATES_INSTALL_DIR}/.source
)
