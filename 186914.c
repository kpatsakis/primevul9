static int decode_ace(struct xdr_stream *xdr, void *ace, struct nfs_client *clp)
{
	__be32 *p;
	unsigned int strlen;
	char *str;

	p = xdr_inline_decode(xdr, 12);
	if (likely(p))
		return decode_opaque_inline(xdr, &strlen, &str);
	print_overflow_msg(__func__, xdr);
	return -EIO;
}