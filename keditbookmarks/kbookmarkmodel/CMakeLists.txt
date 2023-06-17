if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()

set(kbookmarkmodel_SRCS
    commandhistory.cpp
    commands.cpp
    model.cpp
    treeitem.cpp
    view.cpp
)

add_library(kbookmarkmodel_private SHARED ${kbookmarkmodel_SRCS})

target_link_libraries(kbookmarkmodel_private
    KDE4::kio
)

set_target_properties(kbookmarkmodel_private PROPERTIES
     VERSION ${GENERIC_LIB_VERSION}
     SOVERSION ${GENERIC_LIB_SOVERSION}
)

generate_export_header(kbookmarkmodel_private BASE_NAME KBOOKMARKMODEL)

install(
    TARGETS kbookmarkmodel_private
    DESTINATION ${KDE4_LIB_INSTALL_DIR}
)
