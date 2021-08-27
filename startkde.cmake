#!/bin/sh
#
#  DEFAULT KDE STARTUP SCRIPT ( @KDE_VERSION_STRING@ )
#

if test "x$1" = x--failsafe; then
    KDE_FAILSAFE=1 # General failsafe flag
    KWIN_COMPOSE=N # Disable KWin's compositing
    export KWIN_COMPOSE KDE_FAILSAFE
fi

# When the X server dies we get a HUP signal from xinit. We must ignore it
# because we still need to do some cleanup.
trap 'echo GOT SIGHUP' HUP

# in case we have been started with full pathname spec without being in PATH
if test -n "$PATH" ; then
    qdbus=$(basename @QT_QDBUS_EXECUTABLE@)
else
    qdbus=@QT_QDBUS_EXECUTABLE@
fi

# Check if a KDE session already is running and whether it's possible to connect to X
kcheckrunning
kcheckrunning_result=$?
if test $kcheckrunning_result -eq 0 ; then
	echo "KDE seems to be already running on this display."
	xmessage -geometry 500x100 "KDE seems to be already running on this display." > /dev/null 2>/dev/null
	exit 1
elif test $kcheckrunning_result -eq 2 ; then
	echo "\$DISPLAY is not set or cannot connect to the X server."
        exit 1
fi

# Boot sequence:
#
# kdeinit is used to fork off processes which improves memory usage
# and startup time.
#
# * kdeinit starts klauncher first.
# * Then kded is started. kded is responsible for keeping the sycoca
#   database up to date. When an up to date database is present it goes
#   into the background and the startup continues.
# * Then kdeinit starts kcminit. kcminit performs initialisation of
#   certain devices according to the user's settings
#
# * Then ksmserver is started which takes control of the rest of the startup sequence

# The user's personal KDE directory is usually ~/.kde, but this setting
# may be overridden by setting KDEHOME.

kdehome=$HOME/@KDE_DEFAULT_HOME@
test -n "$KDEHOME" && kdehome=`echo "$KDEHOME"|sed "s,^~/,$HOME/,"`

# see kstartupconfig source for usage
mkdir -m 700 -p $kdehome
mkdir -m 700 -p $kdehome/share
mkdir -m 700 -p $kdehome/share/config
cat >$kdehome/share/config/startupconfigkeys <<EOF
kcminputrc Mouse cursorTheme 'Oxygen_White'
kcminputrc Mouse cursorSize ''
ksplashrc KSplash Theme Default
ksplashrc KSplash Engine KSplashX
krandrrc Display ApplyOnStartup false
krandrrc Display StartupCommands ''
krandrrc [Screen0]
krandrrc [Screen1]
krandrrc [Screen2]
krandrrc [Screen3]
kcmfonts General forceFontDPI 0
kdeglobals Locale Language '' # trigger requesting languages from KLocale
EOF
kstartupconfig4
returncode=$?
if test $returncode -ne 0; then
    xmessage -geometry 500x100 "kstartupconfig4 does not exist or fails. The error code is $returncode. Check your installation."
    exit 1
fi
[ -r $kdehome/share/config/startupconfig ] && . $kdehome/share/config/startupconfig

# XCursor mouse theme needs to be applied here to work even for kded or ksmserver
if test -n "$kcminputrc_mouse_cursortheme" -o -n "$kcminputrc_mouse_cursorsize" ; then
    @EXPORT_XCURSOR_PATH@

    kapplymousetheme "$kcminputrc_mouse_cursortheme" "$kcminputrc_mouse_cursorsize"
    if test $? -eq 10; then
        XCURSOR_THEME=default
        export XCURSOR_THEME
    elif test -n "$kcminputrc_mouse_cursortheme"; then
        XCURSOR_THEME="$kcminputrc_mouse_cursortheme"
        export XCURSOR_THEME
    fi
    if test -n "$kcminputrc_mouse_cursorsize"; then
        XCURSOR_SIZE="$kcminputrc_mouse_cursorsize"
        export XCURSOR_SIZE
    fi
fi

[ -r @KDE4_BIN_INSTALL_DIR@/krandrstartup ] && . @KDE4_BIN_INSTALL_DIR@/krandrstartup

if test "$kcmfonts_general_forcefontdpi" -ne 0; then
    xrdb -quiet -merge -nocpp <<EOF
Xft.dpi: $kcmfonts_general_forcefontdpi
EOF
fi


dl=$DESKTOP_LOCKED
unset DESKTOP_LOCKED # Don't want it in the environment

