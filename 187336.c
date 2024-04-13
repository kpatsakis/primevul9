static int decode_fsinfo(struct xdr_stream *xdr, struct nfs_fsinfo *fsinfo)
{
	unsigned int savep;
	uint32_t attrlen, bitmap[3];
	int status;

	if ((status = decode_op_hdr(xdr, OP_GETATTR)) != 0)
		goto xdr_error;
	if ((status = decode_attr_bitmap(xdr, bitmap)) != 0)
		goto xdr_error;
	if ((status = decode_attr_length(xdr, &attrlen, &savep)) != 0)
		goto xdr_error;

	fsinfo->rtmult = fsinfo->wtmult = 512;	/* ??? */

	if ((status = decode_attr_lease_time(xdr, bitmap, &fsinfo->lease_time)) != 0)
		goto xdr_error;
	if ((status = decode_attr_maxfilesize(xdr, bitmap, &fsinfo->maxfilesize)) != 0)
		goto xdr_error;
	if ((status = decode_attr_maxread(xdr, bitmap, &fsinfo->rtmax)) != 0)
		goto xdr_error;
	fsinfo->rtpref = fsinfo->dtpref = fsinfo->rtmax;
	if ((status = decode_attr_maxwrite(xdr, bitmap, &fsinfo->wtmax)) != 0)
		goto xdr_error;
	fsinfo->wtpref = fsinfo->wtmax;
	status = decode_attr_time_delta(xdr, bitmap, &fsinfo->time_delta);
	if (status != 0)
		goto xdr_error;
	status = decode_attr_pnfstype(xdr, bitmap, &fsinfo->layouttype);
	if (status != 0)
		goto xdr_error;
	status = decode_attr_layout_blksize(xdr, bitmap, &fsinfo->blksize);
	if (status)
		goto xdr_error;

	status = verify_attr_len(xdr, savep, attrlen);
xdr_error:
	dprintk("%s: xdr returned %d!\n", __func__, -status);
	return status;
}