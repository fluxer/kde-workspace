# need a header file from Aurorae sources
include_directories(
    ${CMAKE_SOURCE_DIR}/kwin/clients/aurorae/src/lib
)

set(AURORAE_SOURCE_DIR ${CMAKE_SOURCE_DIR}/kwin/clients/aurorae/src/lib/)

set(kcm_kwindecoration_PART_SRCS
    kwindecoration.cpp
    buttons.cpp
    buttonsconfigdialog.cpp
    configdialog.cpp
    preview.cpp
    decorationmodel.cpp
    ${AURORAE_SOURCE_DIR}/auroraetheme.cpp
    ${AURORAE_SOURCE_DIR}/themeconfig.cpp
    auroraeconfig.ui
    buttons.ui
    config.ui
    decoration.ui
)

kde4_add_plugin(kcm_kwindecoration ${kcm_kwindecoration_PART_SRCS})
target_link_libraries(kcm_kwindecoration kdecorations ${KDE4_PLASMA_LIBS} ${QT_QTDECLARATIVE_LIBRARY} ${QT_QTUITOOLS_LIBRARY} ${X11_LIBRARIES})
install(TARGETS kcm_kwindecoration  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )

########### install files ###############

install( FILES kwindecoration.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )
install( FILES
    qml/main.qml
    qml/AuroraeDecoration.qml
    qml/AuroraePreview.qml
    qml/DecorationPreview.qml
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kwin/kcm_kwindecoration)
