static int decode_attr_time_delta(struct xdr_stream *xdr, uint32_t *bitmap,
				  struct timespec *time)
{
	int status = 0;

	time->tv_sec = 0;
	time->tv_nsec = 0;
	if (unlikely(bitmap[1] & (FATTR4_WORD1_TIME_DELTA - 1U)))
		return -EIO;
	if (likely(bitmap[1] & FATTR4_WORD1_TIME_DELTA)) {
		status = decode_attr_time(xdr, time);
		bitmap[1] &= ~FATTR4_WORD1_TIME_DELTA;
	}
	dprintk("%s: time_delta=%ld %ld\n", __func__, (long)time->tv_sec,
		(long)time->tv_nsec);
	return status;
}