if(LIBDRM_FOUND)
    set(KCM_ENABLE_DRM TRUE)
    include_directories(${LIBDRM_INCLUDE_DIR})
endif()

if(OPENGL_FOUND AND OPENGL_GLU_FOUND)
    set(KCM_ENABLE_OPENGL TRUE)
    include_directories(${OPENGL_INCLUDE_DIR} ${OPENGL_GLU_INCLUDE_DIR})
endif()

if(OpenGL_EGL_FOUND)
    set(KCM_ENABLE_EGL TRUE)
    include_directories(${OPENGL_EGL_INCLUDE_DIRS})
endif()

########### next target ###############

kde4_add_plugin(kcm_opengl opengl.cpp)

target_link_libraries(kcm_opengl KDE4::kdeui ${QT_QTGUI_LIBRARY} ${X11_X11_LIB})

if(LIBDRM_FOUND)
    target_link_libraries(kcm_opengl ${LIBDRM_LIBRARIES})
endif()

if(OPENGL_FOUND AND OPENGL_GLU_FOUND)
    target_link_libraries(kcm_opengl ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
endif()

if(OpenGL_EGL_FOUND)
    target_link_libraries(kcm_opengl ${OPENGL_egl_LIBRARY})
endif()

configure_file(openglconfig.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/openglconfig.h)

install(TARGETS kcm_opengl  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR})

########### install files ###############

install(FILES opengl.desktop  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR})
