include_directories(${CMAKE_SOURCE_DIR}/kwin)

set(kcm_kwinscreenedges_SRCS
    main.cpp
    monitor.cpp
)
kde4_add_plugin(kcm_kwinscreenedges ${kcm_kwinscreenedges_SRCS})
target_link_libraries(kcm_kwinscreenedges ${X11_LIBRARIES} kworkspace KDE4::plasma)
install(TARGETS kcm_kwinscreenedges DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

install(FILES kwinscreenedges.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
