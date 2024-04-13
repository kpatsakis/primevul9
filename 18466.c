static inline void forward_timer_base(struct timer_base *base)
{
#ifdef CONFIG_NO_HZ_COMMON
	unsigned long jnow;

	/*
	 * We only forward the base when we are idle or have just come out of
	 * idle (must_forward_clk logic), and have a delta between base clock
	 * and jiffies. In the common case, run_timers will take care of it.
	 */
	if (likely(!base->must_forward_clk))
		return;

	jnow = READ_ONCE(jiffies);
	base->must_forward_clk = base->is_idle;
	if ((long)(jnow - base->clk) < 2)
		return;

	/*
	 * If the next expiry value is > jiffies, then we fast forward to
	 * jiffies otherwise we forward to the next expiry value.
	 */
	if (time_after(base->next_expiry, jnow)) {
		base->clk = jnow;
	} else {
		if (WARN_ON_ONCE(time_before(base->next_expiry, base->clk)))
			return;
		base->clk = base->next_expiry;
	}
#endif
}