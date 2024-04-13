static int decode_attr_maxwrite(struct xdr_stream *xdr, uint32_t *bitmap, uint32_t *res)
{
	__be32 *p;
	int status = 0;

	*res = 1024;
	if (unlikely(bitmap[0] & (FATTR4_WORD0_MAXWRITE - 1U)))
		return -EIO;
	if (likely(bitmap[0] & FATTR4_WORD0_MAXWRITE)) {
		uint64_t maxwrite;
		p = xdr_inline_decode(xdr, 8);
		if (unlikely(!p))
			goto out_overflow;
		xdr_decode_hyper(p, &maxwrite);
		if (maxwrite > 0x7FFFFFFF)
			maxwrite = 0x7FFFFFFF;
		*res = (uint32_t)maxwrite;
		bitmap[0] &= ~FATTR4_WORD0_MAXWRITE;
	}
	dprintk("%s: maxwrite=%lu\n", __func__, (unsigned long)*res);
	return status;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}