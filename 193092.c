timeval_to_jiffies(const struct timeval *value)
{
	return __timespec_to_jiffies(value->tv_sec,
				     value->tv_usec * NSEC_PER_USEC);
}