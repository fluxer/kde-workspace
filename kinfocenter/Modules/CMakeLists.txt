

add_subdirectory( infosummary )
add_subdirectory( memory )
add_subdirectory( devinfo )
add_subdirectory( info )
add_subdirectory( nics )
add_subdirectory( pci )

add_feature_info("OpenGL support" OPENGL_FOUND "View OpenGL details in kinfocenter." )
add_feature_info("EGL support" OpenGL_EGL_FOUND "View EGL details in kinfocenter." )
add_feature_info("DRM support" LIBDRM_FOUND "View 3D acceleration details in kinfocenter." )
add_feature_info("USB support" LIBUSB_FOUND "View USB details in kinfocenter." )
add_feature_info("Kernel modules support" KMOD_FOUND "View kernel module details in kinfocenter." )

if((OPENGL_FOUND AND OPENGL_GLU_FOUND) OR OpenGL_EGL_FOUND)
    add_subdirectory( opengl )
else()
    message(STATUS "OpenGL/ES2.0 information module has been disabled.")
endif()

if(LIBUSB_FOUND)
    add_subdirectory( usbview )
else()
    message(STATUS "USB information module has been disabled.")
endif()

if(RAW1394_FOUND)
   add_subdirectory( view1394 )
endif(RAW1394_FOUND)

if(KMOD_FOUND)
   add_subdirectory( kernel )
endif(KMOD_FOUND)
