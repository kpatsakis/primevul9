static void encode_getattr_one(struct xdr_stream *xdr, uint32_t bitmap, struct compound_hdr *hdr)
{
	__be32 *p;

	encode_op_hdr(xdr, OP_GETATTR, decode_getattr_maxsz, hdr);
	p = reserve_space(xdr, 8);
	*p++ = cpu_to_be32(1);
	*p = cpu_to_be32(bitmap);
}