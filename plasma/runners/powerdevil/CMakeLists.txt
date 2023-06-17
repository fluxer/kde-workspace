INCLUDE_DIRECTORIES(
    ${CMAKE_CURRENT_BINARY_DIR}
)

set(krunner_powerdevil_SRCS
    PowerDevilRunner.cpp
)

kde4_add_plugin(krunner_powerdevil ${krunner_powerdevil_SRCS})
target_link_libraries(krunner_powerdevil KDE4::kdeui KDE4::kio KDE4::plasma KDE4::solid)

install(TARGETS krunner_powerdevil DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

install(FILES plasma-runner-powerdevil.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})

