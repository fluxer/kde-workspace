configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-kxsconfig.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-kxsconfig.h )

########### next target ###############

set(kxsconfig_SRCS 
   kxscontrol.cpp 
   kxsitem.cpp 
   kxsxml.cpp 
   kxsconfig.cpp )


kde4_add_executable(kxsconfig ${kxsconfig_SRCS})

target_link_libraries(kxsconfig ${KDE4_KIO_LIBS} ${X11_X11_LIB} ${X11_Xt_LIB})

install(TARGETS kxsconfig  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### next target ###############

set(kxsrun_SRCS 
   kxsrun.cpp 
   kxscontrol.cpp 
   kxsitem.cpp 
   kxsxml.cpp )

kde4_add_executable(kxsrun ${kxsrun_SRCS})

target_link_libraries(kxsrun  ${KDE4_KIO_LIBS})

install(TARGETS kxsrun  ${INSTALL_TARGETS_DEFAULT_ARGS} )


########### install files ###############

FILE(GLOB _desktopfiles "ScreenSavers/*.desktop" )
foreach(_currentdesktopfile ${_desktopfiles})
	STRING(REGEX REPLACE ".desktop" "" _newitem "${_currentdesktopfile}" )
	#MESSAGE(STATUS "newItem <${_newitem}>")
	GET_FILENAME_COMPONENT(_screensaverName ${_newitem} NAME_WE)
	#MESSAGE(STATUS "name without extension <${_screensaverName}>")
	find_file(XSCREENSAVER_FILE_FOUND ${_screensaverName}.xml PATHS ${XSCREENSAVER_CONFIG_DIR} )
	if(XSCREENSAVER_FILE_FOUND)
		MESSAGE(STATUS "xscreensaver name ${_screensaverName} found")
		install(FILES ${_currentdesktopfile} DESTINATION  ${SERVICES_INSTALL_DIR}/ScreenSavers )
	else(XSCREENSAVER_FILE_FOUND)
		MESSAGE(STATUS "xscreensaver name ${_screensaverName} not found")
	endif(XSCREENSAVER_FILE_FOUND)
	unset(XSCREENSAVER_FILE_FOUND CACHE)
endforeach(_currentdesktopfile ${_desktopfiles})
