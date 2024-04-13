static int decode_attr_fh_expire_type(struct xdr_stream *xdr,
				      uint32_t *bitmap, uint32_t *type)
{
	__be32 *p;

	*type = 0;
	if (unlikely(bitmap[0] & (FATTR4_WORD0_FH_EXPIRE_TYPE - 1U)))
		return -EIO;
	if (likely(bitmap[0] & FATTR4_WORD0_FH_EXPIRE_TYPE)) {
		p = xdr_inline_decode(xdr, 4);
		if (unlikely(!p))
			goto out_overflow;
		*type = be32_to_cpup(p);
		bitmap[0] &= ~FATTR4_WORD0_FH_EXPIRE_TYPE;
	}
	dprintk("%s: expire type=0x%x\n", __func__, *type);
	return 0;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}