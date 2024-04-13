static void encode_string(struct xdr_stream *xdr, unsigned int len, const char *str)
{
	__be32 *p;

	p = reserve_space(xdr, 4 + len);
	xdr_encode_opaque(p, str, len);
}