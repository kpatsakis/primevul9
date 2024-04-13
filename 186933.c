static int decode_attr_mdsthreshold(struct xdr_stream *xdr,
				    uint32_t *bitmap,
				    struct nfs4_threshold *res)
{
	__be32 *p;
	int status = 0;
	uint32_t num;

	if (unlikely(bitmap[2] & (FATTR4_WORD2_MDSTHRESHOLD - 1U)))
		return -EIO;
	if (bitmap[2] & FATTR4_WORD2_MDSTHRESHOLD) {
		/* Did the server return an unrequested attribute? */
		if (unlikely(res == NULL))
			return -EREMOTEIO;
		p = xdr_inline_decode(xdr, 4);
		if (unlikely(!p))
			goto out_overflow;
		num = be32_to_cpup(p);
		if (num == 0)
			return 0;
		if (num > 1)
			printk(KERN_INFO "%s: Warning: Multiple pNFS layout "
				"drivers per filesystem not supported\n",
				__func__);

		status = decode_first_threshold_item4(xdr, res);
		bitmap[2] &= ~FATTR4_WORD2_MDSTHRESHOLD;
	}
	return status;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}