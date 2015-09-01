set(kioclient_SRCS kioclient.cpp )
add_executable(kioclient ${kioclient_SRCS})
target_compile_definitions(kioclient PRIVATE -DKIOCLIENT_AS_KIOCLIENT)
target_link_libraries(kioclient ${KDE4_KIO_LIBS})
install(TARGETS kioclient ${INSTALL_TARGETS_DEFAULT_ARGS})

################

set(kde-open_SRCS kioclient.cpp )
add_executable(kde-open ${kde-open_SRCS})
target_compile_definitions(kde-open PRIVATE -DKIOCLIENT_AS_KDEOPEN)
target_link_libraries(kde-open ${KDE4_KIO_LIBS})
install(TARGETS kde-open ${INSTALL_TARGETS_DEFAULT_ARGS})

################

set(kde-cp_SRCS kioclient.cpp )
add_executable(kde-cp ${kde-cp_SRCS})
target_compile_definitions(kde-cp PRIVATE -DKIOCLIENT_AS_KDECP)
target_link_libraries(kde-cp ${KDE4_KIO_LIBS})
install(TARGETS kde-cp ${INSTALL_TARGETS_DEFAULT_ARGS})

################

set(kde-mv_SRCS kioclient.cpp )
add_executable(kde-mv ${kde-mv_SRCS})
target_compile_definitions(kde-mv PRIVATE -DKIOCLIENT_AS_KDEMV)
target_link_libraries(kde-mv ${KDE4_KIO_LIBS})
install(TARGETS kde-mv ${INSTALL_TARGETS_DEFAULT_ARGS})

