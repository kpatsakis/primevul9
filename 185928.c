static void snd_timer_notify1(struct snd_timer_instance *ti, int event)
{
	struct snd_timer *timer;
	unsigned long flags;
	unsigned long resolution = 0;
	struct snd_timer_instance *ts;
	struct timespec tstamp;

	if (timer_tstamp_monotonic)
		ktime_get_ts(&tstamp);
	else
		getnstimeofday(&tstamp);
	if (snd_BUG_ON(event < SNDRV_TIMER_EVENT_START ||
		       event > SNDRV_TIMER_EVENT_PAUSE))
		return;
	if (event == SNDRV_TIMER_EVENT_START ||
	    event == SNDRV_TIMER_EVENT_CONTINUE)
		resolution = snd_timer_resolution(ti);
	if (ti->ccallback)
		ti->ccallback(ti, event, &tstamp, resolution);
	if (ti->flags & SNDRV_TIMER_IFLG_SLAVE)
		return;
	timer = ti->timer;
	if (timer == NULL)
		return;
	if (timer->hw.flags & SNDRV_TIMER_HW_SLAVE)
		return;
	spin_lock_irqsave(&timer->lock, flags);
	list_for_each_entry(ts, &ti->slave_active_head, active_list)
		if (ts->ccallback)
			ts->ccallback(ts, event + 100, &tstamp, resolution);
	spin_unlock_irqrestore(&timer->lock, flags);
}