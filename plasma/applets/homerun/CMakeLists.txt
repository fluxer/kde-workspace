project(homerun)

# Packaging
set(HOMERUN_VERSION "1.2.5")

# Compile options
add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
add_definitions(-DQT_USE_FAST_CONCATENATION -DQT_USE_FAST_OPERATOR_PLUS)
add_definitions(-std=c++0x)
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

# configuration file
configure_file(homerun_config.h.in ${CMAKE_BINARY_DIR}/homerun_config.h @ONLY)

# config/
install(FILES config/homerunrc DESTINATION ${CONFIG_INSTALL_DIR})
install(FILES config/homerunkickerrc DESTINATION ${CONFIG_INSTALL_DIR})

install(FILES data/favoriteapps.xml DESTINATION ${DATA_INSTALL_DIR}/homerun/)

add_subdirectory(lib)
add_subdirectory(components)
add_subdirectory(fullscreen)
add_subdirectory(fixes)
#add_subdirectory(tests)
add_subdirectory(icons)
add_subdirectory(kicker)
add_subdirectory(plugins)
