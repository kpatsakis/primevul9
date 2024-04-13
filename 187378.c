static void encode_uint64(struct xdr_stream *xdr, u64 n)
{
	__be32 *p;

	p = reserve_space(xdr, 8);
	xdr_encode_hyper(p, n);
}