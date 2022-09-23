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
macro_bool_to_01(FONTCONFIG_FOUND HAVE_FONTCONFIG) # kcontrol/{fonts,kfontinst}
macro_bool_to_01(FREETYPE_FOUND HAVE_FREETYPE) # kcontrol/fonts
macro_bool_to_01(X11_XShm_FOUND HAVE_XSHM) # ksplash
macro_bool_to_01(X11_XTest_FOUND HAVE_XTEST) # khotkeys, kxkb
macro_bool_to_01(X11_Xcomposite_FOUND HAVE_XCOMPOSITE) # plasma, kwin
macro_bool_to_01(X11_Xcursor_FOUND HAVE_XCURSOR) # many uses
macro_bool_to_01(X11_Xdamage_FOUND HAVE_XDAMAGE) # kwin
macro_bool_to_01(X11_Xfixes_FOUND HAVE_XFIXES) # klipper, kicker, kwin
macro_bool_to_01(X11_Xkb_FOUND HAVE_XKB) # kglobalaccel, kcontrol/keyboard
macro_bool_to_01(X11_Xinerama_FOUND HAVE_XINERAMA)
macro_bool_to_01(X11_Xrandr_FOUND HAVE_XRANDR) # kwin
macro_bool_to_01(X11_Xrender_FOUND HAVE_XRENDER) # kcontrol/style, kicker
macro_bool_to_01(X11_xf86misc_FOUND HAVE_XF86MISC) # kcontrol/keyboard
macro_bool_to_01(X11_XSync_FOUND HAVE_XSYNC) # kwin
macro_bool_to_01(X11_XRes_FOUND HAVE_XRES) # ksysguard
macro_bool_to_01(X11_dpms_FOUND HAVE_DPMS) # kscreensaver

cmake_reset_check_state()
set(CMAKE_REQUIRED_INCLUDES ${X11_Xrandr_INCLUDE_PATH})
set(CMAKE_REQUIRED_LIBRARIES ${X11_Xrandr_LIB})
check_function_exists(XRRGetScreenSizeRange XRANDR_1_2_FOUND)
macro_bool_to_01(XRANDR_1_2_FOUND HAS_RANDR_1_2)
check_function_exists(XRRGetScreenResourcesCurrent XRANDR_1_3_FOUND)
macro_bool_to_01(XRANDR_1_3_FOUND HAS_RANDR_1_3)
cmake_reset_check_state()
