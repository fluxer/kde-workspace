project(kde-workspace)

include(CheckIncludeFiles)
include(CheckFunctionExists)
include(GenerateExportHeader)

find_package(KDE4 4.22.0 REQUIRED)
include(KDE4Defaults)

find_package(ZLIB)
set_package_properties(ZLIB PROPERTIES
    DESCRIPTION "Support for gzip compressed files and data streams"
    URL "http://www.zlib.net"
    TYPE REQUIRED
)

find_package(PNG)
set_package_properties(PNG PROPERTIES
    DESCRIPTION "An Open, Extensible Image Format with Lossless Compression"
    URL "http://www.libpng.org/"
    TYPE REQUIRED
)

# shared-mime-info 0.40 is mandatory for generic-icon, kdelibs requires 0.70 anyway
find_package(SharedMimeInfo 0.70)
set_package_properties(SharedMimeInfo PROPERTIES
    DESCRIPTION "Shared Mime Info"
    URL "http://freedesktop.org/wiki/Software/shared-mime-info"
    PURPOSE "Needed for update MIME database"
    TYPE REQUIRED
)

if(Q_WS_X11)
    find_package(XCB REQUIRED)
    set_package_properties(XCB PROPERTIES
        DESCRIPTION "X protocol C-language Binding"
        URL "http://xcb.freedesktop.org"
        TYPE REQUIRED
    )

    find_package(X11_XCB)
    set_package_properties(X11_XCB
        PROPERTIES DESCRIPTION "XCB X11 protocol client library"
        TYPE REQUIRED
    )

    find_package(X11)
    set_package_properties(X11 PROPERTIES
        DESCRIPTION "X11 libraries"
        URL "http://www.x.org"
        TYPE REQUIRED
    )

    add_feature_info("libXTest" X11_XTest_FOUND "The X11 Testing Resource extension library is useful for automated testing of X clients")
    add_feature_info("libXcomposite" X11_Xcomposite_FOUND "The X11 Composite extension library may be used by Plasma and KWin")
    add_feature_info("libXShm" X11_XShm_FOUND "The X11 shared-memory fences library may be used by ksplash")
    add_feature_info("libxf86misc" X11_xf86misc_FOUND "The X11 miscellaneous library may be used by keyboard KCM") # TODO: unused?
    add_feature_info("libXSync" X11_XSync_FOUND "The X11 synchronization may be used by KWin")
    add_feature_info("libXinput" X11_Xinput_FOUND "The X11 XINPUT extension library may be used by keyboard KCM")
    add_feature_info("libXft" X11_Xft_FOUND "The X11 font API library may be used by kfontinst KCM")
    add_feature_info("libXinerama" X11_Xinerama_FOUND "The X11 XINERAMA extension library may be used by ksplash")
    add_feature_info("libXRes" X11_XRes_FOUND "The X Resource library may be used by ksysguard")

    if(NOT X11_Xau_FOUND)
        message(FATAL_ERROR "The X11 authorization extension library was not found. Required for authorization in kworkspace library")
    endif()
    if(NOT X11_Xdamage_FOUND)
        message(FATAL_ERROR "The X11 damaged region extension library was not found. Required for compositing support in KWin")
    endif()
    if(NOT X11_Xrender_FOUND)
        message(FATAL_ERROR " The X Rendering extension client library was not found. Required for XRender Compositing backend in KWin")
    endif()
    if(NOT X11_Xfixes_FOUND)
        message(FATAL_ERROR "The X11 miscellaneous 'fixes' extension library was not found. Required for XRender Compositing backend in KWin")
    endif()
    if(NOT X11_Xrandr_FOUND)
        message(FATAL_ERROR "The X11 RandR extension library was not found. Required for Multi Screen Support")
    endif()
    if(NOT X11_Xcursor_FOUND)
        message(FATAL_ERROR "The X11 cursor management library was not found. Required for desktop effects support in KWin")
    endif()
endif(Q_WS_X11)

macro_optional_find_package(DBusMenuQt 0.6.0)
set_package_properties(DBusMenuQt PROPERTIES
    DESCRIPTION "Support for notification area menus via the DBusMenu protocol"
    URL "https://launchpad.net/libdbusmenu-qt"
    TYPE RECOMMENDED
)

