static int decode_attr_error(struct xdr_stream *xdr, uint32_t *bitmap, int32_t *res)
{
	__be32 *p;

	if (unlikely(bitmap[0] & (FATTR4_WORD0_RDATTR_ERROR - 1U)))
		return -EIO;
	if (likely(bitmap[0] & FATTR4_WORD0_RDATTR_ERROR)) {
		p = xdr_inline_decode(xdr, 4);
		if (unlikely(!p))
			goto out_overflow;
		bitmap[0] &= ~FATTR4_WORD0_RDATTR_ERROR;
		*res = -be32_to_cpup(p);
	}
	return 0;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}