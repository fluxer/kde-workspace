set(plasmaclock_LIB_SRCS
    clockapplet.cpp
    calendar.cpp
    calendartable.cpp
    wheelytoolbutton.cpp
    timezonesConfig.ui
    generalConfig.ui
    calendarConfig.ui
)

add_library(plasmaclock SHARED ${plasmaclock_LIB_SRCS})

target_link_libraries(plasmaclock
    ${KDE4_PLASMA_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS}
)

set_target_properties(plasmaclock PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)

#clockapplet.h
install(
    FILES clockapplet.h calendar.h calendartable.h plasmaclock_export.h
    DESTINATION ${KDE4_INCLUDE_INSTALL_DIR}/plasmaclock
    COMPONENT Devel
)
install(
    TARGETS plasmaclock
    EXPORT kdeworkspaceLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS}
)
