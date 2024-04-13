static int decode_server_caps(struct xdr_stream *xdr, struct nfs4_server_caps_res *res)
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
	if ((status = decode_attr_supported(xdr, bitmap, res->attr_bitmask)) != 0)
		goto xdr_error;
	if ((status = decode_attr_fh_expire_type(xdr, bitmap,
						 &res->fh_expire_type)) != 0)
		goto xdr_error;
	if ((status = decode_attr_link_support(xdr, bitmap, &res->has_links)) != 0)
		goto xdr_error;
	if ((status = decode_attr_symlink_support(xdr, bitmap, &res->has_symlinks)) != 0)
		goto xdr_error;
	if ((status = decode_attr_aclsupport(xdr, bitmap, &res->acl_bitmask)) != 0)
		goto xdr_error;
	status = verify_attr_len(xdr, savep, attrlen);
xdr_error:
	dprintk("%s: xdr returned %d!\n", __func__, -status);
	return status;
}