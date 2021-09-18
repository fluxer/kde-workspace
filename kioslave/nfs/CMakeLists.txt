## Check for XDR types
include(CheckTypeSize)

cmake_reset_check_state()
set(CMAKE_EXTRA_INCLUDE_FILES "rpc/rpc.h")
check_type_size(xdr_u_int64_t HAVE_XDR_U_INT64_T)
check_type_size(xdr_uint64_t HAVE_XDR_UINT64_T)
check_type_size(xdr_u_hyper HAVE_XDR_U_HYPER)
check_type_size(xdr_u_longlong_t HAVE_XDR_U_LONGLONG_T)
cmake_reset_check_state()

if (NOT HAVE_XDR_U_INT64_T AND NOT HAVE_XDR_UINT64_T AND NOT HAVE_XDR_U_HYPER AND NOT HAVE_XDR_U_LONGLONG_T)
    message(FATAL_ERROR "Could not find 64-bit XDR data types!")
endif (NOT HAVE_XDR_U_INT64_T AND NOT HAVE_XDR_UINT64_T AND NOT HAVE_XDR_U_HYPER AND NOT HAVE_XDR_U_LONGLONG_T)

if (HAVE_XDR_U_INT64_T)
    add_definitions(-DHAVE_XDR_U_INT64_T)
endif (HAVE_XDR_U_INT64_T)
if (HAVE_XDR_UINT64_T)
    add_definitions(-DHAVE_XDR_UINT64_T)
endif (HAVE_XDR_UINT64_T)
if (HAVE_XDR_U_HYPER)
    add_definitions(-DHAVE_XDR_U_HYPER)
endif (HAVE_XDR_U_HYPER)
if (HAVE_XDR_U_LONGLONG_T)
    add_definitions(-DHAVE_XDR_U_LONGLONG_T)
endif (HAVE_XDR_U_LONGLONG_T)

 add_definitions(-DTRANSLATION_DOMAIN=\"kio_nfs\")

set(kio_nfs_PART_SRCS kio_nfs.cpp nfsv2.cpp nfsv3.cpp rpc_nfs3_prot_xdr.c rpc_nfs2_prot_xdr.c )


kde4_add_plugin(kio_nfs ${kio_nfs_PART_SRCS})


target_link_libraries(kio_nfs   ${KDE4_KIO_LIBS})

install(TARGETS kio_nfs  DESTINATION ${KDE4_PLUGIN_INSTALL_DIR} )


########### install files ###############

install( FILES nfs.protocol  DESTINATION  ${KDE4_SERVICES_INSTALL_DIR} )
