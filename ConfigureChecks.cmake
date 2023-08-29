include(CMakePushCheckState)
include(CheckTypeSize)
include(CheckSymbolExists)
include(CheckLibraryExists)

find_program(some_x_program NAMES iceauth xrdb xterm)
if (NOT some_x_program)
    set(some_x_program ${CMAKE_INSTALL_PREFIX}/bin/xrdb)
    message("Warning: Could not determine X binary directory. Assuming ${CMAKE_INSTALL_PREFIX}/bin.")
endif (NOT some_x_program)
get_filename_component(proto_xbindir "${some_x_program}" PATH)
get_filename_component(xbindir "${proto_xbindir}" ABSOLUTE)
get_filename_component(xrootdir "${xbindir}" PATH)
set(XLIBDIR "${xrootdir}/lib/X11")
set(XKBDIR "${xrootdir}/share/X11")

check_function_exists(nice HAVE_NICE)
check_include_files(malloc.h HAVE_MALLOC_H)
kde4_bool_to_01(FONTCONFIG_FOUND HAVE_FONTCONFIG) # kcontrol/fonts
kde4_bool_to_01(FREETYPE_FOUND HAVE_FREETYPE) # kcontrol/fonts
kde4_bool_to_01(X11_XTest_FOUND HAVE_XTEST) # kcontrol/keyboard
kde4_bool_to_01(X11_Xcomposite_FOUND HAVE_XCOMPOSITE) # plasma, kwin
kde4_bool_to_01(X11_Xcursor_FOUND HAVE_XCURSOR) # many uses
kde4_bool_to_01(X11_Xdamage_FOUND HAVE_XDAMAGE) # kwin
kde4_bool_to_01(X11_Xfixes_FOUND HAVE_XFIXES) # klipper, kicker, kwin
kde4_bool_to_01(X11_Xkb_FOUND HAVE_XKB) # kglobalaccel, kcontrol/keyboard
kde4_bool_to_01(X11_Xrandr_FOUND HAVE_XRANDR) # kwin
kde4_bool_to_01(X11_xf86misc_FOUND HAVE_XF86MISC) # kcontrol/keyboard
kde4_bool_to_01(X11_XSync_FOUND HAVE_XSYNC) # kwin
kde4_bool_to_01(X11_XRes_FOUND HAVE_XRES) # ksysguard
kde4_bool_to_01(X11_dpms_FOUND HAVE_DPMS) # kscreensaver
