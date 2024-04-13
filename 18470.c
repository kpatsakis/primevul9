trigger_dyntick_cpu(struct timer_base *base, struct timer_list *timer)
{
	if (!is_timers_nohz_active())
		return;

	/*
	 * TODO: This wants some optimizing similar to the code below, but we
	 * will do that when we switch from push to pull for deferrable timers.
	 */
	if (timer->flags & TIMER_DEFERRABLE) {
		if (tick_nohz_full_cpu(base->cpu))
			wake_up_nohz_cpu(base->cpu);
		return;
	}

	/*
	 * We might have to IPI the remote CPU if the base is idle and the
	 * timer is not deferrable. If the other CPU is on the way to idle
	 * then it can't set base->is_idle as we hold the base lock:
	 */
	if (!base->is_idle)
		return;

	/* Check whether this is the new first expiring timer: */
	if (time_after_eq(timer->expires, base->next_expiry))
		return;

	/*
	 * Set the next expiry time and kick the CPU so it can reevaluate the
	 * wheel:
	 */
	if (time_before(timer->expires, base->clk)) {
		/*
		 * Prevent from forward_timer_base() moving the base->clk
		 * backward
		 */
		base->next_expiry = base->clk;
	} else {
		base->next_expiry = timer->expires;
	}
	wake_up_nohz_cpu(base->cpu);
}