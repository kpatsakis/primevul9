static int decode_attr_symlink_support(struct xdr_stream *xdr, uint32_t *bitmap, uint32_t *res)
{
	__be32 *p;

	*res = 0;
	if (unlikely(bitmap[0] & (FATTR4_WORD0_SYMLINK_SUPPORT - 1U)))
		return -EIO;
	if (likely(bitmap[0] & FATTR4_WORD0_SYMLINK_SUPPORT)) {
		p = xdr_inline_decode(xdr, 4);
		if (unlikely(!p))
			goto out_overflow;
		*res = be32_to_cpup(p);
		bitmap[0] &= ~FATTR4_WORD0_SYMLINK_SUPPORT;
	}
	dprintk("%s: symlink support=%s\n", __func__, *res == 0 ? "false" : "true");
	return 0;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}