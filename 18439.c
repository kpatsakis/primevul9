static void expire_timers(struct timer_base *base, struct hlist_head *head)
{
	/*
	 * This value is required only for tracing. base->clk was
	 * incremented directly before expire_timers was called. But expiry
	 * is related to the old base->clk value.
	 */
	unsigned long baseclk = base->clk - 1;

	while (!hlist_empty(head)) {
		struct timer_list *timer;
		void (*fn)(struct timer_list *);

		timer = hlist_entry(head->first, struct timer_list, entry);

		base->running_timer = timer;
		detach_timer(timer, true);

		fn = timer->function;

		if (timer->flags & TIMER_IRQSAFE) {
			raw_spin_unlock(&base->lock);
			call_timer_fn(timer, fn, baseclk);
			base->running_timer = NULL;
			raw_spin_lock(&base->lock);
		} else {
			raw_spin_unlock_irq(&base->lock);
			call_timer_fn(timer, fn, baseclk);
			base->running_timer = NULL;
			timer_sync_wait_running(base);
			raw_spin_lock_irq(&base->lock);
		}
	}
}