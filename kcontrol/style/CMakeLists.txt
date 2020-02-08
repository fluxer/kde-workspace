# needed for including kdecoration.h
include_directories (
    ${CMAKE_SOURCE_DIR}/kwin/libkwineffects
    ${CMAKE_SOURCE_DIR}/kwin/libkdecorations
    ${CMAKE_BINARY_DIR}/kwin/libkwineffects
)

########### next target ###############

set(kcm_style_PART_SRCS
    ../krdb/krdb.cpp
    styleconfdialog.cpp
    kcmstyle.cpp
    stylepreview.ui
    finetuning.ui
)

kde4_add_plugin(kcm_style ${kcm_style_PART_SRCS})

target_link_libraries(kcm_style ${KDE4_KIO_LIBS} ${X11_LIBRARIES}
    ${KDE4_PLASMA_LIBS})
target_link_libraries(kcm_style kdecorations)

install(TARGETS kcm_style  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES kcmstyle.notifyrc  DESTINATION  ${KDE4_DATA_INSTALL_DIR}/kcmstyle )
install( FILES style.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )
