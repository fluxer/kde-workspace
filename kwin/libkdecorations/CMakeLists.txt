# kdecorations library
set(kdecorations_LIB_SRCS
   kdecoration.cpp
   kdecoration_p.cpp
   kdecoration_plugins_p.cpp
   kdecorationfactory.cpp
   kcommondecoration.cpp
   kcommondecoration_p.cpp )

add_library(kdecorations SHARED ${kdecorations_LIB_SRCS})

target_link_libraries(kdecorations PRIVATE ${KDE4_KDEUI_LIBS})
target_link_libraries(kdecorations PUBLIC ${KDE4_KDEUI_LIBS})

set_target_properties(kdecorations PROPERTIES
   VERSION ${GENERIC_LIB_VERSION}
   SOVERSION ${GENERIC_LIB_SOVERSION}
   )
install(TARGETS kdecorations EXPORT kdeworkspaceLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS} )

########### install files ###############

install( FILES
    kdecoration.h
    kdecorationfactory.h
    kcommondecoration.h
    DESTINATION ${KDE4_INCLUDE_INSTALL_DIR} COMPONENT Devel )
