static int posix_get_realtime_coarse(clockid_t which_clock, struct timespec64 *tp)
{
	*tp = current_kernel_time64();
	return 0;
}