ksplash_pid=
if test -z "$dl"; then
  # languages as resolved by KLocale, for the splash screens use
  # klocale_languages is assembled by kdostartupconfig4 calling KLocale
  KLOCALE_LANGUAGES="$klocale_languages"
  export KLOCALE_LANGUAGES
  # the splashscreen and progress indicator
  case "$ksplashrc_ksplash_engine" in
    KSplashX)
      ksplash_pid=`ksplashx "${ksplashrc_ksplash_theme}" --pid`
      ;;
    KSplashQML)
      ksplash_pid=`ksplashqml "${ksplashrc_ksplash_theme}" --pid`
      ;;
    None)
      ;;
    *)
      ;;
  esac
  # no longer needed in the environment
  unset KLOCALE_LANGUAGES
fi

# Set the path for Qt plugins provided by KDE
QT_PLUGIN_PATH=${QT_PLUGIN_PATH+$QT_PLUGIN_PATH:}`kde4-config --path qtplugins`
export QT_PLUGIN_PATH

# Set the platform plugin for Qt to the one provided by KDE
QT_PLATFORM_PLUGIN=kde
export QT_PLATFORM_PLUGIN

# Set a left cursor instead of the standard X11 "X" cursor, since I've heard
# from some users that they're confused and don't know what to do. This is
# especially necessary on slow machines, where starting KDE takes one or two
# minutes until anything appears on the screen.
#
# If the user has overwritten fonts, the cursor font may be different now
# so don't move this up.
#
xsetroot -cursor_name left_ptr

lnusertemp=`kde4-config --path exe --locate lnusertemp`
if test -z "$lnusertemp"; then
  # Startup error
  echo 'startkde: ERROR: Could not locate lnusertemp in '`kde4-config --path exe` 1>&2
fi

# Link "tmp" "socket" and "cache" resources to directory in /tmp
# Creates:
# - a directory /tmp/kde-$USER and links $KDEHOME/tmp-$HOSTNAME to it.
# - a directory /tmp/ksocket-$USER and links $KDEHOME/socket-$HOSTNAME to it.
# - a directory /var/tmp/kdecache-$USER and links $KDEHOME/cache-$HOSTNAME to it.
# Note: temporary locations can be overriden through the TMPDIR and KDEVARTMP
# environment variables
for resource in tmp cache socket; do
    if "$lnusertemp" $resource >/dev/null; then
        : # ok
    else
        echo 'startkde: Call to lnusertemp failed (temporary directories full?). Check your installation.'  1>&2
        test -n "$ksplash_pid" && kill "$ksplash_pid" 2>/dev/null
        xmessage -geometry 600x100 "Call to lnusertemp failed (temporary directories full?). Check your installation."
        exit 1
    fi
done

# In case of dcop sockets left by a previous session, cleanup
#dcopserver_shutdown

echo 'startkde: Starting up...'  1>&2

# Make sure that the KDE prefix is first in XDG_DATA_DIRS and that it's set at all.
# The spec allows XDG_DATA_DIRS to be not set, but X session startup scripts tend
# to set it to a list of paths *not* including the KDE prefix if it's not /usr or
# /usr/local.
if test -z "$XDG_DATA_DIRS"; then
    XDG_DATA_DIRS="@KDE4_SHARE_INSTALL_PREFIX@:/usr/share:/usr/local/share"
else
    XDG_DATA_DIRS="@KDE4_SHARE_INSTALL_PREFIX@:$XDG_DATA_DIRS"
fi
export XDG_DATA_DIRS

# Make sure that D-Bus is running
# D-Bus autolaunch is broken
if test -z "$DBUS_SESSION_BUS_ADDRESS" ; then
    eval `dbus-launch --sh-syntax --exit-with-session`
fi
if $qdbus >/dev/null 2>/dev/null; then
    : # ok
else
    echo 'startkde: Could not start D-Bus. Can you call qdbus?'  1>&2
    test -n "$ksplash_pid" && kill "$ksplash_pid" 2>/dev/null
    xmessage -geometry 500x100 "Could not start D-Bus. Can you call qdbus?"
    exit 1
fi


