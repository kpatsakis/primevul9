static int decode_getfh(struct xdr_stream *xdr, struct nfs_fh *fh)
{
	__be32 *p;
	uint32_t len;
	int status;

	/* Zero handle first to allow comparisons */
	memset(fh, 0, sizeof(*fh));

	status = decode_op_hdr(xdr, OP_GETFH);
	if (status)
		return status;

	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		goto out_overflow;
	len = be32_to_cpup(p);
	if (len > NFS4_FHSIZE)
		return -EIO;
	fh->size = len;
	p = xdr_inline_decode(xdr, len);
	if (unlikely(!p))
		goto out_overflow;
	memcpy(fh->data, p, len);
	return 0;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}