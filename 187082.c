static int decode_attr_maxread(struct xdr_stream *xdr, uint32_t *bitmap, uint32_t *res)
{
	__be32 *p;
	int status = 0;

	*res = 1024;
	if (unlikely(bitmap[0] & (FATTR4_WORD0_MAXREAD - 1U)))
		return -EIO;
	if (likely(bitmap[0] & FATTR4_WORD0_MAXREAD)) {
		uint64_t maxread;
		p = xdr_inline_decode(xdr, 8);
		if (unlikely(!p))
			goto out_overflow;
		xdr_decode_hyper(p, &maxread);
		if (maxread > 0x7FFFFFFF)
			maxread = 0x7FFFFFFF;
		*res = (uint32_t)maxread;
		bitmap[0] &= ~FATTR4_WORD0_MAXREAD;
	}
	dprintk("%s: maxread=%lu\n", __func__, (unsigned long)*res);
	return status;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}