static int posix_get_boottime(const clockid_t which_clock, struct timespec64 *tp)
{
	get_monotonic_boottime64(tp);
	return 0;
}