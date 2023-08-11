#DECLARATIVE APPLET
set(declarative_appletscript_SRCS
    common/extension_launchapp.cpp
    common/extension_io.cpp
    common/scriptenv.cpp
    declarative/toolboxproxy.cpp
    declarative/appletcontainer.cpp
    declarative/declarativeitemcontainer.cpp
    plasmoid/abstractjsappletscript.cpp
    plasmoid/appletinterface.cpp
    plasmoid/declarativeappletscript.cpp
    plasmoid/themedsvg.cpp
    simplebindings/bytearrayclass.cpp
    simplebindings/bytearrayprototype.cpp
    simplebindings/dataengine.cpp
    simplebindings/dataenginereceiver.cpp
    simplebindings/filedialogproxy.cpp
    simplebindings/qscriptbookkeeping.cpp
    simplebindings/url.cpp
    simplebindings/point.cpp
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common)

set_source_files_properties(
    ${CMAKE_CURRENT_SOURCE_DIR}/common/extension_launchapp.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/simplebindings/qscriptbookkeeping.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/simplebindings/bytearrayprototype.cpp
    PROPERTIES SKIP_UNITY_BUILD_INCLUSION TRUE
)

kde4_add_plugin(plasma_appletscript_declarative ${declarative_appletscript_SRCS})
target_link_libraries(plasma_appletscript_declarative
    KDE4::plasma
    KDE4::kio
    ${QT_QTSCRIPT_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    ${QT_QTUITOOLS_LIBRARY}

)


install(TARGETS plasma_appletscript_declarative DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES data/plasma-scriptengine-applet-declarative.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})


