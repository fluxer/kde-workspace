add_subdirectory( favicons )
add_subdirectory( Templates )
if(ENABLE_TESTING)
    add_subdirectory( tests )
endif()

########### libkonq ###############

set(konq_LIB_SRCS
   konq_popupmenu.cpp       # used by konqueror, kfind, folderview, kickoff
   konq_popupmenuplugin.cpp # for KonqPopupMenu and its plugins
   konq_dndpopupmenuplugin.cpp # for KonqDndPopupMenu and its plugins
   konq_copytomenu.cpp         # used by dolphin, KonqPopupMenu
   konq_operations.cpp         # used by dolphin and konqueror
   konqmimedata.cpp         # used by dolphin, KonqOperations, some filemanagement konqueror modules.
   kversioncontrolplugin.cpp  # used by dolphin and its version control plugins
)

add_library(konq ${LIBRARY_TYPE} ${konq_LIB_SRCS})

target_link_libraries(konq PRIVATE
    ${X11_X11_LIB}
)
target_link_libraries(konq PUBLIC
    ${KDE4_KPARTS_LIBS}
    ${KDE4_KFILE_LIBS}
)

set_target_properties(konq PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)

install(TARGETS konq ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install(FILES
    directory_bookmarkbar.desktop
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/kbookmark
)

generate_export_header(konq)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/konq_export.h
    konq_popupmenu.h # used by folderview
    konq_popupmenuplugin.h
    konq_dndpopupmenuplugin.h
    # konq_copytomenu.h - anyone needs it?
    konq_operations.h
    konqmimedata.h
    kversioncontrolplugin.h
    DESTINATION ${KDE4_INCLUDE_INSTALL_DIR}
    COMPONENT Devel
)
install(FILES
    konqpopupmenuplugin.desktop
    konqdndpopupmenuplugin.desktop
    DESTINATION ${KDE4_SERVICETYPES_INSTALL_DIR}
)
