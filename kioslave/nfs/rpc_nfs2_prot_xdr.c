/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "rpc_nfs2_prot.h"
/*
 * Sun RPC is a product of Sun Microsystems, Inc. and is provided for
 * unrestricted use provided that this legend is included on all tape
 * media and as a part of the software program in whole or part.  Users
 * may copy or modify Sun RPC without charge, but are not authorized
 * to license or distribute it to anyone else except as part of a product or
 * program developed by the user or with the express written consent of
 * Sun Microsystems, Inc.
 *
 * SUN RPC IS PROVIDED AS IS WITH NO WARRANTIES OF ANY KIND INCLUDING THE
 * WARRANTIES OF DESIGN, MERCHANTIBILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, OR ARISING FROM A COURSE OF DEALING, USAGE OR TRADE PRACTICE.
 *
 * Sun RPC is provided with no support and without any obligation on the
 * part of Sun Microsystems, Inc. to assist in its use, correction,
 * modification or enhancement.
 *
 * SUN MICROSYSTEMS, INC. SHALL HAVE NO LIABILITY WITH RESPECT TO THE
 * INFRINGEMENT OF COPYRIGHTS, TRADE SECRETS OR ANY PATENTS BY SUN RPC
 * OR ANY PART THEREOF.
 *
 * In no event will Sun Microsystems, Inc. be liable for any lost revenue
 * or profits or other special, indirect and consequential damages, even if
 * Sun has been advised of the possibility of such damages.
 *
 * Sun Microsystems, Inc.
 * 2550 Garcia Avenue
 * Mountain View, California  94043
 */
/*
 * Copyright (c) 1987, 1990 by Sun Microsystems, Inc.
 */

/* from @(#)nfs_prot.x	1.3 91/03/11 TIRPC 1.0 */

