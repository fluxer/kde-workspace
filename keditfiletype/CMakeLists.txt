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

target_link_libraries(kcm_filetypes KDE4::kdeui KDE4::kio)

install(
    TARGETS kcm_filetypes
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### next target ###############

set(keditfiletype_SRCS keditfiletype.cpp ${libfiletypes_SRCS})

add_executable(keditfiletype ${keditfiletype_SRCS})

target_link_libraries(keditfiletype
    KDE4::kio
    ${X11_X11_LIB}
)

install(
    TARGETS keditfiletype
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES filetypes.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
