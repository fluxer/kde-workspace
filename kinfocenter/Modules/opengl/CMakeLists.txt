if(OPENGL_FOUND AND OPENGL_GLU_FOUND)
    include_directories( ${OPENGL_INCLUDE_DIR} ${OPENGL_GLU_INCLUDE_DIR} )
endif()

if(OPENGLES_FOUND)
    set(KCM_ENABLE_OPENGLES TRUE)
    include_directories( ${OPENGLES_INCLUDE_DIR} )
endif()

########### next target ###############

kde4_add_plugin(kcm_opengl opengl.cpp)

target_link_libraries(kcm_opengl ${KDE4_KDEUI_LIBS} ${QT_QTGUI_LIBRARY} ${X11_X11_LIB})

find_library(DL_LIBRARY dl)
if (DL_LIBRARY)
  target_link_libraries(kcm_opengl ${DL_LIBRARY})
endif(DL_LIBRARY)

if(OPENGL_FOUND AND OPENGL_GLU_FOUND)
    target_link_libraries(kcm_opengl ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
endif()

if(OPENGLES_FOUND)
    target_link_libraries(kcm_opengl ${OPENGLES_LIBRARIES})
endif()

configure_file(openglconfig.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/openglconfig.h )

install(TARGETS kcm_opengl  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install(FILES opengl.desktop  DESTINATION  ${SERVICES_INSTALL_DIR})
