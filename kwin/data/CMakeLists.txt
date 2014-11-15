
set(kwin_xml  ${CMAKE_CURRENT_SOURCE_DIR}/../org.kde.KWin.xml)

########### install files ###############

install( FILES pop.wav  DESTINATION  ${SOUND_INSTALL_DIR} )
install( FILES stripTitle.js  DESTINATION  ${DATA_INSTALL_DIR}/kwin )

