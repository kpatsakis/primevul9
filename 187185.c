static int decode_attr_rdev(struct xdr_stream *xdr, uint32_t *bitmap, dev_t *rdev)
{
	uint32_t major = 0, minor = 0;
	__be32 *p;
	int ret = 0;

	*rdev = MKDEV(0,0);
	if (unlikely(bitmap[1] & (FATTR4_WORD1_RAWDEV - 1U)))
		return -EIO;
	if (likely(bitmap[1] & FATTR4_WORD1_RAWDEV)) {
		dev_t tmp;

		p = xdr_inline_decode(xdr, 8);
		if (unlikely(!p))
			goto out_overflow;
		major = be32_to_cpup(p++);
		minor = be32_to_cpup(p);
		tmp = MKDEV(major, minor);
		if (MAJOR(tmp) == major && MINOR(tmp) == minor)
			*rdev = tmp;
		bitmap[1] &= ~ FATTR4_WORD1_RAWDEV;
		ret = NFS_ATTR_FATTR_RDEV;
	}
	dprintk("%s: rdev=(0x%x:0x%x)\n", __func__, major, minor);
	return ret;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}