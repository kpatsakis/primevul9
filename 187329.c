static int decode_statfs(struct xdr_stream *xdr, struct nfs_fsstat *fsstat)
{
	unsigned int savep;
	uint32_t attrlen, bitmap[3] = {0};
	int status;

	if ((status = decode_op_hdr(xdr, OP_GETATTR)) != 0)
		goto xdr_error;
	if ((status = decode_attr_bitmap(xdr, bitmap)) != 0)
		goto xdr_error;
	if ((status = decode_attr_length(xdr, &attrlen, &savep)) != 0)
		goto xdr_error;

	if ((status = decode_attr_files_avail(xdr, bitmap, &fsstat->afiles)) != 0)
		goto xdr_error;
	if ((status = decode_attr_files_free(xdr, bitmap, &fsstat->ffiles)) != 0)
		goto xdr_error;
	if ((status = decode_attr_files_total(xdr, bitmap, &fsstat->tfiles)) != 0)
		goto xdr_error;
	if ((status = decode_attr_space_avail(xdr, bitmap, &fsstat->abytes)) != 0)
		goto xdr_error;
	if ((status = decode_attr_space_free(xdr, bitmap, &fsstat->fbytes)) != 0)
		goto xdr_error;
	if ((status = decode_attr_space_total(xdr, bitmap, &fsstat->tbytes)) != 0)
		goto xdr_error;

	status = verify_attr_len(xdr, savep, attrlen);
xdr_error:
	dprintk("%s: xdr returned %d!\n", __func__, -status);
	return status;
}