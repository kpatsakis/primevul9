void sched_clock_tick(void)
{
	struct sched_clock_data *scd;
	u64 now, now_gtod;

	if (sched_clock_stable())
		return;

	if (unlikely(!sched_clock_running))
		return;

	WARN_ON_ONCE(!irqs_disabled());

	scd = this_scd();
	now_gtod = ktime_to_ns(ktime_get());
	now = sched_clock();

	scd->tick_raw = now;
	scd->tick_gtod = now_gtod;
	sched_clock_local(scd);
}