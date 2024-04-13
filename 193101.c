static int posix_clock_realtime_get(clockid_t which_clock, struct timespec64 *tp)
{
	ktime_get_real_ts64(tp);
	return 0;
}