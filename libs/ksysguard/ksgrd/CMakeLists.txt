########### next target ###############

set(ksgrd_LIB_SRCS
   SensorAgent.cpp
   SensorManager.cpp
   SensorShellAgent.cpp
   SensorSocketAgent.cpp
)


add_library(ksgrd SHARED ${ksgrd_LIB_SRCS})
target_link_libraries(ksgrd
    KDE4::kdecore
    ${QT_QTCORE_LIBRARY}
    ${QT_QTNETWORK_LIBRARY}
)

set_target_properties(ksgrd PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)

install(
    TARGETS ksgrd
    EXPORT kdeworkspaceTargets
    DESTINATION ${KDE4_LIB_INSTALL_DIR}
)

########### install files ###############

install(
    FILES SensorAgent.h SensorClient.h SensorManager.h SensorShellAgent.h SensorSocketAgent.h
    DESTINATION ${KDE4_INCLUDE_INSTALL_DIR}/ksgrd
)
