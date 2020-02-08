
set( themes_RC
    qtcde.themerc
    qtcleanlooks.themerc
    qtmotif.themerc
    qtplastique.themerc
    qtwindows.themerc
)

if (Q_WS_X11)
  set( themes_RC ${themes_RC}
      qtgtk.themerc
  )
endif (Q_WS_X11)

if (APPLE)
  set( themes_RC ${themes_RC}
      qtmacintosh.themerc
  )
endif (APPLE)


########### install files ###############

install( FILES ${themes_RC}  DESTINATION  ${KDE4_DATA_INSTALL_DIR}/kstyle/themes )




