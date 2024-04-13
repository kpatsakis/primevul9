static int decode_attr_lease_time(struct xdr_stream *xdr, uint32_t *bitmap, uint32_t *res)
{
	__be32 *p;

	*res = 60;
	if (unlikely(bitmap[0] & (FATTR4_WORD0_LEASE_TIME - 1U)))
		return -EIO;
	if (likely(bitmap[0] & FATTR4_WORD0_LEASE_TIME)) {
		p = xdr_inline_decode(xdr, 4);
		if (unlikely(!p))
			goto out_overflow;
		*res = be32_to_cpup(p);
		bitmap[0] &= ~FATTR4_WORD0_LEASE_TIME;
	}
	dprintk("%s: file size=%u\n", __func__, (unsigned int)*res);
	return 0;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}