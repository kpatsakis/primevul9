static int decode_attr_change(struct xdr_stream *xdr, uint32_t *bitmap, uint64_t *change)
{
	__be32 *p;
	int ret = 0;

	*change = 0;
	if (unlikely(bitmap[0] & (FATTR4_WORD0_CHANGE - 1U)))
		return -EIO;
	if (likely(bitmap[0] & FATTR4_WORD0_CHANGE)) {
		p = xdr_inline_decode(xdr, 8);
		if (unlikely(!p))
			goto out_overflow;
		xdr_decode_hyper(p, change);
		bitmap[0] &= ~FATTR4_WORD0_CHANGE;
		ret = NFS_ATTR_FATTR_CHANGE;
	}
	dprintk("%s: change attribute=%Lu\n", __func__,
			(unsigned long long)*change);
	return ret;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}