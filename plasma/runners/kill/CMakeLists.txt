project(plasma-runner-kill)

set(krunner_kill_SRCS killrunner.cpp)

set(kcm_krunner_kill_SRCS
    killrunner_config.cpp
)
kde4_add_plugin(kcm_krunner_kill ${kcm_krunner_kill_SRCS})
target_link_libraries(kcm_krunner_kill
    KDE4::kdecore
    KDE4::kdeui
    KDE4::kcmutils 
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
)
    
kde4_add_plugin(krunner_kill ${krunner_kill_SRCS})
target_link_libraries(krunner_kill
    KDE4::plasma
    KDE4::kio
    processcore
)

install(
    TARGETS krunner_kill kcm_krunner_kill
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)
        
install(
    FILES
    plasma-runner-kill.desktop
    plasma-runner-kill_config.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
