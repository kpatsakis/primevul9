static int decode_attr_layout_blksize(struct xdr_stream *xdr, uint32_t *bitmap,
				      uint32_t *res)
{
	__be32 *p;

	dprintk("%s: bitmap is %x\n", __func__, bitmap[2]);
	*res = 0;
	if (bitmap[2] & FATTR4_WORD2_LAYOUT_BLKSIZE) {
		p = xdr_inline_decode(xdr, 4);
		if (unlikely(!p)) {
			print_overflow_msg(__func__, xdr);
			return -EIO;
		}
		*res = be32_to_cpup(p);
		bitmap[2] &= ~FATTR4_WORD2_LAYOUT_BLKSIZE;
	}
	return 0;
}