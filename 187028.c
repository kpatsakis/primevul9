static int decode_create(struct xdr_stream *xdr, struct nfs4_change_info *cinfo)
{
	__be32 *p;
	uint32_t bmlen;
	int status;

	status = decode_op_hdr(xdr, OP_CREATE);
	if (status)
		return status;
	if ((status = decode_change_info(xdr, cinfo)))
		return status;
	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		goto out_overflow;
	bmlen = be32_to_cpup(p);
	p = xdr_inline_decode(xdr, bmlen << 2);
	if (likely(p))
		return 0;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}