static int posix_clock_realtime_adj(const clockid_t which_clock,
				    struct timex *t)
{
	return do_adjtimex(t);
}