bool_t
xdr_nfsstat (XDR *xdrs, nfsstat *objp)
{
	register int32_t *buf;

	 if (!xdr_enum (xdrs, (enum_t *) objp))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_ftype (XDR *xdrs, ftype *objp)
{
	register int32_t *buf;

	 if (!xdr_enum (xdrs, (enum_t *) objp))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_nfs_fh (XDR *xdrs, nfs_fh *objp)
{
	register int32_t *buf;

	int i;
	 if (!xdr_opaque (xdrs, objp->data, NFS_FHSIZE))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_nfstime (XDR *xdrs, nfstime *objp)
{
	register int32_t *buf;

	 if (!xdr_u_int (xdrs, &objp->seconds))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->useconds))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_fattr (XDR *xdrs, fattr *objp)
{
	register int32_t *buf;


	if (xdrs->x_op == XDR_ENCODE) {
		 if (!xdr_ftype (xdrs, &objp->type))
			 return FALSE;
		buf = XDR_INLINE (xdrs, 10 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_u_int (xdrs, &objp->mode))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->nlink))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->uid))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->gid))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->size))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->blocksize))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->rdev))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->blocks))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->fsid))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->fileid))
				 return FALSE;

		} else {
		IXDR_PUT_U_LONG(buf, objp->mode);
		IXDR_PUT_U_LONG(buf, objp->nlink);
		IXDR_PUT_U_LONG(buf, objp->uid);
		IXDR_PUT_U_LONG(buf, objp->gid);
		IXDR_PUT_U_LONG(buf, objp->size);
		IXDR_PUT_U_LONG(buf, objp->blocksize);
		IXDR_PUT_U_LONG(buf, objp->rdev);
		IXDR_PUT_U_LONG(buf, objp->blocks);
		IXDR_PUT_U_LONG(buf, objp->fsid);
		IXDR_PUT_U_LONG(buf, objp->fileid);
		}
		 if (!xdr_nfstime (xdrs, &objp->atime))
			 return FALSE;
		 if (!xdr_nfstime (xdrs, &objp->mtime))
			 return FALSE;
		 if (!xdr_nfstime (xdrs, &objp->ctime))
			 return FALSE;
		return TRUE;
	} else if (xdrs->x_op == XDR_DECODE) {
		 if (!xdr_ftype (xdrs, &objp->type))
			 return FALSE;
		buf = XDR_INLINE (xdrs, 10 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_u_int (xdrs, &objp->mode))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->nlink))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->uid))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->gid))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->size))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->blocksize))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->rdev))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->blocks))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->fsid))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->fileid))
				 return FALSE;

		} else {
		objp->mode = IXDR_GET_U_LONG(buf);
		objp->nlink = IXDR_GET_U_LONG(buf);
		objp->uid = IXDR_GET_U_LONG(buf);
		objp->gid = IXDR_GET_U_LONG(buf);
		objp->size = IXDR_GET_U_LONG(buf);
		objp->blocksize = IXDR_GET_U_LONG(buf);
		objp->rdev = IXDR_GET_U_LONG(buf);
		objp->blocks = IXDR_GET_U_LONG(buf);
		objp->fsid = IXDR_GET_U_LONG(buf);
		objp->fileid = IXDR_GET_U_LONG(buf);
		}
		 if (!xdr_nfstime (xdrs, &objp->atime))
			 return FALSE;
		 if (!xdr_nfstime (xdrs, &objp->mtime))
			 return FALSE;
		 if (!xdr_nfstime (xdrs, &objp->ctime))
			 return FALSE;
	 return TRUE;
	}

	 if (!xdr_ftype (xdrs, &objp->type))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->mode))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->nlink))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->uid))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->gid))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->size))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->blocksize))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->rdev))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->blocks))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->fsid))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->fileid))
		 return FALSE;
	 if (!xdr_nfstime (xdrs, &objp->atime))
		 return FALSE;
	 if (!xdr_nfstime (xdrs, &objp->mtime))
		 return FALSE;
	 if (!xdr_nfstime (xdrs, &objp->ctime))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_sattr (XDR *xdrs, sattr *objp)
{
	register int32_t *buf;


	if (xdrs->x_op == XDR_ENCODE) {
		buf = XDR_INLINE (xdrs, 4 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_u_int (xdrs, &objp->mode))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->uid))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->gid))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->size))
				 return FALSE;

		} else {
		IXDR_PUT_U_LONG(buf, objp->mode);
		IXDR_PUT_U_LONG(buf, objp->uid);
		IXDR_PUT_U_LONG(buf, objp->gid);
		IXDR_PUT_U_LONG(buf, objp->size);
		}
		 if (!xdr_nfstime (xdrs, &objp->atime))
			 return FALSE;
		 if (!xdr_nfstime (xdrs, &objp->mtime))
			 return FALSE;
		return TRUE;
	} else if (xdrs->x_op == XDR_DECODE) {
		buf = XDR_INLINE (xdrs, 4 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_u_int (xdrs, &objp->mode))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->uid))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->gid))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->size))
				 return FALSE;

		} else {
		objp->mode = IXDR_GET_U_LONG(buf);
		objp->uid = IXDR_GET_U_LONG(buf);
		objp->gid = IXDR_GET_U_LONG(buf);
		objp->size = IXDR_GET_U_LONG(buf);
		}
		 if (!xdr_nfstime (xdrs, &objp->atime))
			 return FALSE;
		 if (!xdr_nfstime (xdrs, &objp->mtime))
			 return FALSE;
	 return TRUE;
	}

	 if (!xdr_u_int (xdrs, &objp->mode))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->uid))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->gid))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->size))
		 return FALSE;
	 if (!xdr_nfstime (xdrs, &objp->atime))
		 return FALSE;
	 if (!xdr_nfstime (xdrs, &objp->mtime))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_filename (XDR *xdrs, filename *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, objp, NFS_MAXNAMLEN))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_nfspath (XDR *xdrs, nfspath *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, objp, NFS_MAXPATHLEN))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_attrstat (XDR *xdrs, attrstat *objp)
{
	register int32_t *buf;

	 if (!xdr_nfsstat (xdrs, &objp->status))
		 return FALSE;
	switch (objp->status) {
	case NFS_OK:
		 if (!xdr_fattr (xdrs, &objp->attrstat_u.attributes))
			 return FALSE;
		break;
	default:
		break;
	}
	return TRUE;
}

bool_t
xdr_sattrargs (XDR *xdrs, sattrargs *objp)
{
	register int32_t *buf;

	 if (!xdr_nfs_fh (xdrs, &objp->file))
		 return FALSE;
	 if (!xdr_sattr (xdrs, &objp->attributes))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_diropargs (XDR *xdrs, diropargs *objp)
{
	register int32_t *buf;

	 if (!xdr_nfs_fh (xdrs, &objp->dir))
		 return FALSE;
	 if (!xdr_filename (xdrs, &objp->name))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_diropokres (XDR *xdrs, diropokres *objp)
{
	register int32_t *buf;

	 if (!xdr_nfs_fh (xdrs, &objp->file))
		 return FALSE;
	 if (!xdr_fattr (xdrs, &objp->attributes))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_diropres (XDR *xdrs, diropres *objp)
{
	register int32_t *buf;

	 if (!xdr_nfsstat (xdrs, &objp->status))
		 return FALSE;
	switch (objp->status) {
	case NFS_OK:
		 if (!xdr_diropokres (xdrs, &objp->diropres_u.diropres))
			 return FALSE;
		break;
	default:
		break;
	}
	return TRUE;
}

bool_t
xdr_readlinkres (XDR *xdrs, readlinkres *objp)
{
	register int32_t *buf;

	 if (!xdr_nfsstat (xdrs, &objp->status))
		 return FALSE;
	switch (objp->status) {
	case NFS_OK:
		 if (!xdr_nfspath (xdrs, &objp->readlinkres_u.data))
			 return FALSE;
		break;
	default:
		break;
	}
	return TRUE;
}

bool_t
xdr_readargs (XDR *xdrs, readargs *objp)
{
	register int32_t *buf;

	 if (!xdr_nfs_fh (xdrs, &objp->file))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->offset))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->count))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->totalcount))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_readokres (XDR *xdrs, readokres *objp)
{
	register int32_t *buf;

	 if (!xdr_fattr (xdrs, &objp->attributes))
		 return FALSE;
	 if (!xdr_bytes (xdrs, (char **)&objp->data.data_val, (u_int *) &objp->data.data_len, NFS_MAXDATA))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_readres (XDR *xdrs, readres *objp)
{
	register int32_t *buf;

	 if (!xdr_nfsstat (xdrs, &objp->status))
		 return FALSE;
	switch (objp->status) {
	case NFS_OK:
		 if (!xdr_readokres (xdrs, &objp->readres_u.reply))
			 return FALSE;
		break;
	default:
		break;
	}
	return TRUE;
}

bool_t
xdr_writeargs (XDR *xdrs, writeargs *objp)
{
	register int32_t *buf;


	if (xdrs->x_op == XDR_ENCODE) {
		 if (!xdr_nfs_fh (xdrs, &objp->file))
			 return FALSE;
		buf = XDR_INLINE (xdrs, 3 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_u_int (xdrs, &objp->beginoffset))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->offset))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->totalcount))
				 return FALSE;

		} else {
		IXDR_PUT_U_LONG(buf, objp->beginoffset);
		IXDR_PUT_U_LONG(buf, objp->offset);
		IXDR_PUT_U_LONG(buf, objp->totalcount);
		}
		 if (!xdr_bytes (xdrs, (char **)&objp->data.data_val, (u_int *) &objp->data.data_len, NFS_MAXDATA))
			 return FALSE;
		return TRUE;
	} else if (xdrs->x_op == XDR_DECODE) {
		 if (!xdr_nfs_fh (xdrs, &objp->file))
			 return FALSE;
		buf = XDR_INLINE (xdrs, 3 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_u_int (xdrs, &objp->beginoffset))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->offset))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->totalcount))
				 return FALSE;

		} else {
		objp->beginoffset = IXDR_GET_U_LONG(buf);
		objp->offset = IXDR_GET_U_LONG(buf);
		objp->totalcount = IXDR_GET_U_LONG(buf);
		}
		 if (!xdr_bytes (xdrs, (char **)&objp->data.data_val, (u_int *) &objp->data.data_len, NFS_MAXDATA))
			 return FALSE;
	 return TRUE;
	}

	 if (!xdr_nfs_fh (xdrs, &objp->file))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->beginoffset))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->offset))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->totalcount))
		 return FALSE;
	 if (!xdr_bytes (xdrs, (char **)&objp->data.data_val, (u_int *) &objp->data.data_len, NFS_MAXDATA))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_createargs (XDR *xdrs, createargs *objp)
{
	register int32_t *buf;

	 if (!xdr_diropargs (xdrs, &objp->where))
		 return FALSE;
	 if (!xdr_sattr (xdrs, &objp->attributes))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_renameargs (XDR *xdrs, renameargs *objp)
{
	register int32_t *buf;

	 if (!xdr_diropargs (xdrs, &objp->from))
		 return FALSE;
	 if (!xdr_diropargs (xdrs, &objp->to))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_linkargs (XDR *xdrs, linkargs *objp)
{
	register int32_t *buf;

	 if (!xdr_nfs_fh (xdrs, &objp->from))
		 return FALSE;
	 if (!xdr_diropargs (xdrs, &objp->to))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_symlinkargs (XDR *xdrs, symlinkargs *objp)
{
	register int32_t *buf;

	 if (!xdr_diropargs (xdrs, &objp->from))
		 return FALSE;
	 if (!xdr_nfspath (xdrs, &objp->to))
		 return FALSE;
	 if (!xdr_sattr (xdrs, &objp->attributes))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_nfscookie (XDR *xdrs, nfscookie objp)
{
	register int32_t *buf;

	 if (!xdr_opaque (xdrs, objp, NFS_COOKIESIZE))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_readdirargs (XDR *xdrs, readdirargs *objp)
{
	register int32_t *buf;

	 if (!xdr_nfs_fh (xdrs, &objp->dir))
		 return FALSE;
	 if (!xdr_nfscookie (xdrs, objp->cookie))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->count))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_entry (XDR *xdrs, entry *objp)
{
	register int32_t *buf;

	 if (!xdr_u_int (xdrs, &objp->fileid))
		 return FALSE;
	 if (!xdr_filename (xdrs, &objp->name))
		 return FALSE;
	 if (!xdr_nfscookie (xdrs, objp->cookie))
		 return FALSE;
	 if (!xdr_pointer (xdrs, (char **)&objp->nextentry, sizeof (entry), (xdrproc_t) xdr_entry))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_dirlist (XDR *xdrs, dirlist *objp)
{
	register int32_t *buf;

	 if (!xdr_pointer (xdrs, (char **)&objp->entries, sizeof (entry), (xdrproc_t) xdr_entry))
		 return FALSE;
	 if (!xdr_bool (xdrs, &objp->eof))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_readdirres (XDR *xdrs, readdirres *objp)
{
	register int32_t *buf;

	 if (!xdr_nfsstat (xdrs, &objp->status))
		 return FALSE;
	switch (objp->status) {
	case NFS_OK:
		 if (!xdr_dirlist (xdrs, &objp->readdirres_u.reply))
			 return FALSE;
		break;
	default:
		break;
	}
	return TRUE;
}

bool_t
xdr_statfsokres (XDR *xdrs, statfsokres *objp)
{
	register int32_t *buf;


	if (xdrs->x_op == XDR_ENCODE) {
		buf = XDR_INLINE (xdrs, 5 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_u_int (xdrs, &objp->tsize))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->bsize))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->blocks))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->bfree))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->bavail))
				 return FALSE;
		} else {
			IXDR_PUT_U_LONG(buf, objp->tsize);
			IXDR_PUT_U_LONG(buf, objp->bsize);
			IXDR_PUT_U_LONG(buf, objp->blocks);
			IXDR_PUT_U_LONG(buf, objp->bfree);
			IXDR_PUT_U_LONG(buf, objp->bavail);
		}
		return TRUE;
	} else if (xdrs->x_op == XDR_DECODE) {
		buf = XDR_INLINE (xdrs, 5 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_u_int (xdrs, &objp->tsize))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->bsize))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->blocks))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->bfree))
				 return FALSE;
			 if (!xdr_u_int (xdrs, &objp->bavail))
				 return FALSE;
		} else {
			objp->tsize = IXDR_GET_U_LONG(buf);
			objp->bsize = IXDR_GET_U_LONG(buf);
			objp->blocks = IXDR_GET_U_LONG(buf);
			objp->bfree = IXDR_GET_U_LONG(buf);
			objp->bavail = IXDR_GET_U_LONG(buf);
		}
	 return TRUE;
	}

	 if (!xdr_u_int (xdrs, &objp->tsize))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->bsize))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->blocks))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->bfree))
		 return FALSE;
	 if (!xdr_u_int (xdrs, &objp->bavail))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_statfsres (XDR *xdrs, statfsres *objp)
{
	register int32_t *buf;

	 if (!xdr_nfsstat (xdrs, &objp->status))
		 return FALSE;
	switch (objp->status) {
	case NFS_OK:
		 if (!xdr_statfsokres (xdrs, &objp->statfsres_u.reply))
			 return FALSE;
		break;
	default:
		break;
	}
	return TRUE;
}

