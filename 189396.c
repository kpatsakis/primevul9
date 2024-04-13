void snd_timer_notify(struct snd_timer *timer, int event, struct timespec *tstamp)
{
	unsigned long flags;
	unsigned long resolution = 0;
	struct snd_timer_instance *ti, *ts;

	if (timer->card && timer->card->shutdown)
		return;
	if (! (timer->hw.flags & SNDRV_TIMER_HW_SLAVE))
		return;
	if (snd_BUG_ON(event < SNDRV_TIMER_EVENT_MSTART ||
		       event > SNDRV_TIMER_EVENT_MRESUME))
		return;
	spin_lock_irqsave(&timer->lock, flags);
	if (event == SNDRV_TIMER_EVENT_MSTART ||
	    event == SNDRV_TIMER_EVENT_MCONTINUE ||
	    event == SNDRV_TIMER_EVENT_MRESUME)
		resolution = snd_timer_hw_resolution(timer);
	list_for_each_entry(ti, &timer->active_list_head, active_list) {
		if (ti->ccallback)
			ti->ccallback(ti, event, tstamp, resolution);
		list_for_each_entry(ts, &ti->slave_active_head, active_list)
			if (ts->ccallback)
				ts->ccallback(ts, event, tstamp, resolution);
	}
	spin_unlock_irqrestore(&timer->lock, flags);
}