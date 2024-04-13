static int collect_expired_timers(struct timer_base *base,
				  struct hlist_head *heads)
{
	unsigned long now = READ_ONCE(jiffies);

	/*
	 * NOHZ optimization. After a long idle sleep we need to forward the
	 * base to current jiffies. Avoid a loop by searching the bitfield for
	 * the next expiring timer.
	 */
	if ((long)(now - base->clk) > 2) {
		unsigned long next = __next_timer_interrupt(base);

		/*
		 * If the next timer is ahead of time forward to current
		 * jiffies, otherwise forward to the next expiry time:
		 */
		if (time_after(next, now)) {
			/*
			 * The call site will increment base->clk and then
			 * terminate the expiry loop immediately.
			 */
			base->clk = now;
			return 0;
		}
		base->clk = next;
	}
	return __collect_expired_timers(base, heads);
}