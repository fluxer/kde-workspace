project(windowedwidgetsrunner)

include_directories( ${QT_INCLUDES} ${KDE4_INCLUDES} )

set(krunner_windowedwidgets_SRCS
    windowedwidgetsrunner.cpp
)

kde4_add_plugin(krunner_windowedwidgets ${krunner_windowedwidgets_SRCS})
target_link_libraries(krunner_windowedwidgets KDE4::plasma)

install(TARGETS krunner_windowedwidgets DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})
install(FILES plasma-runner-windowedwidgets.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
