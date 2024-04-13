static int decode_test_stateid(struct xdr_stream *xdr,
			       struct nfs41_test_stateid_res *res)
{
	__be32 *p;
	int status;
	int num_res;

	status = decode_op_hdr(xdr, OP_TEST_STATEID);
	if (status)
		return status;

	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		goto out_overflow;
	num_res = be32_to_cpup(p++);
	if (num_res != 1)
		goto out;

	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		goto out_overflow;
	res->status = be32_to_cpup(p++);

	return status;
out_overflow:
	print_overflow_msg(__func__, xdr);
out:
	return -EIO;
}