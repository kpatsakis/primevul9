void run_local_timers(void)
{
	struct timer_base *base = this_cpu_ptr(&timer_bases[BASE_STD]);

	hrtimer_run_queues();
	/* Raise the softirq only if required. */
	if (time_before(jiffies, base->clk)) {
		if (!IS_ENABLED(CONFIG_NO_HZ_COMMON))
			return;
		/* CPU is awake, so check the deferrable base. */
		base++;
		if (time_before(jiffies, base->clk))
			return;
	}
	raise_softirq(TIMER_SOFTIRQ);
}