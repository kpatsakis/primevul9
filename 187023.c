static int decode_attr_maxname(struct xdr_stream *xdr, uint32_t *bitmap, uint32_t *maxname)
{
	__be32 *p;
	int status = 0;

	*maxname = 1024;
	if (unlikely(bitmap[0] & (FATTR4_WORD0_MAXNAME - 1U)))
		return -EIO;
	if (likely(bitmap[0] & FATTR4_WORD0_MAXNAME)) {
		p = xdr_inline_decode(xdr, 4);
		if (unlikely(!p))
			goto out_overflow;
		*maxname = be32_to_cpup(p);
		bitmap[0] &= ~FATTR4_WORD0_MAXNAME;
	}
	dprintk("%s: maxname=%u\n", __func__, *maxname);
	return status;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}