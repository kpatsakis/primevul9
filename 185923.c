static int _snd_timer_stop(struct snd_timer_instance *timeri, int event)
{
	struct snd_timer *timer;
	unsigned long flags;

	if (snd_BUG_ON(!timeri))
		return -ENXIO;

	if (timeri->flags & SNDRV_TIMER_IFLG_SLAVE) {
		spin_lock_irqsave(&slave_active_lock, flags);
		if (!(timeri->flags & SNDRV_TIMER_IFLG_RUNNING)) {
			spin_unlock_irqrestore(&slave_active_lock, flags);
			return -EBUSY;
		}
		if (timeri->timer)
			spin_lock(&timeri->timer->lock);
		timeri->flags &= ~SNDRV_TIMER_IFLG_RUNNING;
		list_del_init(&timeri->ack_list);
		list_del_init(&timeri->active_list);
		if (timeri->timer)
			spin_unlock(&timeri->timer->lock);
		spin_unlock_irqrestore(&slave_active_lock, flags);
		goto __end;
	}
	timer = timeri->timer;
	if (!timer)
		return -EINVAL;
	spin_lock_irqsave(&timer->lock, flags);
	if (!(timeri->flags & (SNDRV_TIMER_IFLG_RUNNING |
			       SNDRV_TIMER_IFLG_START))) {
		spin_unlock_irqrestore(&timer->lock, flags);
		return -EBUSY;
	}
	list_del_init(&timeri->ack_list);
	list_del_init(&timeri->active_list);
	if (timer->card && timer->card->shutdown) {
		spin_unlock_irqrestore(&timer->lock, flags);
		return 0;
	}
	if ((timeri->flags & SNDRV_TIMER_IFLG_RUNNING) &&
	    !(--timer->running)) {
		timer->hw.stop(timer);
		if (timer->flags & SNDRV_TIMER_FLG_RESCHED) {
			timer->flags &= ~SNDRV_TIMER_FLG_RESCHED;
			snd_timer_reschedule(timer, 0);
			if (timer->flags & SNDRV_TIMER_FLG_CHANGE) {
				timer->flags &= ~SNDRV_TIMER_FLG_CHANGE;
				timer->hw.start(timer);
			}
		}
	}
	timeri->flags &= ~(SNDRV_TIMER_IFLG_RUNNING | SNDRV_TIMER_IFLG_START);
	spin_unlock_irqrestore(&timer->lock, flags);
      __end:
	if (event != SNDRV_TIMER_EVENT_RESOLUTION)
		snd_timer_notify1(timeri, event);
	return 0;
}