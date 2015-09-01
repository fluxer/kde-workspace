set( EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR} )


include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/.. )
########### next target ###############
set(testicons_SRCS testicons.cpp ${CMAKE_CURRENT_SOURCE_DIR}/../icons.cpp )


add_executable(testicons TEST ${testicons_SRCS})

target_link_libraries(testicons ${KDE4_KDEUI_LIBS})


