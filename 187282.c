static int decode_attr_type(struct xdr_stream *xdr, uint32_t *bitmap, uint32_t *type)
{
	__be32 *p;
	int ret = 0;

	*type = 0;
	if (unlikely(bitmap[0] & (FATTR4_WORD0_TYPE - 1U)))
		return -EIO;
	if (likely(bitmap[0] & FATTR4_WORD0_TYPE)) {
		p = xdr_inline_decode(xdr, 4);
		if (unlikely(!p))
			goto out_overflow;
		*type = be32_to_cpup(p);
		if (*type < NF4REG || *type > NF4NAMEDATTR) {
			dprintk("%s: bad type %d\n", __func__, *type);
			return -EIO;
		}
		bitmap[0] &= ~FATTR4_WORD0_TYPE;
		ret = NFS_ATTR_FATTR_TYPE;
	}
	dprintk("%s: type=0%o\n", __func__, nfs_type2fmt[*type]);
	return ret;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}