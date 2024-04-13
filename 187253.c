static int decode_attr_length(struct xdr_stream *xdr, uint32_t *attrlen, unsigned int *savep)
{
	__be32 *p;

	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		goto out_overflow;
	*attrlen = be32_to_cpup(p);
	*savep = xdr_stream_pos(xdr);
	return 0;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}