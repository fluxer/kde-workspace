########### install template files ###############

install(
    FILES
    html.katetemplate
    cpplgpl.cpp.katetemplate
    cpplgpl.hh.katetemplate
    cppgpl.cpp.katetemplate
    cppgpl.hh.katetemplate
    language.xml.katetemplate
    FindXXX.cmake.katetemplate
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kate/plugins/katefiletemplates/templates
)
