static enum hrtimer_restart posix_timer_fn(struct hrtimer *timer)
{
	struct k_itimer *timr;
	unsigned long flags;
	int si_private = 0;
	enum hrtimer_restart ret = HRTIMER_NORESTART;

	timr = container_of(timer, struct k_itimer, it.real.timer);
	spin_lock_irqsave(&timr->it_lock, flags);

	timr->it_active = 0;
	if (timr->it_interval != 0)
		si_private = ++timr->it_requeue_pending;

	if (posix_timer_event(timr, si_private)) {
		/*
		 * signal was not sent because of sig_ignor
		 * we will not get a call back to restart it AND
		 * it should be restarted.
		 */
		if (timr->it_interval != 0) {
			ktime_t now = hrtimer_cb_get_time(timer);

			/*
			 * FIXME: What we really want, is to stop this
			 * timer completely and restart it in case the
			 * SIG_IGN is removed. This is a non trivial
			 * change which involves sighand locking
			 * (sigh !), which we don't want to do late in
			 * the release cycle.
			 *
			 * For now we just let timers with an interval
			 * less than a jiffie expire every jiffie to
			 * avoid softirq starvation in case of SIG_IGN
			 * and a very small interval, which would put
			 * the timer right back on the softirq pending
			 * list. By moving now ahead of time we trick
			 * hrtimer_forward() to expire the timer
			 * later, while we still maintain the overrun
			 * accuracy, but have some inconsistency in
			 * the timer_gettime() case. This is at least
			 * better than a starved softirq. A more
			 * complex fix which solves also another related
			 * inconsistency is already in the pipeline.
			 */
#ifdef CONFIG_HIGH_RES_TIMERS
			{
				ktime_t kj = NSEC_PER_SEC / HZ;

				if (timr->it_interval < kj)
					now = ktime_add(now, kj);
			}
#endif
			timr->it_overrun += (unsigned int)
				hrtimer_forward(timer, now,
						timr->it_interval);
			ret = HRTIMER_RESTART;
			++timr->it_requeue_pending;
			timr->it_active = 1;
		}
	}

	unlock_timer(timr, flags);
	return ret;
}