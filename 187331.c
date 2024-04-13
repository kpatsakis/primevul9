static int decode_first_pnfs_layout_type(struct xdr_stream *xdr,
					 uint32_t *layouttype)
{
	uint32_t *p;
	int num;

	p = xdr_inline_decode(xdr, 4);
	if (unlikely(!p))
		goto out_overflow;
	num = be32_to_cpup(p);

	/* pNFS is not supported by the underlying file system */
	if (num == 0) {
		*layouttype = 0;
		return 0;
	}
	if (num > 1)
		printk(KERN_INFO "NFS: %s: Warning: Multiple pNFS layout "
			"drivers per filesystem not supported\n", __func__);

	/* Decode and set first layout type, move xdr->p past unused types */
	p = xdr_inline_decode(xdr, num * 4);
	if (unlikely(!p))
		goto out_overflow;
	*layouttype = be32_to_cpup(p);
	return 0;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}