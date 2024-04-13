static int decode_attr_fsid(struct xdr_stream *xdr, uint32_t *bitmap, struct nfs_fsid *fsid)
{
	__be32 *p;
	int ret = 0;

	fsid->major = 0;
	fsid->minor = 0;
	if (unlikely(bitmap[0] & (FATTR4_WORD0_FSID - 1U)))
		return -EIO;
	if (likely(bitmap[0] & FATTR4_WORD0_FSID)) {
		p = xdr_inline_decode(xdr, 16);
		if (unlikely(!p))
			goto out_overflow;
		p = xdr_decode_hyper(p, &fsid->major);
		xdr_decode_hyper(p, &fsid->minor);
		bitmap[0] &= ~FATTR4_WORD0_FSID;
		ret = NFS_ATTR_FATTR_FSID;
	}
	dprintk("%s: fsid=(0x%Lx/0x%Lx)\n", __func__,
			(unsigned long long)fsid->major,
			(unsigned long long)fsid->minor);
	return ret;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}