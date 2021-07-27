########### Configure checks for kwalletbackend ###############

check_include_files(stdint.h HAVE_STDINT_H)
check_include_files(sys/bitypes.h HAVE_SYS_BITYPES_H)

configure_file(
    config-kwalletbackend.h.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/config-kwalletbackend.h
)

########### kwalletbackend ###############

set(kwalletbackend_LIB_SRCS
   blockcipher.cc
   blowfish.cc
   cbc.cc
   kwalletentry.cc
   kwalletbackend.cc
   backendpersisthandler.cpp
)

add_library(kwalletbackend SHARED ${kwalletbackend_LIB_SRCS})

target_link_libraries(kwalletbackend ${KDE4_KDEUI_LIBS})

set_target_properties(kwalletbackend PROPERTIES
    VERSION ${GENERIC_LIB_VERSION}
    SOVERSION ${GENERIC_LIB_SOVERSION}
)

generate_export_header(kwalletbackend)

install(TARGETS kwalletbackend ${INSTALL_TARGETS_DEFAULT_ARGS})

if(ENABLE_TESTING)
    add_subdirectory(tests)
endif()
