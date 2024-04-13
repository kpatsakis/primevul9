static int decode_attr_time_metadata(struct xdr_stream *xdr, uint32_t *bitmap, struct timespec *time)
{
	int status = 0;

	time->tv_sec = 0;
	time->tv_nsec = 0;
	if (unlikely(bitmap[1] & (FATTR4_WORD1_TIME_METADATA - 1U)))
		return -EIO;
	if (likely(bitmap[1] & FATTR4_WORD1_TIME_METADATA)) {
		status = decode_attr_time(xdr, time);
		if (status == 0)
			status = NFS_ATTR_FATTR_CTIME;
		bitmap[1] &= ~FATTR4_WORD1_TIME_METADATA;
	}
	dprintk("%s: ctime=%ld\n", __func__, (long)time->tv_sec);
	return status;
}