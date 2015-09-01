add_executable(ktraderclient ktraderclient.cpp)

target_link_libraries(ktraderclient ${KDE4_KIO_LIBS})

install(TARGETS ktraderclient ${INSTALL_TARGETS_DEFAULT_ARGS} )
