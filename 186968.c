static int decode_attr_pnfstype(struct xdr_stream *xdr, uint32_t *bitmap,
				uint32_t *layouttype)
{
	int status = 0;

	dprintk("%s: bitmap is %x\n", __func__, bitmap[1]);
	if (unlikely(bitmap[1] & (FATTR4_WORD1_FS_LAYOUT_TYPES - 1U)))
		return -EIO;
	if (bitmap[1] & FATTR4_WORD1_FS_LAYOUT_TYPES) {
		status = decode_first_pnfs_layout_type(xdr, layouttype);
		bitmap[1] &= ~FATTR4_WORD1_FS_LAYOUT_TYPES;
	} else
		*layouttype = 0;
	return status;
}