
# we need a version of samba which has already smbc_set_context(), Alex
set(SAMBA_REQUIRE_SMBC_SET_CONTEXT TRUE)
set(SAMBA_REQUIRE_SMBC_OPTION_SET TRUE)
macro_optional_find_package(Samba)
set_package_properties(Samba PROPERTIES
    DESCRIPTION "the SMB client library, a version with smbc_set_context() and smbc_option_set()"
    URL "http://www.samba.org"
    TYPE OPTIONAL
    PURPOSE "Needed to build the SMB kioslave"
)

macro_optional_find_package(LibSSH 0.6.0)
set_package_properties(LibSSH PROPERTIES
    DESCRIPTION "the SSH library with SFTP support"
    URL "http://www.libssh.org/"
    TYPE OPTIONAL
    PURPOSE "Needed to build the SFTP kioslave"
)

macro_optional_find_package(Mtp 1.1.2)
set_package_properties(Mtp PROPERTIES
    DESCRIPTION "the library implementation of the Media Transfer Protocol"
    URL "http://libmtp.sourceforge.net/"
    TYPE OPTIONAL
    PURPOSE "Needed to build the MTP kioslave"
)

add_subdirectory( bookmarks )
add_subdirectory( filter )
add_subdirectory( archive )
add_subdirectory( settings )
add_subdirectory( applications )
add_subdirectory( trash )
add_subdirectory( network )
add_subdirectory( remote )
add_subdirectory( desktop )
add_subdirectory( recentdocuments )
add_subdirectory( thumbnail )
if (LIBSSH_FOUND)
  add_subdirectory(sftp)
endif()
if (MTP_FOUND)
  add_subdirectory(mtp)
endif()

add_subdirectory( floppy )
check_include_files(rpc/rpc.h HAVE_RPC_RPC_H)
add_feature_info("NFS kioslave" HAVE_RPC_RPC_H "The RPC library is needed to build the NFS kioslave")
if(HAVE_RPC_RPC_H)
  add_subdirectory(nfs)
endif()

if(SAMBA_FOUND)
    add_subdirectory(smb)
endif()

# add_subdirectory(audiocd)
