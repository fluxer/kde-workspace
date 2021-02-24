project(kde-workspace)

include(CheckIncludeFiles)
include(CheckFunctionExists)
include(GenerateExportHeader)

find_package(KDE4 4.19.0 REQUIRED)
include(KDE4Defaults)

option(WITH_XINERAMA "Xinerama support for multi-headed X displays" ON)

find_package(ZLIB)
set_package_properties(ZLIB PROPERTIES
    DESCRIPTION "Support for gzip compressed files and data streams"
    URL "http://www.zlib.net"
    TYPE REQUIRED
)

macro_optional_find_package(DBusMenuQt 0.6.0)
set_package_properties(DBusMenuQt PROPERTIES
    DESCRIPTION "Support for notification area menus via the DBusMenu protocol"
    URL "https://launchpad.net/libdbusmenu-qt"
    TYPE RECOMMENDED
)

macro_optional_find_package(Strigi 0.6.3)
set_package_properties(Strigi PROPERTIES
    DESCRIPTION "Desktop indexing and search support"
    URL "http://strigi.sourceforge.net"
    PURPOSE "Fonts analyzer"
    TYPE RECOMMENDED
)

macro_optional_find_package(OpenGL)
set_package_properties(OpenGL PROPERTIES
    DESCRIPTION "The OpenGL libraries"
    URL "http://www.opengl.org"
    PURPOSE "3D screensavers and OpenGL info module"
    TYPE OPTIONAL
)

macro_optional_find_package(OpenGLES)
set_package_properties(OpenGLES PROPERTIES
    DESCRIPTION "The OpenGLES libraries"
    URL "http://www.khronos.org/opengles"
    TYPE OPTIONAL
    PURPOSE "OpenGL ES info module"
)

macro_optional_find_package(LibDRM)
set_package_properties(LibDRM PROPERTIES
    DESCRIPTION "Userspace interface to kernel DRM services"
    URL "https://dri.freedesktop.org/wiki/"
    TYPE OPTIONAL
    PURPOSE "OpenGL info module"
)

