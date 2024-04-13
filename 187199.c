static int decode_opaque_fixed(struct xdr_stream *xdr, void *buf, size_t len)
{
	__be32 *p;

	p = xdr_inline_decode(xdr, len);
	if (likely(p)) {
		memcpy(buf, p, len);
		return 0;
	}
	print_overflow_msg(__func__, xdr);
	return -EIO;
}