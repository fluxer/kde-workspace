if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()

set(libfiletypes_SRCS
    filetypedetails.cpp
    filegroupdetails.cpp
    kservicelistwidget.cpp
    typeslistitem.cpp
    mimetypedata.cpp
    mimetypewriter.cpp
    newtypedlg.cpp
    kserviceselectdlg.cpp
)

########### next target ###############

set(kcm_filetypes_SRCS filetypesview.cpp ${libfiletypes_SRCS})

kde4_add_plugin(kcm_filetypes ${kcm_filetypes_SRCS})

target_link_libraries(kcm_filetypes ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS})

install(TARGETS kcm_filetypes  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )

########### next target ###############

set(keditfiletype_SRCS keditfiletype.cpp ${libfiletypes_SRCS})

add_executable(keditfiletype ${keditfiletype_SRCS})

target_link_libraries(keditfiletype ${KDE4_KIO_LIBS})
if (Q_WS_X11)
  target_link_libraries(keditfiletype ${X11_X11_LIB})
endif (Q_WS_X11)

install(TARGETS keditfiletype ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES filetypes.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )
