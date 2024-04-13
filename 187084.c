static int decode_getfattr_attrs(struct xdr_stream *xdr, uint32_t *bitmap,
		struct nfs_fattr *fattr, struct nfs_fh *fh,
		struct nfs4_fs_locations *fs_loc,
		const struct nfs_server *server)
{
	int status;
	umode_t fmode = 0;
	uint32_t type;
	int32_t err;

	status = decode_attr_type(xdr, bitmap, &type);
	if (status < 0)
		goto xdr_error;
	fattr->mode = 0;
	if (status != 0) {
		fattr->mode |= nfs_type2fmt[type];
		fattr->valid |= status;
	}

	status = decode_attr_change(xdr, bitmap, &fattr->change_attr);
	if (status < 0)
		goto xdr_error;
	fattr->valid |= status;

	status = decode_attr_size(xdr, bitmap, &fattr->size);
	if (status < 0)
		goto xdr_error;
	fattr->valid |= status;

	status = decode_attr_fsid(xdr, bitmap, &fattr->fsid);
	if (status < 0)
		goto xdr_error;
	fattr->valid |= status;

	err = 0;
	status = decode_attr_error(xdr, bitmap, &err);
	if (status < 0)
		goto xdr_error;

	status = decode_attr_filehandle(xdr, bitmap, fh);
	if (status < 0)
		goto xdr_error;

	status = decode_attr_fileid(xdr, bitmap, &fattr->fileid);
	if (status < 0)
		goto xdr_error;
	fattr->valid |= status;

	status = decode_attr_fs_locations(xdr, bitmap, fs_loc);
	if (status < 0)
		goto xdr_error;
	fattr->valid |= status;

	status = decode_attr_mode(xdr, bitmap, &fmode);
	if (status < 0)
		goto xdr_error;
	if (status != 0) {
		fattr->mode |= fmode;
		fattr->valid |= status;
	}

	status = decode_attr_nlink(xdr, bitmap, &fattr->nlink);
	if (status < 0)
		goto xdr_error;
	fattr->valid |= status;

	status = decode_attr_owner(xdr, bitmap, server, &fattr->uid, fattr->owner_name);
	if (status < 0)
		goto xdr_error;
	fattr->valid |= status;

	status = decode_attr_group(xdr, bitmap, server, &fattr->gid, fattr->group_name);
	if (status < 0)
		goto xdr_error;
	fattr->valid |= status;

	status = decode_attr_rdev(xdr, bitmap, &fattr->rdev);
	if (status < 0)
		goto xdr_error;
	fattr->valid |= status;

	status = decode_attr_space_used(xdr, bitmap, &fattr->du.nfs3.used);
	if (status < 0)
		goto xdr_error;
	fattr->valid |= status;

	status = decode_attr_time_access(xdr, bitmap, &fattr->atime);
	if (status < 0)
		goto xdr_error;
	fattr->valid |= status;

	status = decode_attr_time_metadata(xdr, bitmap, &fattr->ctime);
	if (status < 0)
		goto xdr_error;
	fattr->valid |= status;

	status = decode_attr_time_modify(xdr, bitmap, &fattr->mtime);
	if (status < 0)
		goto xdr_error;
	fattr->valid |= status;

	status = decode_attr_mounted_on_fileid(xdr, bitmap, &fattr->mounted_on_fileid);
	if (status < 0)
		goto xdr_error;
	fattr->valid |= status;

	status = decode_attr_mdsthreshold(xdr, bitmap, fattr->mdsthreshold);
	if (status < 0)
		goto xdr_error;

xdr_error:
	dprintk("%s: xdr returned %d\n", __func__, -status);
	return status;
}