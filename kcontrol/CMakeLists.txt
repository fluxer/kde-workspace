
macro_optional_find_package(Freetype)
set_package_properties(Freetype PROPERTIES DESCRIPTION "A font rendering engine"
                       URL "http://www.freetype.org"
                       TYPE OPTIONAL
                       PURPOSE "Needed to build kfontinst, a simple font installer."
                      )


set(libkxftconfig_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/fonts/kxftconfig.cpp )


if( X11_Xrandr_FOUND )
   add_subdirectory( randr )
endif(X11_Xrandr_FOUND )

if(X11_Xkb_FOUND)
    add_subdirectory( keyboard )
endif(X11_Xkb_FOUND)

add_subdirectory( display )
add_subdirectory( mouse )

if (CMAKE_SYSTEM_NAME MATCHES Linux)
   # this one doesn't seem to be very portable, Alex
   add_subdirectory( joystick ) 
endif (CMAKE_SYSTEM_NAME MATCHES Linux)

add_subdirectory( access )
add_subdirectory( screensaver )
add_subdirectory( dateandtime )
add_subdirectory( autostart )

add_subdirectory( launch )
add_subdirectory( colors )
add_subdirectory( krdb )
add_subdirectory( style )
add_subdirectory( desktoptheme )
add_subdirectory( splash )
add_subdirectory( standard_actions )
add_subdirectory( keys )
add_subdirectory( workspaceoptions )

add_subdirectory( icons )
add_subdirectory( locale )
add_subdirectory( kded )
add_subdirectory( knotify )
add_subdirectory( componentchooser )
add_subdirectory( menus )
add_subdirectory( dnssd )
add_subdirectory( emoticons )
add_subdirectory( spellchecking )

add_subdirectory( kdebug )

# TODO needs porting
#add_subdirectory( smartcard )

add_subdirectory( desktoppaths )

if(FONTCONFIG_FOUND AND FREETYPE_FOUND)
    add_subdirectory( kfontinst )
    add_subdirectory( fonts )
endif(FONTCONFIG_FOUND AND FREETYPE_FOUND)
