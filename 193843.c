static void __clear_sched_clock_stable(struct work_struct *work)
{
	/* XXX worry about clock continuity */
	if (sched_clock_stable())
		static_key_slow_dec(&__sched_clock_stable);
}