bool_t
xdr_fhandle (XDR *xdrs, fhandle objp)
{
	register int32_t *buf;

	 if (!xdr_opaque (xdrs, objp, FHSIZE))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_fhstatus (XDR *xdrs, fhstatus *objp)
{
	register int32_t *buf;

	 if (!xdr_u_int (xdrs, &objp->fhs_status))
		 return FALSE;
	switch (objp->fhs_status) {
	case 0:
		 if (!xdr_fhandle (xdrs, objp->fhstatus_u.fhs_fhandle))
			 return FALSE;
		break;
	default:
		break;
	}
	return TRUE;
}

bool_t
xdr_dirpath (XDR *xdrs, dirpath *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, objp, MNTPATHLEN))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_name (XDR *xdrs, name *objp)
{
	register int32_t *buf;

	 if (!xdr_string (xdrs, objp, MNTNAMLEN))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_mountlist (XDR *xdrs, mountlist *objp)
{
	register int32_t *buf;

	 if (!xdr_pointer (xdrs, (char **)objp, sizeof (struct mountbody), (xdrproc_t) xdr_mountbody))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_mountbody (XDR *xdrs, mountbody *objp)
{
	register int32_t *buf;

	 if (!xdr_name (xdrs, &objp->ml_hostname))
		 return FALSE;
	 if (!xdr_dirpath (xdrs, &objp->ml_directory))
		 return FALSE;
	 if (!xdr_mountlist (xdrs, &objp->ml_next))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_groups (XDR *xdrs, groups *objp)
{
	register int32_t *buf;

	 if (!xdr_pointer (xdrs, (char **)objp, sizeof (struct groupnode), (xdrproc_t) xdr_groupnode))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_groupnode (XDR *xdrs, groupnode *objp)
{
	register int32_t *buf;

	 if (!xdr_name (xdrs, &objp->gr_name))
		 return FALSE;
	 if (!xdr_groups (xdrs, &objp->gr_next))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_exports (XDR *xdrs, exports *objp)
{
	register int32_t *buf;

	 if (!xdr_pointer (xdrs, (char **)objp, sizeof (struct exportnode), (xdrproc_t) xdr_exportnode))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_exportnode (XDR *xdrs, exportnode *objp)
{
	register int32_t *buf;

	 if (!xdr_dirpath (xdrs, &objp->ex_dir))
		 return FALSE;
	 if (!xdr_groups (xdrs, &objp->ex_groups))
		 return FALSE;
	 if (!xdr_exports (xdrs, &objp->ex_next))
		 return FALSE;
	return TRUE;
}

bool_t
xdr_ppathcnf (XDR *xdrs, ppathcnf *objp)
{
	register int32_t *buf;

	int i;

	if (xdrs->x_op == XDR_ENCODE) {
		buf = XDR_INLINE (xdrs, 6 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_int (xdrs, &objp->pc_link_max))
				 return FALSE;
			 if (!xdr_short (xdrs, &objp->pc_max_canon))
				 return FALSE;
			 if (!xdr_short (xdrs, &objp->pc_max_input))
				 return FALSE;
			 if (!xdr_short (xdrs, &objp->pc_name_max))
				 return FALSE;
			 if (!xdr_short (xdrs, &objp->pc_path_max))
				 return FALSE;
			 if (!xdr_short (xdrs, &objp->pc_pipe_buf))
				 return FALSE;

		} else {
		IXDR_PUT_LONG(buf, objp->pc_link_max);
		IXDR_PUT_SHORT(buf, objp->pc_max_canon);
		IXDR_PUT_SHORT(buf, objp->pc_max_input);
		IXDR_PUT_SHORT(buf, objp->pc_name_max);
		IXDR_PUT_SHORT(buf, objp->pc_path_max);
		IXDR_PUT_SHORT(buf, objp->pc_pipe_buf);
		}
		 if (!xdr_u_char (xdrs, &objp->pc_vdisable))
			 return FALSE;
		 if (!xdr_char (xdrs, &objp->pc_xxx))
			 return FALSE;
		buf = XDR_INLINE (xdrs, ( 2 ) * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_vector (xdrs, (char *)objp->pc_mask, 2,
				sizeof (short), (xdrproc_t) xdr_short))
				 return FALSE;
		} else {
			{
				register short *genp;

				for (i = 0, genp = objp->pc_mask;
					i < 2; ++i) {
					IXDR_PUT_SHORT(buf, *genp++);
				}
			}
		}
		return TRUE;
	} else if (xdrs->x_op == XDR_DECODE) {
		buf = XDR_INLINE (xdrs, 6 * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_int (xdrs, &objp->pc_link_max))
				 return FALSE;
			 if (!xdr_short (xdrs, &objp->pc_max_canon))
				 return FALSE;
			 if (!xdr_short (xdrs, &objp->pc_max_input))
				 return FALSE;
			 if (!xdr_short (xdrs, &objp->pc_name_max))
				 return FALSE;
			 if (!xdr_short (xdrs, &objp->pc_path_max))
				 return FALSE;
			 if (!xdr_short (xdrs, &objp->pc_pipe_buf))
				 return FALSE;

		} else {
		objp->pc_link_max = IXDR_GET_LONG(buf);
		objp->pc_max_canon = IXDR_GET_SHORT(buf);
		objp->pc_max_input = IXDR_GET_SHORT(buf);
		objp->pc_name_max = IXDR_GET_SHORT(buf);
		objp->pc_path_max = IXDR_GET_SHORT(buf);
		objp->pc_pipe_buf = IXDR_GET_SHORT(buf);
		}
		 if (!xdr_u_char (xdrs, &objp->pc_vdisable))
			 return FALSE;
		 if (!xdr_char (xdrs, &objp->pc_xxx))
			 return FALSE;
		buf = XDR_INLINE (xdrs, ( 2 ) * BYTES_PER_XDR_UNIT);
		if (buf == NULL) {
			 if (!xdr_vector (xdrs, (char *)objp->pc_mask, 2,
				sizeof (short), (xdrproc_t) xdr_short))
				 return FALSE;
		} else {
			{
				register short *genp;

				for (i = 0, genp = objp->pc_mask;
					i < 2; ++i) {
					*genp++ = IXDR_GET_SHORT(buf);
				}
			}
		}
	 return TRUE;
	}

	 if (!xdr_int (xdrs, &objp->pc_link_max))
		 return FALSE;
	 if (!xdr_short (xdrs, &objp->pc_max_canon))
		 return FALSE;
	 if (!xdr_short (xdrs, &objp->pc_max_input))
		 return FALSE;
	 if (!xdr_short (xdrs, &objp->pc_name_max))
		 return FALSE;
	 if (!xdr_short (xdrs, &objp->pc_path_max))
		 return FALSE;
	 if (!xdr_short (xdrs, &objp->pc_pipe_buf))
		 return FALSE;
	 if (!xdr_u_char (xdrs, &objp->pc_vdisable))
		 return FALSE;
	 if (!xdr_char (xdrs, &objp->pc_xxx))
		 return FALSE;
	 if (!xdr_vector (xdrs, (char *)objp->pc_mask, 2,
		sizeof (short), (xdrproc_t) xdr_short))
		 return FALSE;
	return TRUE;
}
