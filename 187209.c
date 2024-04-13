static int decode_free_stateid(struct xdr_stream *xdr,
			       struct nfs41_free_stateid_res *res)
{
	__be32 *p;
	int status;

	status = decode_op_hdr(xdr, OP_FREE_STATEID);
	if (status)
		return status;

	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		goto out_overflow;
	res->status = be32_to_cpup(p++);
	return res->status;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}