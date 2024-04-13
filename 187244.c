encode_getattr_three(struct xdr_stream *xdr,
		     uint32_t bm0, uint32_t bm1, uint32_t bm2,
		     struct compound_hdr *hdr)
{
	__be32 *p;

	encode_op_hdr(xdr, OP_GETATTR, decode_getattr_maxsz, hdr);
	if (bm2) {
		p = reserve_space(xdr, 16);
		*p++ = cpu_to_be32(3);
		*p++ = cpu_to_be32(bm0);
		*p++ = cpu_to_be32(bm1);
		*p = cpu_to_be32(bm2);
	} else if (bm1) {
		p = reserve_space(xdr, 12);
		*p++ = cpu_to_be32(2);
		*p++ = cpu_to_be32(bm0);
		*p = cpu_to_be32(bm1);
	} else {
		p = reserve_space(xdr, 8);
		*p++ = cpu_to_be32(1);
		*p = cpu_to_be32(bm0);
	}
}