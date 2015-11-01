
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../  ../processcore/)

check_include_files(sys/endian.h HAVE_SYS_ENDIAN_H)
check_include_files(byteswap.h HAVE_BYTESWAP_H)

set(processui_LIB_SRCS
   ksysguardprocesslist.cpp
   ProcessFilter.cpp
   ProcessModel.cpp
   ReniceDlg.cpp
   KTextEditVT.cpp
   ReniceDlgUi.ui
   ProcessWidgetUI.ui
)

add_library(processui SHARED ${processui_LIB_SRCS})

if(X11_XRes_FOUND)
    target_link_libraries(processui PRIVATE ${X11_XRes_LIB} ${X11_LIBRARIES})
    include_directories(${X11_XRes_INCLUDE_PATH})
endif()

target_link_libraries(processui PRIVATE ${KDE4_KDEUI_LIBS} ${QT_QTSCRIPT_LIBRARY} processcore)
target_link_libraries(processui PUBLIC ${KDE4_KDEUI_LIBS} processcore)


set_target_properties(processui
    PROPERTIES VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)

install(TARGETS processui EXPORT kdeworkspaceLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS} )

#----------------------

kde4_add_widget(ksysguardwidgets_SRCS ksysguard.widgets)

kde4_add_plugin(ksysguardwidgets ${ksysguardwidgets_SRCS} ${ksysguardwidgets_PART_SRCS})

target_link_libraries(ksysguardwidgets ${KDE4_KDEUI_LIBS} processui)

install(TARGETS ksysguardwidgets DESTINATION ${PLUGIN_INSTALL_DIR}/plugins/designer )

install( FILES ProcessModel.h ProcessFilter.h KTextEditVT.h ksysguardprocesslist.h DESTINATION ${INCLUDE_INSTALL_DIR}/ksysguard COMPONENT Devel)

