u64 get_next_timer_interrupt(unsigned long basej, u64 basem)
{
	struct timer_base *base = this_cpu_ptr(&timer_bases[BASE_STD]);
	u64 expires = KTIME_MAX;
	unsigned long nextevt;
	bool is_max_delta;

	/*
	 * Pretend that there is no timer pending if the cpu is offline.
	 * Possible pending timers will be migrated later to an active cpu.
	 */
	if (cpu_is_offline(smp_processor_id()))
		return expires;

	raw_spin_lock(&base->lock);
	nextevt = __next_timer_interrupt(base);
	is_max_delta = (nextevt == base->clk + NEXT_TIMER_MAX_DELTA);
	base->next_expiry = nextevt;
	/*
	 * We have a fresh next event. Check whether we can forward the
	 * base. We can only do that when @basej is past base->clk
	 * otherwise we might rewind base->clk.
	 */
	if (time_after(basej, base->clk)) {
		if (time_after(nextevt, basej))
			base->clk = basej;
		else if (time_after(nextevt, base->clk))
			base->clk = nextevt;
	}

	if (time_before_eq(nextevt, basej)) {
		expires = basem;
		base->is_idle = false;
	} else {
		if (!is_max_delta)
			expires = basem + (u64)(nextevt - basej) * TICK_NSEC;
		/*
		 * If we expect to sleep more than a tick, mark the base idle.
		 * Also the tick is stopped so any added timer must forward
		 * the base clk itself to keep granularity small. This idle
		 * logic is only maintained for the BASE_STD base, deferrable
		 * timers may still see large granularity skew (by design).
		 */
		if ((expires - basem) > TICK_NSEC) {
			base->must_forward_clk = true;
			base->is_idle = true;
		}
	}
	raw_spin_unlock(&base->lock);

	return cmp_next_hrtimer_event(basem, expires);
}