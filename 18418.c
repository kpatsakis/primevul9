static void del_timer_wait_running(struct timer_list *timer)
{
	u32 tf;

	tf = READ_ONCE(timer->flags);
	if (!(tf & TIMER_MIGRATING)) {
		struct timer_base *base = get_timer_base(tf);

		/*
		 * Mark the base as contended and grab the expiry lock,
		 * which is held by the softirq across the timer
		 * callback. Drop the lock immediately so the softirq can
		 * expire the next timer. In theory the timer could already
		 * be running again, but that's more than unlikely and just
		 * causes another wait loop.
		 */
		atomic_inc(&base->timer_waiters);
		spin_lock_bh(&base->expiry_lock);
		atomic_dec(&base->timer_waiters);
		spin_unlock_bh(&base->expiry_lock);
	}
}