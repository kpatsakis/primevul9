static u64 sched_clock_local(struct sched_clock_data *scd)
{
	u64 now, clock, old_clock, min_clock, max_clock;
	s64 delta;

again:
	now = sched_clock();
	delta = now - scd->tick_raw;
	if (unlikely(delta < 0))
		delta = 0;

	old_clock = scd->clock;

	/*
	 * scd->clock = clamp(scd->tick_gtod + delta,
	 *		      max(scd->tick_gtod, scd->clock),
	 *		      scd->tick_gtod + TICK_NSEC);
	 */

	clock = scd->tick_gtod + delta;
	min_clock = wrap_max(scd->tick_gtod, old_clock);
	max_clock = wrap_max(old_clock, scd->tick_gtod + TICK_NSEC);

	clock = wrap_max(clock, min_clock);
	clock = wrap_min(clock, max_clock);

	if (cmpxchg64(&scd->clock, old_clock, clock) != old_clock)
		goto again;

	return clock;
}