int sched_clock_stable(void)
{
	return static_key_false(&__sched_clock_stable);
}