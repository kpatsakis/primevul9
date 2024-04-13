static int decode_threshold_hint(struct xdr_stream *xdr,
				  uint32_t *bitmap,
				  uint64_t *res,
				  uint32_t hint_bit)
{
	__be32 *p;

	*res = 0;
	if (likely(bitmap[0] & hint_bit)) {
		p = xdr_inline_decode(xdr, 8);
		if (unlikely(!p))
			goto out_overflow;
		xdr_decode_hyper(p, res);
	}
	return 0;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}