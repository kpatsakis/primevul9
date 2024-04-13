static void encode_getattr_two(struct xdr_stream *xdr, uint32_t bm0, uint32_t bm1, struct compound_hdr *hdr)
{
	__be32 *p;

	encode_op_hdr(xdr, OP_GETATTR, decode_getattr_maxsz, hdr);
	p = reserve_space(xdr, 12);
	*p++ = cpu_to_be32(2);
	*p++ = cpu_to_be32(bm0);
	*p = cpu_to_be32(bm1);
}