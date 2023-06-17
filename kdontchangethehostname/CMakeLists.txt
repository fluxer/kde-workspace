set(kdontchangethehostname_SRCS khostname.cpp )

add_executable(kdontchangethehostname ${kdontchangethehostname_SRCS})
target_link_libraries(kdontchangethehostname ${KDE4_KDECORE_LIBS})

install(
    TARGETS kdontchangethehostname
    DESTINATION ${KDE4_LIBEXEC_INSTALL_DIR}
)