macro_optional_find_package(UDev)
set_package_properties(UDev PROPERTIES
    DESCRIPTION "The UDev Libraries"
    PURPOSE "Allows support for UPower backend in PowerDevil - STRONGLY RECOMMENDED"
    TYPE OPTIONAL
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

    add_feature_info("Automated testing of X clients" X11_XTest_FOUND
                    "The X11 Testing Resource extension library is useful for automated testing of X clients")
    add_feature_info("libXau" X11_Xau_FOUND "The X11 Authorization Protocol library may be used by KDM")
    add_feature_info("LibXdmcp" X11_Xdmcp_FOUND "The X Display Manager Control Protocol library may be used by KDM")

    if(NOT X11_Xdamage_FOUND)
        message(FATAL_ERROR "The X11 damaged region extension library was not found. Required for compositing support in KWin.")
    endif()
    if(NOT X11_Xrender_FOUND)
        message(FATAL_ERROR " The X Rendering Extension client library was not found. Required for XRender Compositing backend in KWin.")
    endif()
    if(NOT X11_Xfixes_FOUND)
        message(FATAL_ERROR "The X11 miscellaneous 'fixes' extension library was not found. Required for XRender Compositing backend in KWin.")
    endif()
    if(NOT X11_Xrandr_FOUND)
        message(FATAL_ERROR "The X11 RandR extension library was not found. Required for Multi Screen Support.")
    endif()
    if(NOT X11_Xcursor_FOUND)
        message(FATAL_ERROR "The X11 cursor management library was not found. Required for desktop effects support in KWin.")
    endif()
endif(Q_WS_X11)

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

find_package(LibGcrypt 1.5.0)
set_package_properties(LibGcrypt PROPERTIES
    DESCRIPTION "general purpose cryptographic library based on the code from GnuPG"
    URL "https://www.gnu.org/software/libgcrypt/"
    PURPOSE "kwalletd needs libgcrypt to perform PBKDF2-SHA512 hashing"
    TYPE REQUIRED
)

find_package(JPEG)
set_package_properties(JPEG PROPERTIES
    DESCRIPTION "Accelerated JPEG image codec"
    URL "http://libjpeg-turbo.virtualgl.org/"
    TYPE REQUIRED
)

find_package(PNG)
set_package_properties(PNG PROPERTIES
    DESCRIPTION "An Open, Extensible Image Format with Lossless Compression"
    URL "http://www.libpng.org/"
    TYPE REQUIRED
)

macro_optional_find_package(LibGPS)
set_package_properties(LibGPS PROPERTIES
    DESCRIPTION "GPSes/AIS service library"
    URL "http://www.catb.org/gpsd/"
    PURPOSE "GPS support for geolocation in plasma dataengine"
    TYPE OPTIONAL
)

macro_optional_find_package(LibKonq)
set_package_properties(LibKonq PROPERTIES
    DESCRIPTION "Konqueror library"
    URL "http://fluxer.github.io/katana/"
    PURPOSE "folderview plasma applets"
    TYPE OPTIONAL
)

find_package(Perl)
set_package_properties(Perl PROPERTIES
    DESCRIPTION "Highly capable, feature-rich programming language"
    URL "http://www.perl.org"
    PURPOSE "Needed for KDM config handler generator"
    TYPE REQUIRED
)

set(POLKITQT-1_MIN_VERSION "0.99.0")
macro_optional_find_package(PolkitQt-1)
set_package_properties(PolkitQt-1 PROPERTIES
    DESCRIPTION "Qt wrapper around polkit-1 client libraries"
    URL "https://cgit.kde.org/polkit-qt-1.git/"
    PURPOSE "polkit-1 agent"
    TYPE OPTIONAL
)

macro_optional_find_package(LibAttica 0.1.4)
set_package_properties(LibAttica PROPERTIES
    DESCRIPTION "Attica Library"
    URL "https://github.com/fluxer/attica"
    PURPOSE "Attica is needed for KDE specific Open Collaboration Service providers"
    TYPE OPTIONAL
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
# pictures
add_subdirectory(pics)

macro_optional_add_subdirectory(systemsettings )

# core apps
macro_optional_add_subdirectory(kcheckpass)
macro_optional_add_subdirectory(kwin)
macro_optional_add_subdirectory(ksmserver)

macro_optional_add_subdirectory(ksplash)

macro_optional_add_subdirectory(powerdevil)
macro_optional_add_subdirectory(qguiplatformplugin_kde)
macro_optional_add_subdirectory(ksysguard)

macro_optional_add_subdirectory(kcontrol)
macro_optional_add_subdirectory(klipper)
macro_optional_add_subdirectory(kmenuedit)
macro_optional_add_subdirectory(krunner)
macro_optional_add_subdirectory(solid-actions-kcm)
macro_optional_add_subdirectory(kstartupconfig)
macro_optional_add_subdirectory(freespacenotifier)
macro_optional_add_subdirectory(kscreensaver)
macro_optional_add_subdirectory(kinfocenter)

if(Q_WS_X11 AND X11_Xinput_FOUND)
    macro_optional_add_subdirectory(ktouchpadenabler)
endif(Q_WS_X11 AND X11_Xinput_FOUND)

macro_optional_add_subdirectory(kcminit)
macro_optional_add_subdirectory(khotkeys)
macro_optional_add_subdirectory(kwrited)
macro_optional_add_subdirectory(ksystraycmd)
macro_optional_add_subdirectory(appmenu)

if(X11_Xau_FOUND AND X11_Xdmcp_FOUND)
    macro_optional_add_subdirectory( kdm )
else(X11_Xau_FOUND AND X11_Xdmcp_FOUND)
    message(STATUS "Xau lib or Xdmcp lib was missing. kdm will not compile")
endif(X11_Xau_FOUND AND X11_Xdmcp_FOUND)

# data
macro_optional_add_subdirectory(cursors)

macro_optional_add_subdirectory(plasma)
macro_optional_add_subdirectory(statusnotifierwatcher)
macro_optional_add_subdirectory(kstyles)

# imported from kde-runtime and other sub-projects
macro_optional_add_subdirectory(menu)
macro_optional_add_subdirectory(kwalletd)
macro_optional_add_subdirectory(kwalletmanager)
macro_optional_add_subdirectory(knotify)
macro_optional_add_subdirectory(kuiserver)
macro_optional_add_subdirectory(soliduiserver)
macro_optional_add_subdirectory(solidautoeject)
macro_optional_add_subdirectory(solid-device-automounter)
macro_optional_add_subdirectory(solid-networkstatus)
macro_optional_add_subdirectory(kcmshell)
macro_optional_add_subdirectory(kioslave)
macro_optional_add_subdirectory(kurifilter-plugins)
macro_optional_add_subdirectory(renamedlgplugins)
macro_optional_add_subdirectory(kquitapp)
if (Q_WS_X11)
    macro_optional_add_subdirectory(kstart)
endif (Q_WS_X11)
if (POLKITQT-1_FOUND)
    macro_optional_add_subdirectory(polkit-kde-agent)
endif(POLKITQT-1_FOUND)
# Background processes
macro_optional_add_subdirectory(kpasswdserver)
macro_optional_add_subdirectory(kdontchangethehostname)
macro_optional_add_subdirectory(kglobalaccel)
macro_optional_add_subdirectory(ktimezoned)
# Command-line tools (e.g. for shell scripts)
macro_optional_add_subdirectory(kdeeject)
macro_optional_add_subdirectory(kfile)
macro_optional_add_subdirectory(kiconfinder)
macro_optional_add_subdirectory(kioclient)
macro_optional_add_subdirectory(kioexec)
macro_optional_add_subdirectory(ktraderclient)
macro_optional_add_subdirectory(kreadconfig)
macro_optional_add_subdirectory(kmimetypefinder)
# UI Helper applications
macro_optional_add_subdirectory(drkonqi)
macro_optional_add_subdirectory(knetattach)
macro_optional_add_subdirectory(keditfiletype)
# Default settings, content and config
macro_optional_add_subdirectory(l10n)
macro_optional_add_subdirectory(localization)
macro_optional_add_subdirectory(kde-menu)
# Integration for attica
if(LIBATTICA_FOUND)
    macro_optional_add_subdirectory(attica)
endif()

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
