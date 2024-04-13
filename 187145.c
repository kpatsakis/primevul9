static int decode_access(struct xdr_stream *xdr, struct nfs4_accessres *access)
{
	__be32 *p;
	uint32_t supp, acc;
	int status;

	status = decode_op_hdr(xdr, OP_ACCESS);
	if (status)
		return status;
	p = xdr_inline_decode(xdr, 8);
	if (unlikely(!p))
		goto out_overflow;
	supp = be32_to_cpup(p++);
	acc = be32_to_cpup(p);
	access->supported = supp;
	access->access = acc;
	return 0;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}