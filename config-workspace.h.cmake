/* config-workspace.h.  Generated by cmake from config-workspace.h.cmake  */

/* Define if you have DPMS support */
#cmakedefine HAVE_DPMS 1

/* Define if you have the DPMSCapable prototype in <X11/extensions/dpms.h> */
#cmakedefine HAVE_DPMSCAPABLE_PROTO 1

/* Define if you have the DPMSInfo prototype in <X11/extensions/dpms.h> */
#cmakedefine HAVE_DPMSINFO_PROTO 1

/* Defines if your system has the libfontconfig library */
#cmakedefine HAVE_FONTCONFIG 1

/* Defines if your system has the freetype library */
#cmakedefine HAVE_FREETYPE 1

/* Define to 1 if you have the `nice' function. */
#cmakedefine HAVE_NICE 1

/* Define to 1 if you have the `getpassphrase' function. */
#cmakedefine HAVE_GETPASSPHRASE 1

/* Define to 1 if you have the `vsyslog' function. */
#cmakedefine HAVE_VSYSLOG 1

/* Define to 1 if you have the `setpriority' function. */
#cmakedefine HAVE_SETPRIORITY 1

/* Define to 1 if you have the <string.h> header file. */
#cmakedefine HAVE_STRING_H 1

/* Define to 1 if you have the <sys/select.h> header file. */
#cmakedefine HAVE_SYS_SELECT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#cmakedefine HAVE_SYS_TIME_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#cmakedefine HAVE_UNISTD_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#cmakedefine HAVE_STDINT_H 1

/* Define to 1 if you have the <limits.h> header file. */
#cmakedefine HAVE_LIMITS_H 1

/* Define to 1 if you have the <malloc.h> header file. */
#cmakedefine HAVE_MALLOC_H 1

/* KDE's default home directory */
#cmakedefine KDE_DEFAULT_HOME "${KDE_DEFAULT_HOME}"

/* KDE's binaries directory */
#define KDE_BINDIR "${KDE4_BIN_INSTALL_DIR}"

/* KDE's configuration directory */
#define KDE_CONFDIR "${KDE4_CONFIG_INSTALL_DIR}"

/* KDE's static data directory */
#define KDE_DATADIR "${KDE4_DATA_INSTALL_DIR}"

/* KDE's static shared data directory */
#define KDE_SHAREDIR "${KDE4_SHARE_INSTALL_PREFIX}"

/* X binaries directory */
#cmakedefine XBINDIR "${XBINDIR}"

/* X libraries directory */
#cmakedefine XLIBDIR "${XLIBDIR}"

/* xkb resources directory */
#cmakedefine XKBDIR "${XKBDIR}"
