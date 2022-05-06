
########### next target ###############

set(KCM_MEMORY_PART_SRCS memory.cpp chartWidget.cpp totalMemoryChart.cpp physicalMemoryChart.cpp swapMemoryChart.cpp )

kde4_add_plugin(kcm_memory ${KCM_MEMORY_PART_SRCS})

target_link_libraries(kcm_memory ${KDE4_KDEUI_LIBS} ${QT_QTGUI_LIBRARY})

if (CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
   target_link_libraries(kcm_memory kvm)
endif()

install(
    TARGETS kcm_memory
    DESTINATION ${KDE4_PLUGIN_INSTALL_DIR}
)

########### install files ###############

install(
    FILES kcm_memory.desktop
    DESTINATION ${KDE4_SERVICES_INSTALL_DIR}
)
