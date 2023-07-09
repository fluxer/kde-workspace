add_subdirectory(pics)

set(kcm_useraccount_PART_SRCS
    main.cpp
    chfacedlg.cpp
    main_widget.ui
    faceDlg.ui
)

kde4_add_kcfg_files(kcm_useraccount_PART_SRCS settings.kcfgc pass.kcfgc)

kde4_add_plugin(kcm_useraccount ${kcm_useraccount_PART_SRCS})

target_link_libraries(kcm_useraccount
    KDE4::kio
    KDE4::kemail
    konq
)

install(
    TARGETS kcm_useraccount
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

install(
    FILES kcm_useraccount.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