# Mark that full KDE session is running (e.g. Konqueror preloading works only
# with full KDE running). The KDE_FULL_SESSION property can be detected by
# any X client connected to the same X session, even if not launched
# directly from the KDE session but e.g. using "ssh -X", kdesudo. $KDE_FULL_SESSION
# however guarantees that the application is launched in the same environment
# like the KDE session and that e.g. KDE utilities/libraries are available.
# KDE_FULL_SESSION property is also only available since KDE 3.5.5.
# The matching tests are:
#   For $KDE_FULL_SESSION:
#     if test -n "$KDE_FULL_SESSION"; then ... whatever
#   For KDE_FULL_SESSION property:
#     xprop -root | grep "^KDE_FULL_SESSION" >/dev/null 2>/dev/null
#     if test $? -eq 0; then ... whatever
#
# Additionally there is (since KDE 3.5.7) $KDE_SESSION_UID with the uid
# of the user running the KDE session. It should be rarely needed (e.g.
# after sudo to prevent desktop-wide functionality in the new user's kded).
#
# Since KDE4 there is also KDE_SESSION_VERSION, containing the major version number.
# Note that this didn't exist in KDE3, which can be detected by its absense and
# the presence of KDE_FULL_SESSION.
#
KDE_FULL_SESSION=true
export KDE_FULL_SESSION
xprop -root -f KDE_FULL_SESSION 8t -set KDE_FULL_SESSION true

KDE_SESSION_VERSION=4
export KDE_SESSION_VERSION
xprop -root -f KDE_SESSION_VERSION 32c -set KDE_SESSION_VERSION 4

KDE_SESSION_UID=`id -ru`
export KDE_SESSION_UID

XDG_CURRENT_DESKTOP=KDE
export XDG_CURRENT_DESKTOP

# At this point all the environment is ready, let's send it to kwalletd if running
if test -n "$PAM_KWALLET_LOGIN" ; then
    env | socat STDIN UNIX-CONNECT:$PAM_KWALLET_LOGIN
fi

# We set LD_BIND_NOW to increase the efficiency of kdeinit.
# kdeinit unsets this variable before loading applications.
LD_BIND_NOW=true @KDE4_BIN_INSTALL_DIR@/kdeinit4 +kcminit_startup
if test $? -ne 0; then
  # Startup error
  echo 'startkde: Could not start kdeinit4. Check your installation.'  1>&2
  test -n "$ksplash_pid" && kill "$ksplash_pid" 2>/dev/null
  xmessage -geometry 500x100 "Could not start kdeinit4. Check your installation."
  exit 1
fi

# finally, give the session control to the session manager
# see kdebase/ksmserver for the description of the rest of the startup sequence
# if the KDEWM environment variable has been set, then it will be used as KDE's
# window manager instead of kwin.
# if KDEWM is not set, ksmserver will ensure kwin is started.
# kwrapper4 is used to reduce startup time and memory usage
# kwrapper4 does not return useful error codes such as the exit code of ksmserver.
# We only check for 255 which means that the ksmserver process could not be
# started, any problems thereafter, e.g. ksmserver failing to initialize,
# will remain undetected.
test -n "$KDEWM" && KDEWM="--windowmanager $KDEWM"
# If the session should be locked from the start (locked autologin),
# lock now and do the rest of the KDE startup underneath the locker.
KSMSERVEROPTIONS=""
test -n "$dl" && KSMSERVEROPTIONS=" --lockscreen"
kwrapper4 ksmserver $KDEWM $KSMSERVEROPTIONS
if test $? -eq 255; then
  # Startup error
  echo 'startkde: Could not start ksmserver. Check your installation.'  1>&2
  test -n "$ksplash_pid" && kill "$ksplash_pid" 2>/dev/null
  xmessage -geometry 500x100 "Could not start ksmserver. Check your installation."
fi

wait_drkonqi=`kreadconfig --file startkderc --group WaitForDrKonqi --key Enabled --default true`

if test x"$wait_drkonqi"x = x"true"x ; then
    # wait for remaining drkonqi instances with timeout (in seconds)
    wait_drkonqi_timeout=`kreadconfig --file startkderc --group WaitForDrKonqi --key Timeout --default 900`
    wait_drkonqi_counter=0
    while $qdbus | grep "^[^w]*org.kde.drkonqi" > /dev/null ; do
        sleep 5
        wait_drkonqi_counter=$((wait_drkonqi_counter+5))
        if test "$wait_drkonqi_counter" -ge "$wait_drkonqi_timeout" ; then
            # ask remaining drkonqis to die in a graceful way
            $qdbus | grep 'org.kde.drkonqi-' | while read address ; do
                $qdbus "$address" "/MainApplication" "quit"
            done
            break
        fi
    done
fi

echo 'startkde: Shutting down...'  1>&2
# just in case
test -n "$ksplash_pid" && kill "$ksplash_pid" 2>/dev/null

# Clean up
kdeinit4_shutdown

unset KDE_FULL_SESSION
xprop -root -remove KDE_FULL_SESSION
unset KDE_SESSION_VERSION
xprop -root -remove KDE_SESSION_VERSION
unset KDE_SESSION_UID

echo 'startkde: Done.'  1>&2
