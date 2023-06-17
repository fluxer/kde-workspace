# kdecorations library
set(kdecorations_LIB_SRCS
   kdecoration.cpp
   kdecoration_p.cpp
   kdecoration_plugins_p.cpp
   kdecorationfactory.cpp
   kcommondecoration.cpp
   kcommondecoration_p.cpp )

add_library(kdecorations SHARED ${kdecorations_LIB_SRCS})

target_link_libraries(kdecorations PRIVATE KDE4::kdeui)
target_link_libraries(kdecorations PUBLIC KDE4::kdeui)

set_target_properties(kdecorations PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)
install(
    TARGETS kdecorations
    EXPORT kdeworkspaceLibraryTargets
    DESTINATION ${KDE4_LIB_INSTALL_DIR}
)

########### install files ###############

install(
    FILES
    kdecoration.h
    kdecorationfactory.h
    kcommondecoration.h
    DESTINATION ${KDE4_INCLUDE_INSTALL_DIR}
)
