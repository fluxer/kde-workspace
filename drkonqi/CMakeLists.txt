project(drkonqi)

check_function_exists("strsignal" HAVE_STRSIGNAL)
check_function_exists("uname" HAVE_UNAME)

if (NOT DEBUG_PACKAGE_INSTALLER_NAME)
    set (DEBUG_PACKAGE_INSTALLER_NAME "installdbgsymbols.sh")
endif (NOT DEBUG_PACKAGE_INSTALLER_NAME)

configure_file (config-drkonqi.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-drkonqi.h )

include_directories(${KDEPIMLIBS_INCLUDE_DIRS})

add_definitions(-DKDE_DEFAULT_DEBUG_AREA=1410)

add_subdirectory( data )
add_subdirectory( parser )
add_subdirectory( tests )
if ( WIN32 )
    add_subdirectory( kdbgwin )
endif ( WIN32 )

set(drkonqi_SRCS
    main.cpp
    drkonqidialog.cpp
    statuswidget.cpp
    aboutbugreportingdialog.cpp
    reportassistantdialog.cpp
    reportassistantpage.cpp
    reportassistantpages_base.cpp
    reportinterface.cpp
    backtraceratingwidget.cpp
    backtracewidget.cpp
    backtracegenerator.cpp
    drkonqi.cpp
    drkonqibackends.cpp
    detachedprocessmonitor.cpp
    debugpackageinstaller.cpp
    systeminformation.cpp
    crashedapplication.cpp
    debugger.cpp
    debuggerlaunchers.cpp
    debuggermanager.cpp
    applicationdetailsexamples.cpp
    gdbhighlighter.cpp
)

kde4_add_ui_files(drkonqi_SRCS
    ui/maindialog.ui
    ui/backtracewidget.ui
    ui/assistantpage_introduction.ui
    ui/assistantpage_bugawareness.ui
    ui/assistantpage_conclusions.ui
    ui/assistantpage_conclusions_dialog.ui
)

# if BACKTRACE_PARSER_DEBUG is enabled, it will show both the
# parsed and the unparsed backtrace in the backtrace widget.
# Comment this out for release.
#add_definitions(-DBACKTRACE_PARSER_DEBUG)

kde4_add_executable(drkonqi ${drkonqi_SRCS})

target_link_libraries(drkonqi
    ${KDE4_KIO_LIBS}
    ${QT_QTWEBKIT_LIBRARY}
    ${KDE4_KDEWEBKIT_LIBRARY}
    ${KDEPIMLIBS_KXMLRPCCLIENT_LIBRARY}
    drkonqi_backtrace_parser
)

install(TARGETS drkonqi DESTINATION ${LIBEXEC_INSTALL_DIR})

install(FILES drkonqi-rename-config-section.upd DESTINATION ${KCONF_UPDATE_INSTALL_DIR})
