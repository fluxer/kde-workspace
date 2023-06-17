project(dirmodel)

set(dirmodel_SRCS
    dirmodel.cpp
    dirmodelplugin.cpp
)

add_library(dirmodelplugin SHARED ${dirmodel_SRCS})
target_link_libraries(dirmodelplugin
    ${QT_QTCORE_LIBRARY}
    ${QT_QTDECLARATIVE_LIBRARY}
    KDE4::kio
)

install(TARGETS dirmodelplugin DESTINATION ${KDE4_IMPORTS_INSTALL_DIR}/org/kde/dirmodel)
install(FILES qmldir DESTINATION ${KDE4_IMPORTS_INSTALL_DIR}/org/kde/dirmodel)

#add_subdirectory(test)
