set(backtraceparsertest_SRCS
    fakebacktracegenerator.cpp
    backtraceparsertest.cpp
)

kde4_add_test(drkonqi-backtraceparsertest ${backtraceparsertest_SRCS})
target_link_libraries(drkonqi-backtraceparsertest
    ${QT_QTTEST_LIBRARY}
    ${QT_QTCORE_LIBRARY}
    drkonqi_backtrace_parser
)

# create a link so that the data directory can be accessed from the build
# directory, where the test runs
if(NOT ${CMAKE_CURRENT_BINARY_DIR} EQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    add_custom_command(
        TARGET drkonqi-backtraceparsertest POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E create_symlink
        ${CMAKE_CURRENT_SOURCE_DIR}/backtraceparsertest_data
        backtraceparsertest_data
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
endif()
