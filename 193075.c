timespec64_to_jiffies(const struct timespec64 *value)
{
	return __timespec64_to_jiffies(value->tv_sec, value->tv_nsec);
}