macro_optional_find_package(OpenGL)
set_package_properties(OpenGL PROPERTIES
    DESCRIPTION "The OpenGL libraries"
    URL "http://www.opengl.org"
    PURPOSE "OpenGL info module"
    TYPE OPTIONAL
)

macro_optional_find_package(LibDRM)
set_package_properties(LibDRM PROPERTIES
    DESCRIPTION "Userspace interface to kernel DRM services"
    URL "https://dri.freedesktop.org/wiki/"
    TYPE OPTIONAL
    PURPOSE "OpenGL info module"
)

macro_optional_find_package(Freetype)
set_package_properties(Freetype PROPERTIES
    DESCRIPTION "Freely available software library to render fonts"
    URL "https://www.freetype.org"
    PURPOSE "Needed to build font configuration and installation tools"
    TYPE OPTIONAL
)

macro_optional_find_package(Fontconfig)
set_package_properties(Fontconfig PROPERTIES
    DESCRIPTION "Font access configuration library"
    URL "http://www.freedesktop.org/wiki/Software/fontconfig"
    PURPOSE "Needed to build font configuration and installation tools"
    TYPE OPTIONAL
)

macro_optional_find_package(LibKonq)
set_package_properties(LibKonq PROPERTIES
    DESCRIPTION "Konqueror library"
    URL "http://fluxer.github.io/katana/"
    PURPOSE "folderview plasma applets"
    TYPE OPTIONAL
)

macro_optional_find_package(LibUSB)
set_package_properties(LibUSB PROPERTIES
    DESCRIPTION "User level access to USB devices"
    URL "https://libusb.info/"
    TYPE OPTIONAL
    PURPOSE "Provides Logitech mouse support in KControl and USB devices information"
)

macro_optional_find_package(Kmod)
set_package_properties(Kmod PROPERTIES
    DESCRIPTION "Set of tools to handle common tasks with Linux kernel modules"
    URL "https://git.kernel.org/pub/scm/utils/kernel/kmod/kmod.git/"
    TYPE OPTIONAL
    PURPOSE "Kernel modules information"
)
macro_optional_find_package(PCIUTILS)
set_package_properties(PCIUTILS PROPERTIES
    DESCRIPTION "PciUtils is a library for direct access to PCI slots"
    URL "http://atrey.karlin.mff.cuni.cz/~mj/pciutils.shtml"
    TYPE OPTIONAL
    PURPOSE "View PCI details in kinfocenter"
)

macro_optional_find_package(RAW1394)
set_package_properties(RAW1394 PROPERTIES
    DESCRIPTION "library for direct access to IEEE 1394 bus"
    URL "http://www.linux1394.org/"
    TYPE OPTIONAL
    PURPOSE "View FireWire devices in kinfocenter"
)

# we need a version of samba which has already smbc_set_context(), Alex
set(SAMBA_REQUIRE_SMBC_SET_CONTEXT TRUE)
set(SAMBA_REQUIRE_SMBC_OPTION_SET TRUE)
macro_optional_find_package(Samba)
set_package_properties(Samba PROPERTIES
    DESCRIPTION "the SMB client library, a version with smbc_set_context() and smbc_option_set()"
    URL "http://www.samba.org"
    TYPE OPTIONAL
    PURPOSE "Needed to build the SMB kioslave"
)

macro_optional_find_package(LibSSH 0.6.0)
set_package_properties(LibSSH PROPERTIES
    DESCRIPTION "the SSH library with SFTP support"
    URL "http://www.libssh.org/"
    TYPE OPTIONAL
    PURPOSE "Needed to build the SFTP kioslave"
)

macro_optional_find_package(Mtp 1.1.2)
set_package_properties(Mtp PROPERTIES
    DESCRIPTION "the library implementation of the Media Transfer Protocol"
    URL "http://libmtp.sourceforge.net/"
    TYPE OPTIONAL
    PURPOSE "Needed to build the MTP kioslave"
)

