static void encode_uint32(struct xdr_stream *xdr, u32 n)
{
	__be32 *p;

	p = reserve_space(xdr, 4);
	*p = cpu_to_be32(n);
}