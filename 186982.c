static int decode_attr_bitmap(struct xdr_stream *xdr, uint32_t *bitmap)
{
	uint32_t bmlen;
	__be32 *p;

	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		goto out_overflow;
	bmlen = be32_to_cpup(p);

	bitmap[0] = bitmap[1] = bitmap[2] = 0;
	p = xdr_inline_decode(xdr, (bmlen << 2));
	if (unlikely(!p))
		goto out_overflow;
	if (bmlen > 0) {
		bitmap[0] = be32_to_cpup(p++);
		if (bmlen > 1) {
			bitmap[1] = be32_to_cpup(p++);
			if (bmlen > 2)
				bitmap[2] = be32_to_cpup(p);
		}
	}
	return 0;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}