macro_optional_find_package(Gphoto2 2.5)
set_package_properties(Gphoto2 PROPERTIES
    DESCRIPTION "Free, redistributable, ready to use set of digital camera software applications"
    URL "http://www.gphoto.org/"
    TYPE OPTIONAL
    PURPOSE "Needed to build camera kioslave"
)

macro_optional_find_package(BZip2)
set_package_properties(BZip2 PROPERTIES
    DESCRIPTION "A high-quality data compressor"
    URL "http://www.bzip.org"
    TYPE OPTIONAL
    PURPOSE "Provides the ability to read and write bzip2 compressed data files in the filter kioslaves"
)

macro_optional_find_package(LibLZMA)
set_package_properties(LibLZMA PROPERTIES
    DESCRIPTION "A very high compression ratio data compressor"
    URL "http://tukaani.org/xz/"
    TYPE OPTIONAL
    PURPOSE "Provides the ability to read and write xz compressed data files"
)

macro_optional_find_package(OpenEXR)
set_package_properties(OpenEXR PROPERTIES
    DESCRIPTION "API for accessing OpenEXR formatted images"
    URL "http://www.openexr.com"
    PURPOSE "Provides support for OpenEXR formatted images in the thumbnail kioslave"
    TYPE OPTIONAL
)

macro_optional_find_package(Sensors)
set_package_properties(Sensors PROPERTIES
    DESCRIPTION "Linux hardware monitoring"
    URL "https://hwmon.wiki.kernel.org/lm_sensors"
    PURPOSE "Sensors feedback in ksysguard"
    TYPE OPTIONAL
)

macro_optional_find_package(Qalculate)
set_package_properties(Qalculate PROPERTIES
    DESCRIPTION "Qalculate Library"
    URL "http://qalculate.sourceforge.net"
    TYPE OPTIONAL
    PURPOSE "Needed to enable advanced features of the calculator runner"
)

macro_optional_find_package(LightDM)
set_package_properties(LightDM PROPERTIES
    DESCRIPTION "Cross-desktop display manager"
    URL "https://github.com/canonical/lightdm"
    TYPE OPTIONAL
    PURPOSE "Needed for the LightDM greeter"
)

find_program(WRESTOOL_EXECUTABLE wrestool)
add_feature_info(wrestool
    WRESTOOL_EXECUTABLE
    "ICO thumbnails support in KIO slave"
)

find_program(MTOOLS_EXECUTABLES NAMES mdir mmd mdel mrd mren mcopy)
add_feature_info(mtools
    MTOOLS_EXECUTABLES
    "KIO floppy support"
)

find_program(QRENCODE_EXECUTABLE qrencode)
add_feature_info(qrencode
    QRENCODE_EXECUTABLE
    "QR Code support in klipper"
)

find_program(XSCREENSAVER_EXECUTABLE xscreensaver)
add_feature_info(xscreensaver
    XSCREENSAVER_EXECUTABLE
    "Screen saver and screen locking support"
)

find_program(GDB_OR_LLDB_EXECUTABLE NAMES gdb lldb)
add_feature_info(gdb_or_lldb
    GDB_OR_LLDB_EXECUTABLE
    "Backtraces support"
)

check_include_files(sys/wait.h HAVE_SYS_WAIT_H)
check_include_files(sys/time.h HAVE_SYS_TIME_H)

include(ConfigureChecks.cmake)
configure_file(config-unix.h.cmake ${CMAKE_BINARY_DIR}/config-unix.h )
configure_file(config-X11.h.cmake ${CMAKE_BINARY_DIR}/config-X11.h )
if(NOT CMAKE_INSTALL_PREFIX STREQUAL "/usr")
  set(EXPORT_XCURSOR_PATH "XCURSOR_PATH=${CMAKE_INSTALL_PREFIX}/share/icons:$XCURSOR_PATH\":~/.icons:/usr/share/icons:/usr/share/pixmaps:/usr/X11R6/lib/X11/icons\"; export XCURSOR_PATH")
endif(NOT CMAKE_INSTALL_PREFIX STREQUAL "/usr")
configure_file(startkde.cmake ${CMAKE_BINARY_DIR}/startkde  @ONLY)
configure_file(config-workspace.h.cmake ${CMAKE_BINARY_DIR}/config-workspace.h )

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES} ${CMAKE_SOURCE_DIR}/libs)

