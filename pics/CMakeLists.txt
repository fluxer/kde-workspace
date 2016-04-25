add_subdirectory( emoticons )

set(ksvgtopng_SRCS ksvgtopng.cpp )
add_executable(ksvgtopng ${ksvgtopng_SRCS})
target_link_libraries(ksvgtopng ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTSVG_LIBRARY})
install(TARGETS ksvgtopng ${INSTALL_TARGETS_DEFAULT_ARGS} )
