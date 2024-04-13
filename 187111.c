static int decode_attr_filehandle(struct xdr_stream *xdr, uint32_t *bitmap, struct nfs_fh *fh)
{
	__be32 *p;
	int len;

	if (fh != NULL)
		memset(fh, 0, sizeof(*fh));

	if (unlikely(bitmap[0] & (FATTR4_WORD0_FILEHANDLE - 1U)))
		return -EIO;
	if (likely(bitmap[0] & FATTR4_WORD0_FILEHANDLE)) {
		p = xdr_inline_decode(xdr, 4);
		if (unlikely(!p))
			goto out_overflow;
		len = be32_to_cpup(p);
		if (len > NFS4_FHSIZE)
			return -EIO;
		p = xdr_inline_decode(xdr, len);
		if (unlikely(!p))
			goto out_overflow;
		if (fh != NULL) {
			memcpy(fh->data, p, len);
			fh->size = len;
		}
		bitmap[0] &= ~FATTR4_WORD0_FILEHANDLE;
	}
	return 0;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}