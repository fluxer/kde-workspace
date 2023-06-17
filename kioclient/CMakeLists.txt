add_definitions(-DKDE_DEFAULT_DEBUG_AREA=101)

set(kioclient_SRCS kioclient.cpp )
add_executable(kioclient ${kioclient_SRCS})
target_compile_definitions(kioclient PRIVATE -DKIOCLIENT_AS_KIOCLIENT)
target_link_libraries(kioclient ${KDE4_KIO_LIBS})
install(
    TARGETS kioclient
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)

################

set(kde-open_SRCS kioclient.cpp )
add_executable(kde-open ${kde-open_SRCS})
target_compile_definitions(kde-open PRIVATE -DKIOCLIENT_AS_KDEOPEN)
target_link_libraries(kde-open ${KDE4_KIO_LIBS})
install(
    TARGETS kde-open
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)

################

set(kde-cp_SRCS kioclient.cpp )
add_executable(kde-cp ${kde-cp_SRCS})
target_compile_definitions(kde-cp PRIVATE -DKIOCLIENT_AS_KDECP)
target_link_libraries(kde-cp ${KDE4_KIO_LIBS})
install(
    TARGETS kde-cp
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)

################

set(kde-mv_SRCS kioclient.cpp )
add_executable(kde-mv ${kde-mv_SRCS})
target_compile_definitions(kde-mv PRIVATE -DKIOCLIENT_AS_KDEMV)
target_link_libraries(kde-mv ${KDE4_KIO_LIBS})
install(
    TARGETS kde-mv
    DESTINATION ${KDE4_BIN_INSTALL_DIR}
)

