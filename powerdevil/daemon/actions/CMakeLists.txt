install(FILES powerdevilaction.desktop DESTINATION ${KDE4_SERVICETYPES_INSTALL_DIR})

add_subdirectory(bundled)
if(HAVE_DPMS)
    add_subdirectory(dpms)
endif(HAVE_DPMS)