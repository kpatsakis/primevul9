static int decode_getfattr_generic(struct xdr_stream *xdr, struct nfs_fattr *fattr,
		struct nfs_fh *fh, struct nfs4_fs_locations *fs_loc,
		const struct nfs_server *server)
{
	unsigned int savep;
	uint32_t attrlen,
		 bitmap[3] = {0};
	int status;

	status = decode_op_hdr(xdr, OP_GETATTR);
	if (status < 0)
		goto xdr_error;

	status = decode_attr_bitmap(xdr, bitmap);
	if (status < 0)
		goto xdr_error;

	status = decode_attr_length(xdr, &attrlen, &savep);
	if (status < 0)
		goto xdr_error;

	status = decode_getfattr_attrs(xdr, bitmap, fattr, fh, fs_loc, server);
	if (status < 0)
		goto xdr_error;

	status = verify_attr_len(xdr, savep, attrlen);
xdr_error:
	dprintk("%s: xdr returned %d\n", __func__, -status);
	return status;
}