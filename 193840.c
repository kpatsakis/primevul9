void clear_sched_clock_stable(void)
{
	__sched_clock_stable_early = 0;

	smp_mb(); /* matches sched_clock_init() */

	if (!sched_clock_running)
		return;

	schedule_work(&sched_clock_work);
}