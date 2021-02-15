
add_subdirectory( usbview )
add_subdirectory( infosummary )
add_subdirectory( memory )
add_subdirectory( devinfo )
add_subdirectory( info )
add_subdirectory( samba )
add_subdirectory( nics )

add_feature_info("OpenGL support" OPENGL_FOUND "View OpenGL details in kinfocenter." )
add_feature_info("OpenGL/ES support" OPENGLES_FOUND "View OpenGL ES2.0 details in kinfocenter." )
add_feature_info("DRM support" LIBDRM_FOUND "View 3D acceleration details in kinfocenter." )

if((OPENGL_FOUND AND OPENGL_GLU_FOUND) OR OPENGLES_FOUND)
    add_subdirectory( opengl )
else()
    message(STATUS "OpenGL/ES2.0 information module has been disabled.")
endif()

macro_optional_find_package(PCIUTILS)
set_package_properties(PCIUTILS PROPERTIES
    DESCRIPTION "PciUtils is a library for direct access to PCI slots"
    URL "http://atrey.karlin.mff.cuni.cz/~mj/pciutils.shtml"
    TYPE OPTIONAL
    PURPOSE "View PCI details in kinfocenter."
)

add_subdirectory( pci )

macro_optional_find_package(RAW1394)
set_package_properties(RAW1394 PROPERTIES
    DESCRIPTION "library for direct access to IEEE 1394 bus"
    URL "http://www.linux1394.org/"
    TYPE OPTIONAL
    PURPOSE "View FireWire devices in kinfocenter."
)

if(RAW1394_FOUND)
   add_subdirectory( view1394 )
endif(RAW1394_FOUND)
