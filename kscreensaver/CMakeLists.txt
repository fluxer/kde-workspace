
add_subdirectory( libkscreensaver ) 
add_subdirectory( kblank_screensaver ) 
add_subdirectory( krandom_screensaver ) 

macro_optional_find_package(Xscreensaver)
macro_log_feature(XSCREENSAVER_FOUND "Xscreensaver"
    "A modular screen saver and locker for the X Window System"
    "http://www.jwz.org/xscreensaver"
    FALSE
    ""
    "Provides screen savers for the desktop.")

macro_optional_find_package(OpenGL)
macro_log_feature(OPENGL_FOUND "OpenGL" "API for developing portable, interactive 2D and 3D graphics applications" "http://mesa3d.sourceforge.net" FALSE "" "Provides 3D screensavers.")
add_subdirectory( kdesavers ) 
add_subdirectory( kpartsaver ) 

if(XSCREENSAVER_FOUND)
    add_subdirectory(kxsconfig)
    add_subdirectory(xsavers)
endif(XSCREENSAVER_FOUND)