# libs
add_subdirectory(libs)

add_subdirectory(systemsettings)

# core apps
add_subdirectory(kwin)
add_subdirectory(ksmserver)
add_subdirectory(kscreensaver)

add_subdirectory(ksplash)

add_subdirectory(qguiplatformplugin_kde)
add_subdirectory(ksysguard)

add_subdirectory(kcontrol)
add_subdirectory(klipper)
add_subdirectory(kmenuedit)
add_subdirectory(krunner)
add_subdirectory(kstartupconfig)
add_subdirectory(freespacenotifier)
add_subdirectory(kinfocenter)

if(Q_WS_X11 AND X11_Xinput_FOUND)
    add_subdirectory(ktouchpadenabler)
endif(Q_WS_X11 AND X11_Xinput_FOUND)

add_subdirectory(kcminit)
add_subdirectory(khotkeys)
add_subdirectory(ksystraycmd)

if (LightDM_FOUND)
    add_subdirectory(kgreeter)
endif(LightDM_FOUND)

# data
add_subdirectory(cursors)

add_subdirectory(plasma)
add_subdirectory(statusnotifierwatcher)
add_subdirectory(kstyles)

# imported from kde-runtime and other sub-projects
add_subdirectory(menu)
add_subdirectory(knotify)
add_subdirectory(kuiserver)
add_subdirectory(soliduiserver)
add_subdirectory(solidautoeject)
add_subdirectory(solid-actions-kcm)
add_subdirectory(solid-device-automounter)
add_subdirectory(solid-networkstatus)
add_subdirectory(solid-hardware)
add_subdirectory(kcmshell)
add_subdirectory(kioslave)
add_subdirectory(kurifilter-plugins)
add_subdirectory(kquitapp)
if (Q_WS_X11)
    add_subdirectory(kstart)
endif (Q_WS_X11)
# Background processes
add_subdirectory(kdontchangethehostname)
add_subdirectory(kglobalaccel)
add_subdirectory(ktimezoned)
# Command-line tools (e.g. for shell scripts)
add_subdirectory(kdeeject)
add_subdirectory(kfile)
add_subdirectory(kiconfinder)
add_subdirectory(kioclient)
add_subdirectory(kioexec)
add_subdirectory(ktraderclient)
add_subdirectory(kreadconfig)
add_subdirectory(kmimetypefinder)
# UI Helper applications
add_subdirectory(drkonqi)
add_subdirectory(knetattach)
add_subdirectory(kdirshare)
add_subdirectory(keditfiletype)
# Default settings, content and config
add_subdirectory(l10n)
add_subdirectory(localization)
add_subdirectory(kde-menu)

########### install files ###############
# install(PROGRAMS ${CMAKE_CURRENT_SOURCE_DIR}/kde4 DESTINATION ${KDE4_BIN_INSTALL_DIR})
install(PROGRAMS ${CMAKE_BINARY_DIR}/startkde DESTINATION ${KDE4_BIN_INSTALL_DIR})

set(KDE4WORKSPACE_TARGET_PREFIX KDE4Workspace::)

configure_file(
    KDE4WorkspaceConfig.cmake.in
    "${CMAKE_BINARY_DIR}/KDE4WorkspaceConfig.cmake"
    @ONLY
)
configure_file(
    KDE4WorkspaceVersion.cmake.in
    "${CMAKE_BINARY_DIR}/KDE4WorkspaceVersion.cmake"
    @ONLY
)

install(
    FILES
    ${CMAKE_BINARY_DIR}/KDE4WorkspaceConfig.cmake
    ${CMAKE_BINARY_DIR}/KDE4WorkspaceVersion.cmake
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/cmake/modules
)

install(
    EXPORT kdeworkspaceLibraryTargets 
    NAMESPACE ${KDE4WORKSPACE_TARGET_PREFIX}
    DESTINATION ${KDE4_DATA_INSTALL_DIR}/cmake/modules
    FILE KDE4WorkspaceLibraryTargets.cmake
)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
