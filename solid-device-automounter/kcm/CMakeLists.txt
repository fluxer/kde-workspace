set(kcm_device_automounter_SRCS
    DeviceAutomounterKCM.cpp
    DeviceModel.cpp
    DeviceAutomounterKCM.ui
)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

kde4_add_kcfg_files(kcm_device_automounter_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/AutomounterSettingsBase.kcfgc
    ${CMAKE_CURRENT_SOURCE_DIR}/LayoutSettings.kcfgc
)

kde4_add_plugin(kcm_device_automounter ${kcm_device_automounter_SRCS} ${device_automounter_lib_SRCS})

target_link_libraries(kcm_device_automounter KDE4::kdeui KDE4::solid)

install(TARGETS kcm_device_automounter DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

install(FILES device_automounter_kcm.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
