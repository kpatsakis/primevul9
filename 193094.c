static int posix_get_tai(clockid_t which_clock, struct timespec64 *tp)
{
	timekeeping_clocktai64(tp);
	return 0;
}