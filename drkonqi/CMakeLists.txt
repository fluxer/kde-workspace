project(drkonqi)

check_function_exists("strsignal" HAVE_STRSIGNAL)
check_function_exists("uname" HAVE_UNAME)

if(NOT DEBUG_PACKAGE_INSTALLER_NAME)
    set (DEBUG_PACKAGE_INSTALLER_NAME "installdbgsymbols.sh")
endif()

configure_file(
    config-drkonqi.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/config-drkonqi.h
)

add_definitions(-DKDE_DEFAULT_DEBUG_AREA=1410)

add_subdirectory(data)

set(drkonqi_SRCS
    main.cpp
    drkonqidialog.cpp
    statuswidget.cpp
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
    gdbhighlighter.cpp
)

set(backtrace_parser_SRCS
    parser/backtraceparser.cpp
    parser/backtraceparsergdb.cpp
    parser/backtraceparsernull.cpp
)

if(ENABLE_TESTING)
    add_library(drkonqi_backtrace_parser STATIC ${backtrace_parser_SRCS})
    add_subdirectory(tests)
endif()

# if BACKTRACE_PARSER_DEBUG is enabled, it will show both the
# parsed and the unparsed backtrace in the backtrace widget.
# Comment this out for release.
#add_definitions(-DBACKTRACE_PARSER_DEBUG)

add_executable(drkonqi ${drkonqi_SRCS} ${backtrace_parser_SRCS})

target_link_libraries(drkonqi
    ${KDE4_KIO_LIBS}
)

install(TARGETS drkonqi DESTINATION ${LIBEXEC_INSTALL_DIR})
