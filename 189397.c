static int snd_timer_start1(struct snd_timer_instance *timeri,
			    bool start, unsigned long ticks)
{
	struct snd_timer *timer;
	int result;
	unsigned long flags;

	timer = timeri->timer;
	if (!timer)
		return -EINVAL;

	spin_lock_irqsave(&timer->lock, flags);
	if (timeri->flags & SNDRV_TIMER_IFLG_DEAD) {
		result = -EINVAL;
		goto unlock;
	}
	if (timer->card && timer->card->shutdown) {
		result = -ENODEV;
		goto unlock;
	}
	if (timeri->flags & (SNDRV_TIMER_IFLG_RUNNING |
			     SNDRV_TIMER_IFLG_START)) {
		result = -EBUSY;
		goto unlock;
	}

	if (start)
		timeri->ticks = timeri->cticks = ticks;
	else if (!timeri->cticks)
		timeri->cticks = 1;
	timeri->pticks = 0;

	list_move_tail(&timeri->active_list, &timer->active_list_head);
	if (timer->running) {
		if (timer->hw.flags & SNDRV_TIMER_HW_SLAVE)
			goto __start_now;
		timer->flags |= SNDRV_TIMER_FLG_RESCHED;
		timeri->flags |= SNDRV_TIMER_IFLG_START;
		result = 1; /* delayed start */
	} else {
		if (start)
			timer->sticks = ticks;
		timer->hw.start(timer);
	      __start_now:
		timer->running++;
		timeri->flags |= SNDRV_TIMER_IFLG_RUNNING;
		result = 0;
	}
	snd_timer_notify1(timeri, start ? SNDRV_TIMER_EVENT_START :
			  SNDRV_TIMER_EVENT_CONTINUE);
 unlock:
	spin_unlock_irqrestore(&timer->lock, flags);
	return result;
}