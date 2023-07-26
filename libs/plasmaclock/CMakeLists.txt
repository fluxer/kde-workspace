set(plasmaclock_LIB_SRCS
    clockapplet.cpp
    calendar.cpp
    wheelytoolbutton.cpp
    timezonesConfig.ui
)

add_library(plasmaclock SHARED ${plasmaclock_LIB_SRCS})

target_link_libraries(plasmaclock
    KDE4::plasma
    KDE4::kdeui
    KDE4::kio
)

set_target_properties(plasmaclock PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)

generate_export_header(plasmaclock)

#clockapplet.h
install(
    FILES
    clockapplet.h
    calendar.h
    ${CMAKE_CURRENT_BINARY_DIR}/plasmaclock_export.h
    DESTINATION ${KDE4_INCLUDE_INSTALL_DIR}/plasmaclock
)
install(
    TARGETS plasmaclock
    EXPORT kdeworkspaceTargets
    DESTINATION ${KDE4_LIB_INSTALL_DIR}
)
