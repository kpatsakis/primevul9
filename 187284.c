static int decode_opaque_inline(struct xdr_stream *xdr, unsigned int *len, char **string)
{
	__be32 *p;

	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		goto out_overflow;
	*len = be32_to_cpup(p);
	p = xdr_inline_decode(xdr, *len);
	if (unlikely(!p))
		goto out_overflow;
	*string = (char *)p;
	return 0;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}