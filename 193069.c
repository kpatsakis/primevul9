static int posix_get_monotonic_coarse(clockid_t which_clock,
						struct timespec64 *tp)
{
	*tp = get_monotonic_coarse64();
	return 0;
}