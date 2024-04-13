static int decode_attr_time_modify(struct xdr_stream *xdr, uint32_t *bitmap, struct timespec *time)
{
	int status = 0;

	time->tv_sec = 0;
	time->tv_nsec = 0;
	if (unlikely(bitmap[1] & (FATTR4_WORD1_TIME_MODIFY - 1U)))
		return -EIO;
	if (likely(bitmap[1] & FATTR4_WORD1_TIME_MODIFY)) {
		status = decode_attr_time(xdr, time);
		if (status == 0)
			status = NFS_ATTR_FATTR_MTIME;
		bitmap[1] &= ~FATTR4_WORD1_TIME_MODIFY;
	}
	dprintk("%s: mtime=%ld\n", __func__, (long)time->tv_sec);
	return status;
}