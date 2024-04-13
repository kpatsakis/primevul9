static int decode_attr_time(struct xdr_stream *xdr, struct timespec *time)
{
	__be32 *p;
	uint64_t sec;
	uint32_t nsec;

	p = xdr_inline_decode(xdr, 12);
	if (unlikely(!p))
		goto out_overflow;
	p = xdr_decode_hyper(p, &sec);
	nsec = be32_to_cpup(p);
	time->tv_sec = (time_t)sec;
	time->tv_nsec = (long)nsec;
	return 0;
out_overflow:
	print_overflow_msg(__func__, xdr);
	return -EIO;
}