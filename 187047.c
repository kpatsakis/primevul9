static void encode_opaque_fixed(struct xdr_stream *xdr, const void *buf, size_t len)
{
	__be32 *p;

	p = xdr_reserve_space(xdr, len);
	xdr_encode_opaque_fixed(p, buf, len);
}