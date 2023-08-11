########### next target ###############
set(oxygen_demo_SOURCES
    oxygenbenchmarkwidget.cpp
    oxygenbuttondemowidget.cpp
    oxygendemodialog.cpp
    oxygendemowidget.cpp
    oxygenframedemowidget.cpp
    oxygeninputdemowidget.cpp
    oxygenlistdemowidget.cpp
    oxygensimulator.cpp
    oxygensliderdemowidget.cpp
    oxygentabdemowidget.cpp
    main.cpp
    oxygenbenchmarkwidget.ui
    oxygenbuttondemowidget.ui
    oxygenframedemowidget.ui
    oxygeninputdemowidget.ui
    oxygenlistdemowidget.ui
    oxygensliderdemowidget.ui
    oxygentabdemowidget.ui
)

add_executable(oxygen-demo ${oxygen_demo_SOURCES})
target_link_libraries(oxygen-demo
    KDE4::kdeui
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
)

########### install files ###############
install(
    TARGETS oxygen-demo
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)
