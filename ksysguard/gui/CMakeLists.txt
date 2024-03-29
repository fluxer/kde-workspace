
if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()

include_directories(
   ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/
   ${CMAKE_SOURCE_DIR}/libs
   ${CMAKE_SOURCE_DIR}/libs/ksysguard
   ${CMAKE_SOURCE_DIR}/libs/ksysguard/signalplotter
   ${CMAKE_SOURCE_DIR}/libs/ksysguard/processcore
)


########### next target ###############

set(libsensordisplays_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/SensorDisplay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/BarGraph.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/DancingBars.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/DancingBarsSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/DummyDisplay.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/FancyPlotter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/FancyPlotterSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/ListView.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/LogFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/MultiMeter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/MultiMeterSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/ProcessController.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/SensorLogger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/SensorLoggerDlg.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/SensorLoggerSettings.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/SensorModel.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/ListViewSettings.cpp
    # ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/modeltest.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/ListViewSettingsWidget.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/LogFileSettings.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/MultiMeterSettingsWidget.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/SensorLoggerDlgWidget.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/SensorDisplayLib/SensorLoggerSettingsWidget.ui
)

set(ksysguard_SRCS ${libsensordisplays_SRCS}
   ksortfilterproxymodel.cpp
   SensorBrowser.cpp
   WorkSheet.cpp
   WorkSheetSettings.cpp
   Workspace.cpp
   HostConnector.cpp
   StyleEngine.cpp
   ksysguard.cpp
)

add_executable(ksysguard ${ksysguard_SRCS})
target_link_libraries(ksysguard
    KDE4::kio
    processui ksignalplotter
    ksgrd
)

install(
    TARGETS ksysguard
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)

########### install files ###############

install(
    PROGRAMS ksysguard.desktop
    DESTINATION ${KDE4_XDG_APPS_INSTALL_DIR}
)
install(
    FILES
    ProcessTable.sgrd SystemLoad2.sgrd
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/ksysguard
)
install(
    FILES ksysguardui.rc
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/ksysguard
)

