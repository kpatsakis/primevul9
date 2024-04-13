static int common_nsleep(const clockid_t which_clock, int flags,
			 struct timespec64 *tsave)
{
	return hrtimer_nanosleep(tsave, flags & TIMER_ABSTIME ?
				 HRTIMER_MODE_ABS : HRTIMER_MODE_REL,
				 which_clock);
}