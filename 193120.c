static int posix_ktime_get_ts(clockid_t which_clock, struct timespec64 *tp)
{
	ktime_get_ts64(tp);
	return 0;
}