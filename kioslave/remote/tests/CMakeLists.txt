set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )

set(libkioremote_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/../kio_remote.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../remoteimpl.cpp )
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
########### next target ###############
set(testremote_SRCS testremote.cpp ${libkioremote_SRCS} )


kde4_add_manual_test(testremote ${testremote_SRCS})

target_link_libraries(testremote  ${KDE4_KIO_LIBS} )





