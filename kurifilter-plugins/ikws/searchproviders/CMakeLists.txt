
########### install files ###############

FILE(GLOB providersfile *.desktop)

install(
    FILES ${providersfile}
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}/searchproviders
)
