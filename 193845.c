static void __set_sched_clock_stable(void)
{
	if (!sched_clock_stable())
		static_key_slow_inc(&__sched_clock_stable);
}