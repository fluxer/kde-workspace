
########### next target ###############

set(krunner_calculatorrunner_SRCS
    calculatorrunner.cpp
)

if ( QALCULATE_FOUND )
    include_directories(${QALCULATE_INCLUDE_DIR})
    add_definitions(-DENABLE_QALCULATE)

    set(krunner_calculatorrunner_SRCS
        ${krunner_calculatorrunner_SRCS}
        qalculate_engine.cpp
    )

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")
endif( QALCULATE_FOUND )

kde4_add_plugin(krunner_calculatorrunner ${krunner_calculatorrunner_SRCS})
target_link_libraries(krunner_calculatorrunner
    ${KDE4_KDEUI_LIBS}
    ${KDE4_PLASMA_LIBS}
    ${QT_QTSCRIPT_LIBRARY}
)

if ( QALCULATE_FOUND )
    target_link_libraries(krunner_calculatorrunner
        ${KDE4_SOLID_LIBS}
        ${KDE4_KIO_LIBS}
        ${QALCULATE_LIBRARIES}
    )
endif ( QALCULATE_FOUND )

install(TARGETS krunner_calculatorrunner DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

########### install files ###############
install(FILES plasma-runner-calculator.desktop DESTINATION ${KDE4_SERVICES_INSTALL_DIR})
