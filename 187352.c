static int decode_layoutreturn(struct xdr_stream *xdr,
			       struct nfs4_layoutreturn_res *res)
{
	__be32 *p;
	int status;

	status = decode_op_hdr(xdr, OP_LAYOUTRETURN);
	if (status)
		return status;
	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		goto out_overflow;
	res->lrs_present = be32_to_cpup(p);
	if (res->lrs_present)
		status = decode_stateid(xdr, &res->stateid);
	return status;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}