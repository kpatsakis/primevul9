static int decode_attr_mounted_on_fileid(struct xdr_stream *xdr, uint32_t *bitmap, uint64_t *fileid)
{
	__be32 *p;
	int ret = 0;

	*fileid = 0;
	if (unlikely(bitmap[1] & (FATTR4_WORD1_MOUNTED_ON_FILEID - 1U)))
		return -EIO;
	if (likely(bitmap[1] & FATTR4_WORD1_MOUNTED_ON_FILEID)) {
		p = xdr_inline_decode(xdr, 8);
		if (unlikely(!p))
			goto out_overflow;
		xdr_decode_hyper(p, fileid);
		bitmap[1] &= ~FATTR4_WORD1_MOUNTED_ON_FILEID;
		ret = NFS_ATTR_FATTR_MOUNTED_ON_FILEID;
	}
	dprintk("%s: fileid=%Lu\n", __func__, (unsigned long long)*fileid);
	return ret;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}