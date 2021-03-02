include(UnixAuth)
set_package_properties(PAM PROPERTIES
    DESCRIPTION "PAM Libraries"
    URL "https://www.kernel.org/pub/linux/libs/pam/"
    TYPE OPTIONAL
    PURPOSE "Required for screen unlocking and optionally used by the KDM log in manager"
)

include(CMakePushCheckState)
include(CheckTypeSize)
include(CheckSymbolExists)

if (PAM_FOUND)
    set(KDE4_COMMON_PAM_SERVICE "kde" CACHE STRING "The PAM service to use unless overridden for a particular app.")

    macro(define_pam_service APP)
        string(TOUPPER ${APP}_PAM_SERVICE var)
        set(cvar KDE4_${var})
        set(${cvar} "${KDE4_COMMON_PAM_SERVICE}" CACHE STRING "The PAM service for ${APP}.")
        mark_as_advanced(${cvar})
        set(${var} "\"${${cvar}}\"")
    endmacro(define_pam_service)

    macro(install_pam_service APP)
        string(TOUPPER KDE4_${APP}_PAM_SERVICE cvar)
        install(CODE "
            set(DESTDIR_VALUE \"\$ENV{DESTDIR}\")
            if (NOT DESTDIR_VALUE)
                exec_program(\"${CMAKE_SOURCE_DIR}/mkpamserv\" ARGS ${${cvar}} RETURN_VALUE ret)
                if (NOT ret)
                    exec_program(\"${CMAKE_SOURCE_DIR}/mkpamserv\" ARGS -P ${${cvar}}-np)
                endif (NOT ret)
            endif (NOT DESTDIR_VALUE)
                    ")
    endmacro(install_pam_service)

    define_pam_service(KDM)
    define_pam_service(kscreensaver)

else (PAM_FOUND)

    macro(install_pam_service APP)
    endmacro(install_pam_service)

endif (PAM_FOUND)

find_program(some_x_program NAMES iceauth xrdb xterm)
if (NOT some_x_program)
    set(some_x_program /usr/bin/xrdb)
    message("Warning: Could not determine X binary directory. Assuming /usr/bin.")
endif (NOT some_x_program)
get_filename_component(proto_xbindir "${some_x_program}" PATH)
get_filename_component(XBINDIR "${proto_xbindir}" ABSOLUTE)
get_filename_component(xrootdir "${XBINDIR}" PATH)
set(XLIBDIR "${xrootdir}/lib/X11")
set(XKBDIR "${xrootdir}/share/X11")

check_function_exists(getpassphrase HAVE_GETPASSPHRASE)
check_function_exists(vsyslog HAVE_VSYSLOG)
check_function_exists(nice HAVE_NICE)

check_include_files(string.h HAVE_STRING_H)
check_include_files(sys/select.h HAVE_SYS_SELECT_H)
check_include_files(limits.h HAVE_LIMITS_H)
check_include_files(sys/time.h HAVE_SYS_TIME_H)     # ksmserver, ksplashml, sftp
check_include_files(stdint.h HAVE_STDINT_H)         # kcontrol/kfontinst
check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(malloc.h HAVE_MALLOC_H)
macro_bool_to_01(FONTCONFIG_FOUND HAVE_FONTCONFIG) # kcontrol/{fonts,kfontinst}
macro_bool_to_01(FREETYPE_FOUND HAVE_FREETYPE) # kcontrol/fonts
macro_bool_to_01(X11_XShm_FOUND HAVE_XSHM) # ksplash
macro_bool_to_01(X11_XTest_FOUND HAVE_XTEST) # khotkeys, kxkb, kdm
macro_bool_to_01(X11_Xcomposite_FOUND HAVE_XCOMPOSITE) # plasma, kwin
macro_bool_to_01(X11_Xcursor_FOUND HAVE_XCURSOR) # many uses
macro_bool_to_01(X11_Xdamage_FOUND HAVE_XDAMAGE) # kwin
macro_bool_to_01(X11_Xfixes_FOUND HAVE_XFIXES) # klipper, kicker, kwin
macro_bool_to_01(X11_Xkb_FOUND HAVE_XKB) # kdm, kglobalaccel, kcontrol/keyboard
macro_bool_to_01(X11_Xinerama_FOUND HAVE_XINERAMA)
macro_bool_to_01(X11_Xrandr_FOUND HAVE_XRANDR) # kwin
macro_bool_to_01(X11_Xrender_FOUND HAVE_XRENDER) # kcontrol/style, kicker
macro_bool_to_01(X11_xf86misc_FOUND HAVE_XF86MISC) # kcontrol/keyboard
macro_bool_to_01(X11_dpms_FOUND HAVE_DPMS) # powerdevil
macro_bool_to_01(X11_XSync_FOUND HAVE_XSYNC) # kwin
macro_bool_to_01(X11_XRes_FOUND HAVE_XRES) # ksysguard

check_function_exists(setpriority  HAVE_SETPRIORITY) # kscreenlocker 

cmake_reset_check_state()
set(CMAKE_REQUIRED_LIBRARIES ${X11_Xext_LIB})
set(CMAKE_REQUIRED_INCLUDES ${X11_X11_INCLUDE_PATH})
check_symbol_exists(DPMSCapable "X11/Xlib.h;X11/extensions/dpms.h" HAVE_DPMSCAPABLE_PROTO)
check_symbol_exists(DPMSInfo "X11/Xlib.h;X11/extensions/dpms.h" HAVE_DPMSINFO_PROTO)
cmake_reset_check_state()

cmake_reset_check_state()
set(CMAKE_REQUIRED_INCLUDES ${X11_Xrandr_INCLUDE_PATH})
set(CMAKE_REQUIRED_LIBRARIES ${X11_Xrandr_LIB})
check_function_exists(XRRGetScreenSizeRange XRANDR_1_2_FOUND)
macro_bool_to_01(XRANDR_1_2_FOUND HAS_RANDR_1_2)
check_function_exists(XRRGetScreenResourcesCurrent XRANDR_1_3_FOUND)
macro_bool_to_01(XRANDR_1_3_FOUND HAS_RANDR_1_3)
cmake_reset_check_state()
