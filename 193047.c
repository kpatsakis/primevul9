static int posix_get_monotonic_raw(clockid_t which_clock, struct timespec64 *tp)
{
	getrawmonotonic64(tp);
	return 0;
}