set( ksysguarddtest_SRCS
    ksysguarddtest.cpp
    ${CMAKE_SOURCE_DIR}/libs/ksysguard/ksgrd/SensorAgent.cpp
    ${CMAKE_SOURCE_DIR}/libs/ksysguard/ksgrd/SensorManager.cpp
    ${CMAKE_SOURCE_DIR}/libs/ksysguard/ksgrd/SensorSocketAgent.cpp
    ${CMAKE_SOURCE_DIR}/libs/ksysguard/ksgrd/SensorShellAgent.cpp
)

kde4_add_test(ksysguard-ksysguarddtest ${ksysguarddtest_SRCS})

target_link_libraries(ksysguard-ksysguarddtest
    KDE4::kdeui
    ${QT_QTTEST_LIBRARY}
    ${QT_QTNETWORK_LIBRARY}
)
