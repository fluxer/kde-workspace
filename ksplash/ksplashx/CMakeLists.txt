project(ksplashx)

add_subdirectory(scale)
add_subdirectory(utils)

include_directories(
    ${PNG_INCLUDE_DIRS}
    ${JPEG_INCLUDE_DIR}
)

include(CheckCXXSourceCompiles)
CHECK_CXX_SOURCE_COMPILES("
int main() { __asm__(\"pxor %mm0, %mm0\") ; }"
    X86_MMX_FOUND
)

########### next target ###############

set(ksplashx_SRCS
    main.cpp
    qglobal.cpp
    qjpegio.cpp
    qpngio.cpp
    qimage.cpp
    qsize.cpp
    pixmap.cpp
    qcolor.cpp
    qcolor_x11.cpp
    qcolor_p.cpp
    x11_defs.cpp
    defs.cpp
    splash.cpp
    qrect.cpp
    kcpuinfo.cpp
    scale.cpp
)

# this doesn't work on x86_64 so it needs somebody who knows assembly enough to fix it
if(X86_MMX_FOUND AND "${CMAKE_SYSTEM_PROCESSOR}" MATCHES "i[3-6]86")
    enable_language(ASM)
    set(ksplashx_SRCS
        ${ksplashx_SRCS}
        asm_scale.S
    )
    add_definitions(-DHAVE_X86_MMX)
endif()

add_executable(ksplashx ${ksplashx_SRCS})
target_link_libraries(ksplashx ${X11_LIBRARIES} ${PNG_LIBRARIES} ${JPEG_LIBRARIES})
if(X11_Xinerama_FOUND)
    target_link_libraries(ksplashx ${X11_Xinerama_LIB})
endif()

install(TARGETS ksplashx ${INSTALL_TARGETS_DEFAULT_ARGS})
